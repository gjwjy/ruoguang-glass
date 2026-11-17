// 柔光玻璃 —— 独立实现的原生 LSPosed 模块
// 思路：不重写桌面的渲染逻辑，只让它自带的“原生玻璃宿主”真正被走到，
// 同时完全不触碰任何着色/灰度路径，所以组件内容保持原色。
#define _GNU_SOURCE
#include <android/log.h>
#include <dlfcn.h>
#include <elf.h>
#include <errno.h>
#include <link.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <pthread.h>
#include <sys/syscall.h>
#include <unistd.h>

#define TAG "LSPosed"
#define LOG(...) __android_log_print(ANDROID_LOG_ERROR, TAG, "RG: " __VA_ARGS__)

typedef int (*HookFn)(void *, void *, void **);
typedef int (*UnhookFn)(void *);
typedef struct { uint32_t version; HookFn hook; UnhookFn unhook; } Api;

static HookFn g_hook;
static int g_patched;

/* 模块一被 dlopen 就留下痕迹，便于判断“没加载”还是“加载了没生效” */
__attribute__((constructor)) static void on_load(void) {
  LOG("library loaded pid=%d", getpid());
}

/* 一处补丁：把 4 字节指令写到镜像内的固定偏移 */
struct fix { const char *lib; uintptr_t off; uint32_t word; const char *why; };

/* 全部为自包含的短指令，不含跳往代码洞的分支，因此无需额外搬运代码：
   0xD503201F = NOP（跳过“不支持玻璃就返回”的判断）
   0x14000008 = B +0x20（跳过一段兜底分支） */
static const struct fix kFixes[] = {
  {0, 0, 0, "no verified site for this build"},
};

static int cmdline_is(const char *want) {
  char buf[96] = {0};
  int fd = (int)syscall(SYS_openat, AT_FDCWD, "/proc/self/cmdline", O_RDONLY | O_CLOEXEC, 0);
  if (fd < 0) return 0;
  ssize_t n = syscall(SYS_read, fd, buf, sizeof(buf) - 1);
  syscall(SYS_close, fd);
  return n > 0 && strcmp(buf, want) == 0;
}

/* 用链接器自己维护的链表取基址（apk 内嵌的 so 只有这里给的值才是准的） */
static uintptr_t g_base;
static int phdr_cb(struct dl_phdr_info *info, size_t size, void *data) {
  (void)size; (void)data;
  const char *n = info->dlpi_name ? info->dlpi_name : "";
  if (!strstr(n, "/libapp.so")) return 0;
  g_base = (uintptr_t)info->dlpi_addr;
  return 1;
}

static uintptr_t find_base(void) {
  g_base = 0;
  dl_iterate_phdr(phdr_cb, NULL);
  return g_base;
}

static int apply_all(uintptr_t base) {
  size_t done = 0, same = 0;
  for (size_t i = 0; i < sizeof(kFixes) / sizeof(kFixes[0]); i++) {
    const struct fix *f = &kFixes[i];
    if (!f->off) continue;
    uintptr_t at = base + f->off;
    uintptr_t page = at & ~(uintptr_t)(4095);
    if (mprotect((void *)page, 4096, PROT_READ | PROT_WRITE | PROT_EXEC) != 0) {
      LOG("mprotect fail %#lx: %s", (unsigned long)f->off, strerror(errno));
      continue;
    }
    if (*(volatile uint32_t *)at == f->word) { same++; continue; }
    *(volatile uint32_t *)at = f->word;
    __builtin___clear_cache((char *)at, (char *)(at + 4));
    LOG("patch %#lx -> %08x (%s)", (unsigned long)f->off, f->word, f->why);
    done++;
  }
  LOG("libapp patched base=%#lx written=%zu already=%zu",
      (unsigned long)base, done, same);
  return 1;
}

static void *watcher(void *arg) {
  (void)arg;
  for (int i = 0; i < 30000; i++) {         /* 最长等 5 分钟 */
    uintptr_t base = find_base();
    if (base) { apply_all(base); return NULL; }
    if (i && i % 500 == 0) LOG("watcher %d: still no libapp", i);
    usleep(10000);
  }
  LOG("libapp never mapped, give up");
  return NULL;
}

__attribute__((visibility("default"), used))
int native_init(const Api *api) {
  LOG("native_init api=%p version=%u", (const void *)api, api ? *(const uint32_t *)api : 0u);
  if (!api) return 0;
  g_hook = api->hook;
  if (!cmdline_is("com.miui.home")) { LOG("skip pid=%d", getpid()); return 0; }

  uintptr_t base = find_base();
  if (base) { apply_all(base); return 0; }

  LOG("libapp not mapped yet, start watcher");
  pthread_t th;
  if (pthread_create(&th, NULL, watcher, NULL) == 0) pthread_detach(th);
  else LOG("pthread_create failed");
  return 0;
}
