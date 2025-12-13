// standard libraries
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

// options
#define USE_I2S // else use PWM
#define UART_ID uart0
#define BAUD_RATE 9600
#define UART_TX_PIN 0
#define UART_RX_PIN 1
#define INPUT_BUFSIZE 8192

// pico & DECtalk includes
#include "epsonapi.h" // include DECtalk
#include "pico/stdlib.h"
#include "hardware/clocks.h" // for clock control
#include "hardware/structs/ioqspi.h"

#ifdef USE_I2S
#include "pico/audio_i2s.h"
#else
#include "pico/audio_pwm.h"
#endif

static const char *daisy_bell = "[:phoneme on] [dey<600,24>ziy<600,21>dey<600,17>ziy<600,12>gih<200,14>vmiy<200,16>yurr<200,17>ah<400,14>nsrr<200,17>duw<1200,12>][ay<600,19>mhxah<600,24>fkrey<600,21>ziy<600,17>ah<400,14>llfow<100,16>rthah<100,17>llah<400,19>vah<200,21>vyu<1200,19>][ih<200,21>twow<200,22>ntbiy<200,21>ah<200,19>stay<400,24>llih<200,21>shmae<200,19>rih<600,17>jh][ay<200,19>keh<400,21>ntah<200,17>fow<400,14>rdah<200,17>keh<200,14>rih<800,12>jh][buh<200,12>tyu<400,17>lluh<200,21>kswiy<400,19>tah<200,12>pah<400,17>nthah<200,21>siy<200,19>t][ah<100,21>vah<100,22>bay<200,24>sih<200,21>kuh<200,17>llbih<400,19>lltfow<200,12>rtuw<1200,17>]";


// globals
char inbuf[INPUT_BUFSIZE];
static int chars_rxed = 0;
struct audio_buffer_pool *audio_pool;

// audio functions
struct audio_buffer_pool *init_audio() {
    static audio_format_t audio_format = {
        .format = AUDIO_BUFFER_FORMAT_PCM_S16,
        .sample_freq = 11025,
        .channel_count = 1
    };

    static struct audio_buffer_format producer_format = {
            .format = &audio_format,
            .sample_stride = 2
    };

    // 71 is the callback buffer size at 11025hz, may need to adjust if you want 8khz
    struct audio_buffer_pool *producer_pool = audio_new_producer_pool(&producer_format, 3, 71);
    bool __unused ok;
    const struct audio_format *output_format;
#ifdef USE_I2S
    struct audio_i2s_config config = {
            .data_pin = PICO_AUDIO_I2S_DATA_PIN,
            .clock_pin_base = PICO_AUDIO_I2S_CLOCK_PIN_BASE,
            .dma_channel = 0,
            .pio_sm = 0,
    };

    output_format = audio_i2s_setup(&audio_format, &config);
#else
    // Use default mono config but override the base pin
    struct audio_pwm_channel_config config = default_mono_channel_config;
    config.core.base_pin = 28;
    output_format = audio_pwm_setup(&audio_format, -1, &config);
#endif
    if (!output_format) {
        panic("PicoAudio: Unable to open audio device.\n");
    }

#ifdef USE_I2S
    ok = audio_i2s_connect(producer_pool);
    assert(ok);
    audio_i2s_set_enabled(true);
#else
    ok = audio_pwm_default_connect(producer_pool, false);
    assert(ok);
    audio_pwm_set_enabled(true);
#endif
    return producer_pool;
}

// buffer callback
short *write_wav(short *iwave, long length) {
    struct audio_buffer *buffer = take_audio_buffer(audio_pool, true);
    memcpy(buffer->buffer->bytes, iwave, length*2);
    buffer->sample_count = length; //buffer->max_sample_count;
    give_audio_buffer(audio_pool, buffer) ;
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
    TextToSpeechStart((char *)input, NULL, WAVE_FORMAT_1M16);
    TextToSpeechSync();
}

int main() {
    // init user input
    stdio_init_all();

    // set frequency for PWM timing
    set_sys_clock_48mhz();

    printf("\n\n");
    printf("System clock: %lu Hz\n", clock_get_hz(clk_sys));

    // Set up our UART
    uart_init(UART_ID, BAUD_RATE);

    audio_pool = init_audio();
    printf("Audio initialized\n");

    // init tts
    TextToSpeechInit(write_wav, NULL);
    printf("TTS initialized\n");
    
    TTSstart("DECtalk Mini is Running.");

    print_header();
    printf("> ");
    fflush(stdout);
    
    while (1) {
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
                // Daisy Bell debug
                TTSstart(daisy_bell);
                chars_rxed = 0;
                memset(inbuf, 0, sizeof(inbuf));
                print_header();
                printf("> ");
                fflush(stdout);
                continue;
            }

            // Speak if not empty
            if (chars_rxed > 0) {
                TTSstart((char *)inbuf);
            }

            chars_rxed = 0;
            memset(inbuf, 0, sizeof(inbuf));
            print_header();  // Refresh header
            printf("> ");
            fflush(stdout);
            continue;
        }

        // Add character to buffer
        if (chars_rxed < INPUT_BUFSIZE-1) {
            inbuf[chars_rxed] = ch;
            chars_rxed++;
            putchar(ch);  // Echo character
            fflush(stdout);
        }

        // Handle buffer full
        if (chars_rxed >= INPUT_BUFSIZE-1) {
            inbuf[INPUT_BUFSIZE-1] = '\0';
            printf("\n");
            TTSstart((char *)inbuf);
            chars_rxed = 0;
            memset(inbuf, 0, sizeof(inbuf));
            print_header();
            printf("> ");
            fflush(stdout);
        }
    }
}
