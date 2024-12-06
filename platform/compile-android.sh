#!/bin/bash

# compile defines and flags
DEFINES=(-DSIMULATOR -DSINGLE_THREADED -DHACK_ALERT -DFULL_LANGUAGE_SUPPORT -DFULL_LTS -DNO_INDEXES -DDIRECT_LTS_INPUT -DACNA -DHOMOGRAPHS -DMATH_MODE -DDIVIDED_LTS_RULES -DOUTPUT_FILE -D__linux__ -DSINGLE_LANGUAGE -DMultiple_Sample_Rates)
CFLAGS=(-static -fPIC -I include -g -Wunused-function -w)

# delete previous outputs
rm -rf out/

# download toolchain
if [ ! -d "toolchain/aarch64-linux-musl-cross" ] || [ ! -d "toolchain/armel-linux-musleabi-cross" ] || [ ! -d "toolchain/x86_64-linux-musl-cross" ]; then
    rm -rf toolchain/*
    echo "[i] Downloading aarch64 Toolchain.."
    curl https://musl.cc/aarch64-linux-musl-cross.tgz | tar zxf - -C toolchain

    echo "[i] Downloading armv8 Toolchain..."
    curl https://musl.cc/armel-linux-musleabi-cross.tgz | tar zxf - -C toolchain

    echo "[i] Downloading x86_64 Toolchain..."
    curl https://musl.cc/x86_64-linux-musl-cross.tgz | tar zxf - -C toolchain
else
    echo "[i] Toolchain found, proceeding..."
fi

# compile source
compile_arch() {
    rm -rf *.so
    ARCH=$1
    echo "[i] compiling for ${ARCH}..."
    [[ $ARCH = "armel" ]] && MUSL="musleabi" || MUSL="musl"

    # build objects
    toolchain/${ARCH}-linux-$MUSL-cross/bin/${ARCH}-linux-$MUSL-gcc ${DEFINES[@]} ${CFLAGS[@]} -c src/dtk/*.c
    toolchain/${ARCH}-linux-$MUSL-cross/bin/${ARCH}-linux-$MUSL-g++ ${DEFINES[@]} ${CFLAGS[@]} -c src/main.cpp -I /usr/lib/jvm/jre-21-openjdk/include/ -I/usr/lib/jvm/jre-21-openjdk/include/linux/
    # link c and c++ together
    toolchain/${ARCH}-linux-$MUSL-cross/bin/${ARCH}-linux-$MUSL-gcc -fPIC -static -shared -static-libgcc *.o -o libepsonapi.so

    rm -rf *.o
    # copy binaries to destination
    DEST=$2
    mkdir -p $DEST
    mv libepsonapi.so $2
    cp -r $2 platform/App/app/src/main/jniLibs/
}

# build the ndk libraries for each platform (arch, output_dir)
compile_arch "aarch64" "out/android/arm64-v8a/"
compile_arch "armel" "out/android/armeabi-v7a/"
compile_arch "x86_64" "out/android/x86_64/"

# compile the android app itself
cd platform/App
./gradlew assembleDebug
cd ../../
cp platform/App/app/build/outputs/apk/debug/app-universal-debug.apk out/android/ # final resulting apk
