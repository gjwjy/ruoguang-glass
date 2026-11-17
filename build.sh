#!/usr/bin/env bash
set -euo pipefail
ANDROID_HOME=${ANDROID_HOME:?set ANDROID_HOME}
BT=${BUILD_TOOLS:-$(ls -d "$ANDROID_HOME"/build-tools/* | sort -V | tail -1)}
NDK=${NDK:-$(ls -d "$ANDROID_HOME"/ndk/* | sort -V | tail -1)}
JAR="$ANDROID_HOME/platforms/android-37.0/android.jar"
CC="$NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/aarch64-linux-android35-clang"
KS=${KS:?set KS to your keystore path}
ALIAS=${ALIAS:-ruoguang}
PASS=${PASS:?set PASS}

rm -rf build && mkdir -p build/lib/arm64-v8a build/stage
"$CC" -shared -fPIC -O2 -fvisibility=hidden -Wl,-z,max-page-size=16384 \
  -o build/lib/arm64-v8a/librg.so native/glass.c -llog -ldl

"$BT/aapt2" link -o build/base.apk --manifest AndroidManifest.xml -I "$JAR" \
  --min-sdk-version 35 --target-sdk-version 37
cp build/base.apk build/unsigned.apk
cp -r module/META-INF build/stage/
( cd build/stage && zip -q -r ../unsigned.apk META-INF )
( cd build && zip -q -0 unsigned.apk lib/arm64-v8a/librg.so )
"$BT/zipalign" -P 16 -f 4 build/unsigned.apk build/aligned.apk
"$BT/apksigner" sign --ks "$KS" --ks-key-alias "$ALIAS" \
  --ks-pass "pass:$PASS" --key-pass "pass:$PASS" \
  --min-sdk-version 35 --v1-signing-enabled false --v2-signing-enabled true \
  --out release/柔光玻璃.apk build/aligned.apk
echo "built: release/柔光玻璃.apk"
