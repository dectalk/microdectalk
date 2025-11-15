#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"
#include "pico/stdio_usb.h"

#include "epsonapi.h" // import DECtalk 4.99

// PWM configuration
#define AUDIO_PIN 28
#define SAMPLE_RATE 11025
#define PWM_WRAP 4095  // 12-bit resolution

// Global variables for PWM
uint slice_num;

void init_audio_pwm() {
    gpio_set_function(AUDIO_PIN, GPIO_FUNC_PWM);
    slice_num = pwm_gpio_to_slice_num(AUDIO_PIN);
    
    // Calculate PWM clock divider
    float clock_div = (float)clock_get_hz(clk_sys) / (PWM_WRAP * SAMPLE_RATE);
    
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, clock_div);
    pwm_config_set_wrap(&config, PWM_WRAP);
    
    pwm_init(slice_num, &config, true);

    // silence
    pwm_set_gpio_level(AUDIO_PIN, 0);
}

#define BUFFER_SIZE 512
short audio_buffer[BUFFER_SIZE];
volatile size_t buffer_read_pos = 0;
volatile size_t buffer_write_pos = 0;
volatile size_t buffer_count = 0;
volatile bool tts_active = false;
volatile bool audio_playing = false;

bool repeating_timer_callback(struct repeating_timer *t) {
    if (buffer_count > 0) {
        int16_t sample = audio_buffer[buffer_read_pos];
        uint16_t pwm_value = (sample + 32768) >> 4;  // Convert to 0-4095 range

        pwm_set_gpio_level(AUDIO_PIN, pwm_value);
        audio_playing = true;

        buffer_read_pos = (buffer_read_pos + 1) % BUFFER_SIZE;
        buffer_count--;
    } else {
        // Output silence when no data
        pwm_set_gpio_level(AUDIO_PIN, 0);
        audio_playing = false;
    }
    
    return true;
}

short *write_wav(short *iwave, long length) {
    for (int i = 0; i < length; i++) {
        while (buffer_count >= BUFFER_SIZE) {
            tight_loop_contents();
        }

        audio_buffer[buffer_write_pos] = iwave[i];
        buffer_write_pos = (buffer_write_pos + 1) % BUFFER_SIZE;
        buffer_count++;
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
    tts_active = true;
    printf("Speaking...\n");

    // Start up DECtalk text input
    TextToSpeechStart((char *)input, NULL, WAVE_FORMAT_1M16);
    TextToSpeechSync();
    
    tts_active = false;
    
    // Keep playing until all audio is output
    while (buffer_count > 0 || audio_playing) {
        sleep_ms(10);
    }
    
    // Small delay to ensure clean finish
    sleep_ms(100);
}

char inbuf[128];
static int chars_rxed = 0;

int main() {
    // Initialize PWM with silence first
    init_audio_pwm();
    
    // Start timer for sample playback at 11.025kHz
    struct repeating_timer timer;
    add_repeating_timer_us(1000000 / SAMPLE_RATE, repeating_timer_callback, NULL, &timer);
    
    // Init USB serial
    stdio_init_all();
    
    // Small delay to ensure PWM is stable before starting TTS
    sleep_ms(100);
    
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
        if (chars_rxed < 127) {
            inbuf[chars_rxed] = ch;
            chars_rxed++;
            putchar(ch);  // Echo character
            fflush(stdout);
        }
        
        // Handle buffer full
        if (chars_rxed >= 127) {
            inbuf[127] = '\0';
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
