package dev.hyperglass.lsposed;

import android.app.Activity;
import android.content.Intent;
import android.graphics.Color;
import android.net.Uri;
import android.os.Bundle;
import android.text.TextUtils;
import android.util.TypedValue;
import android.view.Gravity;
import android.view.View;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.ScrollView;
import android.widget.TextView;

public class InfoActivity extends Activity {
    private static final String REPO = "https://github.com/gjwjy/ruoguang-glass";
    private static final String FINGERPRINT = "11:FB:8F:1D:73:22:FD:AD:FC:04:81:5C:01:BC:52:8F:FC:65:98:75:C2:A5:F4:E6:88:9F:47:D4:7E:7C:2D:80";

    private TextView text(String s, float size, int color, boolean bold) {
        TextView t = new TextView(this);
        t.setText(s);
        t.setTextSize(TypedValue.COMPLEX_UNIT_SP, size);
        t.setTextColor(color);
        if (bold) t.setTypeface(t.getTypeface(), android.graphics.Typeface.BOLD);
        int p = (int) (getResources().getDisplayMetrics().density * 16);
        t.setPadding(p, p / 2, p, p / 2);
        return t;
    }

    @Override
    protected void onCreate(Bundle b) {
        super.onCreate(b);
        int fg = 0xFF1B1B1B;
        int dim = 0xFF6B6B6B;
        LinearLayout root = new LinearLayout(this);
        root.setOrientation(LinearLayout.VERTICAL);
        root.setBackgroundColor(0xFFF5F5F7);
        int p = (int) (getResources().getDisplayMetrics().density * 12);

        root.addView(text("柔光玻璃", 28, fg, true));
        root.addView(text("HyperOS 4 桌面玻璃材质 · LSPosed 原生模块", 13, dim, false));

        root.addView(text("效果", 16, fg, true));
        root.addView(text("· 桌面小组件与文件夹改为柔光玻璃材质，透出壁纸\n"
                + "· 保留原版彩色图标、圆环与图片饱和度\n"
                + "· 小组件文字白色\n"
                + "· 无多余界面，不占用桌面图标之外的入口", 14, fg, false));

        root.addView(text("使用方法", 16, fg, true));
        root.addView(text("1. 手机需已 root，并安装 LSPosed（支持 HyperOS Runtime 原生模块）\n"
                + "2. 在 LSPosed 中启用「柔光玻璃」\n"
                + "3. 作用域勾选「系统桌面」(com.miui.home)\n"
                + "4. 重启桌面进程：\n"
                + "   su -c \"am force-stop com.miui.home\"\n"
                + "5. 回到桌面，卡片与文件夹即为玻璃材质", 14, fg, false));

        root.addView(text("安装 / 升级", 16, fg, true));
        root.addView(text("· 若已安装旧证书版本，需先卸载再安装本版（签名不同）\n"
                + "· 覆盖安装后请重新确认作用域并重启桌面进程\n"
                + "· 卸载模块不会修改系统文件，效果随模块停用消失", 14, fg, false));

        root.addView(text("说明", 16, fg, true));
        root.addView(text("· 日志：LSPosed 日志中过滤 tag「LSPosed」、关键字「RG:」，"
                + "出现 libapp patched written=5 表示补丁已命中\n"
                + "· 不同桌面版本的补丁地址不同，升级系统后可能失效\n"
                + "· 桌面图标若显示为系统单色样式，属于图标主题行为，可在主题/图标包中调整", 14, fg, false));

        root.addView(text("签名指纹", 16, fg, true));
        root.addView(text(FINGERPRINT, 11, dim, false));
        root.addView(text("SHA-256", 11, dim, false));

        Button open = new Button(this);
        open.setText("打开 GitHub 仓库");
        open.setAllCaps(false);
        open.setOnClickListener(new View.OnClickListener() {
            @Override public void onClick(View v) {
                try {
                    startActivity(new Intent(Intent.ACTION_VIEW, Uri.parse(REPO)));
                } catch (Exception ignored) {
                }
            }
        });
        LinearLayout.LayoutParams lp = new LinearLayout.LayoutParams(
                LinearLayout.LayoutParams.MATCH_PARENT, LinearLayout.LayoutParams.WRAP_CONTENT);
        lp.setMargins(p, p, p, p);
        root.addView(open, lp);

        TextView link = text(REPO, 13, 0xFF3A6FF7, false);
        link.setOnClickListener(new View.OnClickListener() {
            @Override public void onClick(View v) {
                try {
                    startActivity(new Intent(Intent.ACTION_VIEW, Uri.parse(REPO)));
                } catch (Exception ignored) {
                }
            }
        });
        root.addView(link);

        ScrollView sv = new ScrollView(this);
        sv.addView(root);
        setContentView(sv);
    }
}
