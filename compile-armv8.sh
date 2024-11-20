#clear
rm *.o

OUT_FILE="out/main"
DEFINES=(-DSIMULATOR -DSINGLE_THREADED -DHACK_ALERT -DFULL_LANGUAGE_SUPPORT -DFULL_LTS -DNO_INDEXES -DDIRECT_LTS_INPUT -DACNA -DHOMOGRAPHS -DMATH_MODE -DDIVIDED_LTS_RULES -DOUTPUT_FILE -D__linux__)
CFLAGS=(-static -fPIC -I include -g -Wunused-function)

#CC=(~/Android/Sdk/ndk/28.0.12433566/toolchains/llvm/prebuilt/linux-x86_64/bin/clang --target=aarch64-none-linux-android31)
#CPP=(~/Android/Sdk/ndk/28.0.12433566/toolchains/llvm/prebuilt/linux-x86_64/bin/clang++ --target=aarch64-none-linux-android31)

CC=(~/x-tools/arm-unknown-linux-gnueabi/bin/arm-unknown-linux-gnueabi-gcc)
CPP=(~/x-tools/arm-unknown-linux-gnueabi/bin/arm-unknown-linux-gnueabi-g++)

${CC[@]} ${DEFINES[@]} ${CFLAGS[@]} -c src/*.c
${CPP[@]} ${DEFINES[@]} ${CFLAGS[@]} -c main.cpp -I /usr/lib/jvm/jre-21-openjdk/include/ -I/usr/lib/jvm/jre-21-openjdk/include/linux/

~/Android/Sdk/ndk/28.0.12433566/toolchains/llvm/prebuilt/linux-x86_64/bin/clang --target=armv8-none-linux-android31 -fPIC -shared *.o -o libepsonapi.so -I include

#${CPP[@]} -fPIC -shared -static-libgcc -static-libstdc++ *.o -o libepsonapi.so -fno-PIE
