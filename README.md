# 柔光玻璃（RuoGuang Glass）

小米 HyperOS 4（Android 17）系统桌面的「柔光玻璃」材质 LSPosed 原生模块。

- 作用域：`com.miui.home`
- 形态：无界面、无桌面图标的纯原生模块（`hasCode=false`，`extractNativeLibs=false`）
- 包名：`com.haotian.glass`

## 效果

- 桌面小组件（MAML 渲染）改为柔光玻璃材质，透出壁纸
- 文件夹底板使用玻璃预设
- 保留原版彩色图标 / 圆环 / 图片饱和度
- 文字白色

## 安装

1. 手机已 root + KernelSU/Magisk + LSPosed（需支持 HyperOS Runtime 原生模块）
2. 安装 `release/` 下的 APK
3. LSPosed 中启用「柔光玻璃」，作用域勾选「系统桌面」
4. 重启桌面进程：`su -c "am force-stop com.miui.home"`

## 本仓库内容

| 目录 | 说明 |
|---|---|
| `native/` | 自研 Native 模块源码（`native_init` 入口、`dl_iterate_phdr` 基址定位、补丁写入） |
| `module/META-INF/xposed/` | LSPosed 原生模块描述文件（`module.prop` / `native_init.list` / `scope.list`） |
| `AndroidManifest.xml` | 模块清单 |
| `build.sh` | 打包脚本（aapt2 → zip -0 存储 so → zipalign -P 16 → apksigner） |
| `release/` | 可安装产物 |

## 构建

```bash
ANDROID_HOME=/path/to/android-sdk ./build.sh
```

关键点：`.so` 必须以 **Stored（不压缩）** 方式进包并 16KB 对齐，否则 LSPosed 会报
`dlopen failed: .../base.apk!/lib/arm64-v8a/librg.so not found`。

## 说明

- 发布版 APK 使用的玻璃渲染配方来自已验证的兼容内核；本仓库的 `native/` 是可独立编译的模块壳层，用于承载后续自研补丁。
- 不同 launcher 版本的 `libapp.so` 布局不同，补丁地址必须逐版核对，写入前需校验原指令。

## 签名

发布版使用本项目自有证书签名：

```
SHA-256: 11fb8f1d7322fdadfc04815c01bc528ffc659875c2a5f4e6889f47d47e7c2d80
```

如果手机已安装旧证书版本，需先卸载再安装本版。

## 许可

MIT
