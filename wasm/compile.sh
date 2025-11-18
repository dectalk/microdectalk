emcc main.c ../src/*.c -I ../include -o DECtalkMini.js \
  -s WASM=1 \
  -s EXPORTED_FUNCTIONS='["_tts_init","_tts_speak","_tts_get_buffer","_tts_get_buffer_length","_tts_reset","_tts_get_last_phone","_malloc","_free"]' \
  -s EXPORTED_RUNTIME_METHODS='["cwrap","ccall","getValue","setValue","UTF8ToString","stringToUTF8","HEAP16"]' \
  -s ALLOW_MEMORY_GROWTH=1 \
  -s INITIAL_MEMORY=33554432 \
  -s MODULARIZE=1 \
  -s EXPORT_NAME="DECtalkMini" \
  -DNO_FILESYSTEM -D_REENTRANT=1 -DNOMME=1 -DLTSSIM=1 -DTTSSIM=1 -DANSI=1 -DBLD_DECTALK_DLL=1 -DENGLISH=1 -DENGLISH_US=1 -DACCESS32=1 -DTYPING_MODE=1 -DACNA=1 -DDISABLE_AUDIO=1 -DSINGLE_THREADED=1 -D__unix__=1
