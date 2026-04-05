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
#define MAX_LINE_LEN 256
#define LINE_QUEUE_DEPTH 8

// pico & DECtalk includes
#include "epsonapi.h" // include DECtalk
#include "pico/stdlib.h"
#include "pico/stdio_usb.h"
#include "tusb.h"
#include "pico/multicore.h"
#include "pico/util/queue.h"
#include "hardware/clocks.h" // for clock control
#include "hardware/structs/ioqspi.h"

#ifdef USE_I2S
#include "pico/audio_i2s.h"
#else
#include "pico/audio_pwm.h"
#endif

// Queue of text chunks from core 0 → core 1 (TTS).
// sync=true: call TextToSpeechSync after Start (end of logical line).
// sync=false: overflow chunk — Start only, let engine accumulate continuously.
typedef struct { char text[MAX_LINE_LEN]; bool sync; } Line;
static queue_t line_queue;

// Set by core 0 on 0x90. write_wav discards audio while set, causing
// cmd_loop(0x0B) in TextToSpeechSync to drain instantly without touching
// engine state. Safe to spam — no TextToSpeechInit is ever triggered.
static volatile bool stop_requested = false;

// globals
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
short *write_wav(short *iwave, long length, int phoneme) {
    struct audio_buffer *buffer;
    while (!(buffer = take_audio_buffer(audio_pool, false))) {
        if (stop_requested) {
            TextToSpeechReset();
            return iwave;
        }
    }
    if (stop_requested) {
        give_audio_buffer(audio_pool, buffer);
        TextToSpeechReset();
        return iwave;
    }
    memcpy(buffer->buffer->bytes, iwave, length*2);
    buffer->sample_count = length;
    give_audio_buffer(audio_pool, buffer);
    return iwave;
}

// Core 1: owns all TTS operations exclusively (TTS is not cross-core safe).
// Dequeues complete lines from core 0 and synthesizes them sequentially.
static void core1_tts_task() {
    audio_pool = init_audio();
    TextToSpeechInit(write_wav, NULL);

    // Signal core 0 that init is done
    multicore_fifo_push_blocking(1);

    // Speak startup message
    TextToSpeechStart((char *)("DECtalk Mini is Running."), NULL, WAVE_FORMAT_1M16);
    TextToSpeechSync();

    Line line;
    while (1) {
        // While idle, immediately acknowledge stop requests — otherwise core 0
        // would spin-wait forever on a stop with nothing being synthesized.
        while (!queue_try_remove(&line_queue, &line)) {
            if (stop_requested) stop_requested = false;
            tight_loop_contents();
        }
        stop_requested = false;
        TextToSpeechStart(line.text, NULL, WAVE_FORMAT_1M16);

        // Only sync on the final chunk of a logical line. Overflow chunks have
        // sync=false so the engine accumulates them as continuous text without
        // resetting its parse state between chunks.
        if (line.sync) {
            TextToSpeechSync();
            if (stop_requested) {
                Line discard;
                while (queue_try_remove(&line_queue, &discard)) {}
            }
        }
    }
}

// USB CDC: interactive/cooked mode — ANSI header, local echo, backspace, prompt.
// UART: raw mode — no echo, no backspace, plain line accumulation.
// Both sources feed the same TTS queue with independent line buffers.
#define TERM_ROWS    24
#define HISTORY_SIZE 16

static void cooked_init(void) {
    printf("\033[2J\033[H");          // clear screen, cursor home
    printf("=== DECtalkMini === /exit to exit, CTRL-C to stop\r\n");
    printf("-------------------\r\n");
    printf("\r\n");
    printf("\033[4;%dr", TERM_ROWS);  // scroll region: rows 4-TERM_ROWS
    printf("\033[4;1H");              // cursor to first scrolling row
    printf("> ");
}

// flush=true appends \x0b so DECtalk synthesises without an utterance break.
// flush=false enqueues the chunk bare — used on overflow so DECtalk keeps its
// internal buffer flowing continuously across chunk boundaries.
static void enqueue_line(char *buf, int len, bool flush) {
    if (flush) buf[len++] = '\x0b';
    buf[len] = '\0';
    Line line;
    strncpy(line.text, buf, MAX_LINE_LEN - 1);
    line.text[MAX_LINE_LEN - 1] = '\0';
    line.sync = flush;  // only sync after the final chunk of a logical line
    queue_add_blocking(&line_queue, &line);
}

int main() {
    stdio_init_all();

    // set frequency for PWM timing
    //set_sys_clock_48mhz();

    printf("\n\n");
    printf("System clock: %lu Hz\n", clock_get_hz(clk_sys));

    // Configure UART pins; stdio_uart is disabled so UART is ours directly
    uart_init(UART_ID, BAUD_RATE);
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

    queue_init(&line_queue, sizeof(Line), LINE_QUEUE_DEPTH);
    multicore_launch_core1(core1_tts_task);

    // Wait for core 1 to finish audio + TTS init before printing
    multicore_fifo_pop_blocking();
    printf("Audio and TTS initialized\n");

    // Core 0 input loop: collect chars from USB serial (cooked) and hardware
    // UART (raw), accumulate into separate line buffers, enqueue for core 1.
    printf("READY\n");  // signals test scripts that the device is up and listening

    char usb_buf[MAX_LINE_LEN];
    int  usb_len = 0;
    char uart_buf[MAX_LINE_LEN];
    int  uart_len = 0;
    bool usb_was_connected = false;

    // Input history for USB CDC (static to avoid blowing the 2KB stack)
    static char history[HISTORY_SIZE][MAX_LINE_LEN];
    static char history_saved[MAX_LINE_LEN];
    int  history_count     = 0;
    int  history_pos       = -1;   // -1 = live input
    int  history_saved_len = 0;

    // Escape sequence state for USB CDC
    typedef enum { ESC_NONE, ESC_ESC, ESC_CSI } EscState;
    EscState esc_state = ESC_NONE;

    while (1) {
        // Re-run cooked_init whenever a new USB CDC connection is detected
        bool usb_connected = stdio_usb_connected();
        if (usb_connected && !usb_was_connected) {
            cooked_init();
            usb_len = 0;
        }
        usb_was_connected = usb_connected;

        // --- UART (raw mode) ---
        if (uart_is_readable(UART_ID)) {
            int ch = uart_getc(UART_ID);
            if ((uint8_t)ch == 0x90) {
                stop_requested = true;
                uart_len = 0;
                while (stop_requested) tight_loop_contents();
            } else if (ch == '\n' || ch == '\r') {
                if (uart_len > 0) {
                    enqueue_line(uart_buf, uart_len, true);
                    uart_len = 0;
                }
            } else {
                // Overflow: enqueue chunk bare and keep going — never drop input
                if (uart_len >= MAX_LINE_LEN - 2) {
                    enqueue_line(uart_buf, uart_len, false);
                    uart_len = 0;
                }
                uart_buf[uart_len++] = (char)ch;
            }
        }

        // --- USB CDC (cooked/interactive mode) ---
        int ch = getchar_timeout_us(0);
        if (ch == PICO_ERROR_TIMEOUT) continue;

        // Escape sequence state machine — absorbs arrow keys and scroll sequences
        if (esc_state == ESC_ESC) {
            esc_state = (ch == '[') ? ESC_CSI : ESC_NONE;
            continue;
        }
        if (esc_state == ESC_CSI) {
            if ((ch >= '0' && ch <= '9') || ch == ';') continue; // param bytes
            esc_state = ESC_NONE;
            if (ch == 'A') { // up arrow — older history
                if (history_count == 0) continue;
                if (history_pos == -1) {
                    memcpy(history_saved, usb_buf, usb_len);
                    history_saved_len = usb_len;
                    history_pos = 0;
                } else if (history_pos < history_count - 1) {
                    history_pos++;
                }
                usb_len = strlen(history[history_pos]);
                memcpy(usb_buf, history[history_pos], usb_len);
                printf("\r\033[K> %.*s", usb_len, usb_buf);
            } else if (ch == 'B') { // down arrow — newer history / live input
                if (history_pos == -1) continue;
                if (history_pos > 0) {
                    history_pos--;
                    usb_len = strlen(history[history_pos]);
                    memcpy(usb_buf, history[history_pos], usb_len);
                } else {
                    history_pos = -1;
                    usb_len = history_saved_len;
                    memcpy(usb_buf, history_saved, usb_len);
                }
                printf("\r\033[K> %.*s", usb_len, usb_buf);
            }
            // all other CSI sequences (scroll, page, right/left, etc.) are ignored
            continue;
        }
        if (ch == '\033') { esc_state = ESC_ESC; continue; }

        // Normal input
        if ((uint8_t)ch == 0x90 || ch == 0x03) {  // 0x90 or Ctrl-C
            stop_requested = true;
            usb_len = 0;
            history_pos = -1;
            while (stop_requested) tight_loop_contents();
            printf("\r\n> ");
        } else if (ch == '\n' || ch == '\r') {
            if (usb_len > 0) {
                usb_buf[usb_len] = '\0';
                if (strcmp(usb_buf, "/exit") == 0) {
                    printf("\r\nBye.\r\n\r\n\r\n");
                    usb_len = 0;
                    history_pos = -1;
                    sleep_ms(100);
                    tud_disconnect();
                    sleep_ms(500);
                    tud_connect();
                } else {
                    // Push to history
                    if (history_count < HISTORY_SIZE) history_count++;
                    memmove(&history[1], &history[0], (history_count - 1) * sizeof(history[0]));
                    strncpy(history[0], usb_buf, MAX_LINE_LEN - 1);
                    history[0][MAX_LINE_LEN - 1] = '\0';
                    history_pos = -1;
                    enqueue_line(usb_buf, usb_len, true);
                    usb_len = 0;
                    printf("\r\n> ");
                }
            } else {
                printf("\r\n> ");
            }
        } else if (ch == '\b' || (uint8_t)ch == 0x7F) {
            if (usb_len > 0) {
                usb_len--;
                printf("\b \b");
            }
        } else {
            // Overflow: enqueue chunk bare and keep going — never drop input
            if (usb_len >= MAX_LINE_LEN - 2) {
                enqueue_line(usb_buf, usb_len, false);
                usb_len = 0;
            }
            usb_buf[usb_len++] = (char)ch;
            printf("%c", (char)ch);  // local echo
        }
    }
}
