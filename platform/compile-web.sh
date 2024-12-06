source ~/emsdk/emsdk_env.sh

mkdir -p out/web/
rm -rf out/web/*
cp platform/index.html out/web

emcc src/dtk/*.c src/tts_bindings.cpp \
     -lembind \
     -s WASM=1 \
     -s DEMANGLE_SUPPORT=1 \
     -s ALLOW_MEMORY_GROWTH=1 \
     -g -w -ferror-limit=9999 \
     -o0 \
     -gsource-map \
     -I include \
     -o out/web/epsonapi.js \
     -s EXPORTED_FUNCTIONS='["_TTSinit","_TTSstart","_get_total_size"]' \
     -s "EXPORTED_RUNTIME_METHODS=['ccall', 'cwrap', 'addFunction']" \
     -s ALLOW_TABLE_GROWTH=1 \
     -fsanitize=undefined \
     -DSIMULATOR -DSINGLE_THREADED -DHACK_ALERT -DFULL_LANGUAGE_SUPPORT -DFULL_LTS -DNO_INDEXES -DDIRECT_LTS_INPUT -DACNA -DHOMOGRAPHS -DMATH_MODE -DDIVIDED_LTS_RULES -DOUTPUT_FILE -D__linux__ -DSINGLE_LANGUAGE -DMultiple_Sample_Rates \
     -DPOINTER_MATH_REMOVAL_001 \
