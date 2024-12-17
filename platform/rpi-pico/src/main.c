#include <stdio.h>
#define MAX_MESSAGE_LENGTH 12
#include "epsonapi.h"

#include <math.h>

#include <stdlib.h>
#include "pico/stdlib.h"

#define I2S
#define USB_SERIAL

#ifdef I2S

#include "pico/audio_i2s.h"
#include "hardware/structs/ioqspi.h"
#else
#define AUDIO_PIN 28
#define WAV_DATA_LENGTH 71
#include "hardware/pwm.h"  // pwm
#include "hardware/irq.h"  // interrupts
#include "hardware/sync.h" // wait for interrupt
#include "hardware/clocks.h"
int wav_position = 0;

#endif

#ifndef USB_SERIAL
// uart
#include "hardware/uart.h"
#include "hardware/irq.h"

#define UART_ID uart0
#define BAUD_RATE 115200
#define DATA_BITS 8
#define STOP_BITS 1
#define PARITY    UART_PARITY_NONE

// We are using pins 0 and 1, but see the GPIO function select table in the
// datasheet for information on which other pins can be used.
#define UART_TX_PIN 16
#define UART_RX_PIN 17

#endif

char inbuf[128];
static int chars_rxed = 0;

#ifndef USB_SERIAL
void init_uart() {
    uart_init(UART_ID, 2400);
    gpio_set_function(UART_TX_PIN, UART_FUNCSEL_NUM(UART_ID, UART_TX_PIN));
    gpio_set_function(UART_RX_PIN, UART_FUNCSEL_NUM(UART_ID, UART_RX_PIN));
    int __unused actual = uart_set_baudrate(UART_ID, BAUD_RATE);
    uart_set_hw_flow(UART_ID, false, false);
    uart_set_format(UART_ID, DATA_BITS, STOP_BITS, PARITY);
    uart_set_fifo_enabled(UART_ID, false);
    uart_puts(UART_ID, "\nHello, uart interrupts\n");
}
#endif

#ifdef I2S
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

    struct audio_buffer_pool *producer_pool = audio_new_producer_pool(&producer_format, 3, 71); // todo correct size
    bool __unused ok;
    const struct audio_format *output_format;
    struct audio_i2s_config config = {
            .data_pin = PICO_AUDIO_I2S_DATA_PIN,
            .clock_pin_base = PICO_AUDIO_I2S_CLOCK_PIN_BASE,
            .dma_channel = 0,
            .pio_sm = 0,
    };

    output_format = audio_i2s_setup(&audio_format, &config);
    if (!output_format) {
        panic("PicoAudio: Unable to open audio device.\n");
    }

    ok = audio_i2s_connect(producer_pool);
    assert(ok);
    audio_i2s_set_enabled(true);
    return producer_pool;
}
struct audio_buffer_pool *ap;
struct audio_buffer *buffer;
#else

short samples[11025*10];
int total_size = 0;
void pwm_interrupt_handler() {
    pwm_clear_irq(pwm_gpio_to_slice_num(AUDIO_PIN));
    if (wav_position < (total_size<<3) - 1) {
        // set pwm level
        // allow the pwm value to repeat for 8 cycles this is >>3
        pwm_set_gpio_level(AUDIO_PIN, samples[wav_position>>3] >> 8);
        wav_position++;
    } else {
        // do nothing
    }
}

void init_pwm() {
    set_sys_clock_khz(176000, true);
    gpio_set_function(AUDIO_PIN, GPIO_FUNC_PWM);
    int audio_pin_slice = pwm_gpio_to_slice_num(AUDIO_PIN);
    // Setup PWM interrupt to fire when PWM cycle is complete
    pwm_clear_irq(audio_pin_slice);
    pwm_set_irq_enabled(audio_pin_slice, true);
    // set the handle function above
    irq_set_exclusive_handler(PWM_IRQ_WRAP, pwm_interrupt_handler);
    irq_set_enabled(PWM_IRQ_WRAP, true);
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, 8.0f);
    pwm_config_set_wrap(&config, 250);
    pwm_init(audio_pin_slice, &config, true);
    pwm_set_gpio_level(AUDIO_PIN, 0);
}
#endif

// buffer callback
void write_wav(short *iwave, int length) {
#ifdef I2S
    buffer = buffer = take_audio_buffer(ap, true);
    memcpy(buffer->buffer->bytes, iwave, length*2);
    buffer->sample_count = buffer->max_sample_count;
    give_audio_buffer(ap, buffer);
#else
    for (int i = 0; i < length; i++) {
        samples[total_size + i] = iwave[i];
    }
    total_size += length;
#endif
}

void TTSstart(const char *input) {
    //total_length = 0;
    TextToSpeechStart((char *)input);
#ifndef I2S
    wav_position = 0;
    __wfi();
#endif
}

// serial read functions
#define MAX_BUFFER_SIZE 64  // Maximum size of the buffer for each line

int main() {
#ifdef I2S
    ap = init_audio();
#else
    init_pwm();
#endif

    // init user input
    stdio_init_all();
#ifndef USB_SERIAL
    init_uart();
#endif
    sleep_ms(100);

    // init tts
    TextToSpeechInit();
    TTSstart("Microdectalk is Running.");

    char buf[128];
    int len = 0;
    while (1) {
#ifndef I2S
        __wfi();
#endif

#ifndef USB_SERIAL
        if (!uart_is_readable(UART_ID)) {
            continue;
        }
#endif

#ifndef USB_SERIAL
        // append new input to buffer
        uint8_t ch = uart_getc(UART_ID);
#else
        char ch = getchar(); //
#endif

        inbuf[chars_rxed] = ch;
        chars_rxed++;

        // send if endline or more characters than buffer
       	if (ch == '\n' || ch == '\r' || chars_rxed >= 128) {
            TextToSpeechStart((char *)inbuf);
            chars_rxed = 0;
#ifndef USB_SERIAL
            if (uart_is_writable(UART_ID)) {
                uart_puts(UART_ID, inbuf);
                uart_puts(UART_ID, "\n\r");
            }
#else
            printf("%s\n\r", inbuf);
#endif
            memset(inbuf, 0, sizeof(inbuf));
        }

    }
}

