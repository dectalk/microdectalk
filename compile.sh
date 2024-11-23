#!/bin/bash

# compile defines and flags
DEFINES=(-DSIMULATOR -DSINGLE_THREADED -DHACK_ALERT -DFULL_LANGUAGE_SUPPORT -DFULL_LTS -DNO_INDEXES -DDIRECT_LTS_INPUT -DACNA -DHOMOGRAPHS -DMATH_MODE -DDIVIDED_LTS_RULES -DOUTPUT_FILE -D__linux__ -DSINGLE_LANGUAGE -DMultiple_Sample_Rates)
CFLAGS=(-static -fPIC -I include -g -Wunused-function -w)

# delete previous outputs
rm -rf out/
mkdir -p out/arm64-v8a
mkdir -p out/armeabi-v7a
mkdir -p toolchain

# download toolchain
if [ ! -d "toolchain/aarch64-linux-musl-cross" ] || [ ! -d "toolchain/armel-linux-musleabi-cross" ]; then
    rm -rf toolchain/*
    echo "[i] Downloading aarch64 Toolchain.."
    curl https://musl.cc/aarch64-linux-musl-cross.tgz | tar zxf - -C toolchain

    echo "[i] Downloading armv8 Toolchain..."
    curl https://musl.cc/armel-linux-musleabi-cross.tgz | tar -zxf - -C toolchain
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
    mv libepsonapi.so $2
    cp -r $2 App/app/src/main/jniLibs/
}

# build the ndk libraries for each platform (arch, output_dir)
compile_arch "aarch64" "out/arm64-v8a/"
compile_arch "armel" "out/armeabi-v7a/"

# compile the android app itself
cd App
./gradlew build
cd ..
cp App/app/build/outputs/apk/release/app-release-unsigned.apk out/ # final resulting apk
