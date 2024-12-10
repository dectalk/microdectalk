#include <emscripten.h>
#include <emscripten/val.h>
#include <emscripten/bind.h>

#include <stdio.h>

uint32_t header_raw[11] = {
    0x46464952, // RIFF
    0x00000000, // WAV size
    0x45564157, // WAVE
    0x20746d66, // fmt
    0x00000010, // fmt chunk size
    0x00010001, // Audio format 1=PCM & Number of channels 1=Mono
    0x00002B11, // Sampling Frequency in Hz
    0x00003E80, // bytes per second
    0x00100002, // 2=16-bit mono & Number of bits per sample
    0x61746164, // data
    0x00000000  // data size
};

extern "C" {
    void (*user_callback)(long, int); // array pointer, length

    #define MAX_BUFFER (10 * 60) * 11025
    short samples[MAX_BUFFER]; // store 60 seconds of speech
    int total_size = sizeof(header_raw) / 2;

    void write_wav(short *iwave, int length) {
        // user callback not robust enough yet, let old method proceed
        //if (user_callback) {
        //    user_callback((long)iwave, length);
        //    return; // user specified valid callback, override default behavior
        //}

        if (total_size + length > MAX_BUFFER) {
            return;
        }
	for (int i = 0; i < length; i++) {
            samples[total_size + i] = iwave[i];
        }
	total_size += length;
        //printf("Total Size: %i\n", total_size);
    }

    // Existing function declarations (you'll replace these with your actual implementations)
    extern int TextToSpeechStart(char *input);
    extern int TextToSpeechInit();

    // Initialize TTS system and set user callback
    EMSCRIPTEN_KEEPALIVE
    void TTSinit(void (*callback)(long, int) ) {
        // Your TTS initialization code here
        printf("TextToSpeechInit\n");
        TextToSpeechInit();

        user_callback = callback;
    }

    // Start TTS and return buffer
    EMSCRIPTEN_KEEPALIVE
    long TTSstart(std::string input) {
        total_size = 0; // TTSstart implies a new TTS session, resetting audio buffer

        printf("input length: %i\n", (int) strlen(input.c_str()) );
        TextToSpeechStart( (char *) input.c_str() );

        // write wav length to header
        header_raw[10] = (total_size - 22) * sizeof(short);
        header_raw[1] = header_raw[10] + sizeof(header_raw) - 8;

        memcpy(samples, header_raw, sizeof(header_raw) );

        return (long) samples; // Return the global samples buffer
    }

    // Additional function to get buffer size (important for JavaScript)
    EMSCRIPTEN_KEEPALIVE
    int get_total_size() {
        return total_size - (sizeof(header_raw) / 2);
    }
}
