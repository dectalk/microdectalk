DEFINES=(-DSIMULATOR -DSINGLE_THREADED -DHACK_ALERT -DFULL_LANGUAGE_SUPPORT -DFULL_LTS -DNO_INDEXES -DDIRECT_LTS_INPUT -DACNA -DHOMOGRAPHS -DMATH_MODE -DDIVIDED_LTS_RULES -DOUTPUT_FILE -D__linux__ -DSINGLE_LANGUAGE -DMultiple_Sample_Rates)
CFLAGS=(-I include -g -w -lm)

mkdir -p out/linux

gcc ${DEFINES[@]} ${CFLAGS[@]} src/*.c platform/linux_bindings.c -o out/linux/main
