#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "pico/audio_pwm.h"
#include "pico/stdio_usb.h"
#include "hardware/clocks.h"

#include "epsonapi.h" // import DECtalk 4.99

// Audio configuration
#define SAMPLE_RATE 22050
#define SAMPLES_PER_BUFFER 1024

// Global audio pool
struct audio_buffer_pool *ap;

struct audio_buffer_pool *init_audio() {
    static audio_format_t audio_format = {
        .format = AUDIO_BUFFER_FORMAT_PCM_S16,
        .sample_freq = SAMPLE_RATE,
        .channel_count = 1,
    };

    static struct audio_buffer_format producer_format = {
        .format = &audio_format,
        .sample_stride = 2
    };

    struct audio_buffer_pool *producer_pool = audio_new_producer_pool(&producer_format, 3,
                                                                      SAMPLES_PER_BUFFER);
    
    // Set up PWM audio on GPIO 28
    const struct audio_format *output_format;
    
    // Use default mono config but override the base pin
    struct audio_pwm_channel_config config = default_mono_channel_config;
    config.core.base_pin = 28;
    
    output_format = audio_pwm_setup(&audio_format, -1, &config);
    if (!output_format) {
        panic("PicoAudio: Unable to open audio device.\n");
    }
    
    bool ok = audio_pwm_default_connect(producer_pool, false);
    assert(ok);
    audio_pwm_set_enabled(true);
    
    printf("Connecting PIO PWM audio via 'blocking give'\n");
    
    return producer_pool;
}

short *write_wav(short *iwave, long length) {
    // Process audio in chunks that fit in the buffer
    int offset = 0;
    while (offset < length) {
        struct audio_buffer *buffer = take_audio_buffer(ap, true);
        int16_t *samples = (int16_t *)buffer->buffer->bytes;
        
        int samples_to_copy = length - offset;
        // Each input sample becomes 2 output samples (original + zero)
        if (samples_to_copy > buffer->max_sample_count / 2) {
            samples_to_copy = buffer->max_sample_count / 2;
        }
        
        // upsample audio because samplerate is borked
        for (int i = 0; i < samples_to_copy; i++) {
            samples[i*2] = iwave[offset + i];
            if (i < samples_to_copy - 1) {
                samples[i*2+1] = (iwave[offset+i] + iwave[offset+i+1]) / 2;
            } else {
                samples[i*2+1] = iwave[offset+i]; // repeat last sample
            }
        }
        
        buffer->sample_count = samples_to_copy * 2;
        give_audio_buffer(ap, buffer);
        
        offset += samples_to_copy;
    }
    
    return iwave;
}

void print_header() {
    printf("\033[s");  // Save cursor position
    printf("\033[0;0H");  // Move to top-left
    printf("\033[2K");  // Clear line
    printf("### DECtalk Mini - Type text and press Enter to speak ###\n");
    printf("\033[u");  // Restore cursor position
    fflush(stdout);
}

void TTSstart(const char *input) {
    printf("Speaking...\n");

    // Start up DECtalk text input
    TextToSpeechStart((char *)input, NULL, WAVE_FORMAT_1M16);
    TextToSpeechSync();
    
    // Small delay to ensure clean finish
    sleep_ms(100);
}

char inbuf[2048];
static int chars_rxed = 0;

int main() {
    // Set system clock to 48MHz for audio clock timing
    set_sys_clock_48mhz();
    
    // Now init USB serial at the correct clock speed
    stdio_init_all();

    printf("Initializing audio...\n");
    
    // Initialize audio using pico-extras
    ap = init_audio();
    
    printf("Initializing TTS...\n");
    
    // Initialize DECtalk
    TextToSpeechInit(write_wav, NULL);
    printf("TTS initialized\n");
    
    // Say hello on startup
    TTSstart("DECtalk Mini is Running.");
    
    printf("Ready for input\n");
    
    // Main loop - read text and speak it
    printf("> ");
    fflush(stdout);
    
    while (true) {
        // Read line from USB serial
        char ch = getchar();

        // Handle backspace (0x7F is DEL, 0x08 is backspace)
        if (ch == 0x7F || ch == 0x08) {
            if (chars_rxed > 0) {
                chars_rxed--;
                inbuf[chars_rxed] = '\0';
                printf("\b \b");  // Move back, print space, move back again
                fflush(stdout);
            }
            continue;
        }
        
        // Handle newline/carriage return
        if (ch == '\n' || ch == '\r') {
            inbuf[chars_rxed] = '\0';
            printf("\n");

            if (inbuf[0] == ':' && inbuf[1] == 'd') {
                // daisy bell
                const char* text = "[:phoneme on] [dey<600,24>ziy<600,21>dey<600,17>ziy<600,12>gih<200,14>vmiy<200,16>yurr<200,17>ah<400,14>nsrr<200,17>duw<1200,12>][ay<600,19>mhxah<600,24>fkrey<600,21>ziy<600,17>ah<400,14>llfow<100,16>rthah<100,17>llah<400,19>vah<200,21>vyu<1200,19>][ih<200,21>twow<200,22>ntbiy<200,21>ah<200,19>stay<400,24>llih<200,21>shmae<200,19>rih<600,17>jh][ay<200,19>keh<400,21>ntah<200,17>fow<400,14>rdah<200,17>keh<200,14>rih<800,12>jh][buh<200,12>tyu<400,17>lluh<200,21>kswiy<400,19>tah<200,12>pah<400,17>nthah<200,21>siy<200,19>t][ah<100,21>vah<100,22>bay<200,24>sih<200,21>kuh<200,17>llbih<400,19>lltfow<200,12>rtuw<1200,17>]";
                TTSstart(text);
                chars_rxed = 0;
                memset(inbuf, 0, sizeof(inbuf));
                printf("> ");
                fflush(stdout);
                continue;
            }

            // Speak if not empty
            if (chars_rxed > 0) {
                TTSstart(inbuf);
            }
            
            chars_rxed = 0;
            memset(inbuf, 0, sizeof(inbuf));
            print_header();  // Refresh header
            printf("> ");
            fflush(stdout);
            continue;
        }
        
        // Add character to buffer
        if (chars_rxed < 2047) {
            inbuf[chars_rxed] = ch;
            chars_rxed++;
            putchar(ch);  // Echo character
            fflush(stdout);
        }
        
        // Handle buffer full
        if (chars_rxed >= 2047) {
            inbuf[2047] = '\0';
            printf("\n");
            TTSstart(inbuf);
            chars_rxed = 0;
            memset(inbuf, 0, sizeof(inbuf));
            printf("> ");
            fflush(stdout);
        }
    }
    
    return 0;
}
