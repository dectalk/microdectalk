#define NO_FILESYSTEM
#include "epsonapi.h"
#include <emscripten.h>
#include <string.h>
#include <stdio.h>

#define MAX_BUFFER_SIZE 65536
static short audio_buffer[MAX_BUFFER_SIZE];
static int buffer_index = 0;
static int sample_rate = 11025;

extern int last_phoneme;

// Declare JS function that will be called from C
EM_JS(void, js_phone_callback, (int phone), {
    if (window.onPhoneCallback) {
        window.onPhoneCallback(phone);
    }
});

EMSCRIPTEN_KEEPALIVE
short* audio_callback(short *data, long length) {
    //printf("last_phone value: %d\n", last_phoneme);  // Should be 0-48
    // Call JavaScript callback with current phone value
    js_phone_callback(last_phoneme & 0x00FF);
    
    // Copy samples from callback into our buffer
    for (int i = 0; i < length && buffer_index < MAX_BUFFER_SIZE; i++) {
        audio_buffer[buffer_index++] = data[i];
    }
    return data;
}

EMSCRIPTEN_KEEPALIVE
int tts_init() {
    buffer_index = 0;
    return TextToSpeechInit(audio_callback, NULL);
}

EMSCRIPTEN_KEEPALIVE
int tts_speak(const char *text, int format) {
    buffer_index = 0;
    sample_rate = (format == 0x0001) ? 11025 : 8000;
    int result = TextToSpeechStart((char*)text, NULL, format);
    if (result == 0) {
        TextToSpeechSync();
    }
    return result;
}

EMSCRIPTEN_KEEPALIVE
short* tts_get_buffer() {
    return audio_buffer;
}

EMSCRIPTEN_KEEPALIVE
int tts_get_buffer_length() {
    return buffer_index;
}

EMSCRIPTEN_KEEPALIVE
int tts_get_last_phone() {
    return last_phoneme;
}

EMSCRIPTEN_KEEPALIVE
int tts_reset() {
    buffer_index = 0;
    return TextToSpeechReset();
}
