#clear
rm *.o

OUT_FILE="out/main"
DEFINES=(-DSIMULATOR -DSINGLE_THREADED -DHACK_ALERT -DFULL_LANGUAGE_SUPPORT -DFULL_LTS -DNO_INDEXES -DDIRECT_LTS_INPUT -DACNA -DHOMOGRAPHS -DMATH_MODE -DDIVIDED_LTS_RULES -DOUTPUT_FILE -D__linux__ -DSINGLE_LANGUAGE -DMultiple_Sample_Rates)
CFLAGS=(--sysroot=/usr/aarch64-redhat-linux/sys-root/fc41/ -static -fPIC -I include -g -Wunused-function)

aarch64-linux-gnu-gcc ${DEFINES[@]} ${CFLAGS[@]} -c src/*.c
aarch64-linux-gnu-g++ ${DEFINES[@]} ${CFLAGS[@]} -c main.cpp -I /usr/lib/jvm/jre-21-openjdk/include/ -I/usr/lib/jvm/jre-21-openjdk/include/linux/

#ar rcs libepsonapi.a *.o
~/Android/Sdk/ndk/28.0.12433566/toolchains/llvm/prebuilt/linux-x86_64/bin/clang --target=aarch64-none-linux-android31 -shared -static-libgcc *.o -o libepsonapi.so -I include

