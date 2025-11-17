#include <tonc.h>
#include "epsonapi.h"

#define TARGET_RATE 11025
#define MAX_SAMPLES (TARGET_RATE * 19)
#define DMA_CHUNK_SIZE 0x4000

EWRAM_DATA s8 audio_buffer[MAX_SAMPLES];
EWRAM_DATA int audio_length = 0;
EWRAM_DATA int play_position = 0;

volatile int audio_playing = 0;
int total_samples_received = 0;
int has_generated = 0;

// Keyboard and input state
#define MAX_INPUT_LEN 64
char input_text[MAX_INPUT_LEN + 1] = "";
int input_len = 0;
int cursor_x = 0;
int cursor_y = 0;

const char keyboard[4][13] = {
    "abcdefghijkl",
    "mnopqrstuvwx",
    "yz 1234567890",
    ".,!?-'\"\x7F"  // \x7F is DEL, used as backspace marker
};

void draw_screen() {
    tte_erase_screen();
    
    // Header with status
    tte_set_pos(8, 8);
    if (audio_playing) {
        tte_write("## DECtalk - Status: PLAYING ##");
    } else if (audio_length > 0) {
        tte_write("## DECtalk - Status: READY   ##");
    } else if (has_generated) {
        tte_write("## DECtalk - Status: BUSY    ##");
    } else {
        tte_write("## DECtalk - Status: IDLE    ##");
    }
    
    // Text input box
    tte_set_pos(8, 20);
    tte_write(">");
    tte_write(input_text);
    
    // Keyboard - build each row with cursor highlight
    char line_buf[64];
    
    for (int y = 0; y < 4; y++) {
        int buf_pos = 0;
        for (int x = 0; x < 12; x++) {
            char c = keyboard[y][x];
            if (c == 0) break;
            
            if (cursor_x == x && cursor_y == y) {
                line_buf[buf_pos++] = '[';
                if (c == '\x7F') {
                    line_buf[buf_pos++] = '<';
                    line_buf[buf_pos++] = '-';
                } else {
                    line_buf[buf_pos++] = c;
                }
                line_buf[buf_pos++] = ']';
            } else {
                line_buf[buf_pos++] = ' ';
                if (c == '\x7F') {
                    line_buf[buf_pos++] = '<';
                    line_buf[buf_pos++] = '-';
                } else {
                    line_buf[buf_pos++] = c;
                }
                line_buf[buf_pos++] = ' ';
            }
        }
        line_buf[buf_pos] = '\0';
        
        tte_set_pos(8, 40 + (y * 12));
        tte_write(line_buf);
    }
    
    // Playback status
    if (audio_playing) {
        int percent = (play_position * 100) / audio_length;
        tte_set_pos(8, 92);
        tte_printf("Playing %d%%", percent);
    } else if (audio_length > 0 && play_position > 0) {
        tte_set_pos(8, 92);
        tte_write("Done");
    }
    
    // Controls
    tte_set_pos(8, 108);
    tte_write("D-Pad:Move A:Type");
    tte_set_pos(8, 120);
    tte_write("START:Speak SELECT:Clear");
}

void setup_next_chunk() {
    int remaining = audio_length - play_position;
    
    if (remaining <= 0) {
        REG_DMA1CNT = 0;
        REG_TM0CNT = 0;
        audio_playing = 0;
        play_position = audio_length;
        draw_screen();
        return;
    }
    
    int chunk_size = (remaining > DMA_CHUNK_SIZE) ? DMA_CHUNK_SIZE : remaining;
    
    REG_DMA1CNT = 0;
    
    REG_DMA1SAD = (u32)(audio_buffer + play_position);
    REG_DMA1DAD = (u32)&REG_FIFO_A;
    REG_DMA1CNT = DMA_DST_FIXED | DMA_REPEAT | DMA_16 | DMA_AT_FIFO | DMA_ENABLE;
    
    play_position += chunk_size;
    
    if ((play_position % (DMA_CHUNK_SIZE * 4)) == 0) {
        draw_screen();
    }
}

void vblank_isr() {
    if (audio_playing) {
        static int vblanks_since_chunk = 0;
        vblanks_since_chunk++;
        
        if (vblanks_since_chunk >= 90) {
            setup_next_chunk();
             vblanks_since_chunk = 0;
        }
    }
}

short *write_wav(short *iwave, long length) {
    for (int i = 0; i < length; i++) {
        if (audio_length >= MAX_SAMPLES - 100) {
            while (audio_length < MAX_SAMPLES && (audio_length % 32) != 0) {
                audio_buffer[audio_length++] = 0;
            }
            return iwave;
        }
        
        s8 sample = (s8)(iwave[i] >> 8);
        audio_buffer[audio_length++] = sample;
        total_samples_received++;
    }
    return iwave;
}
 
void stop_audio() {
    if (!audio_playing) return;
    
    REG_DMA1CNT = 0;
    REG_TM0CNT = 0;
    audio_playing = 0;
    
    draw_screen();
}

void play_audio() {
    if (input_len == 0 || audio_playing) return;
    
    // Reset audio buffer
    audio_length = 0;
    total_samples_received = 0;
    play_position = 0;
    has_generated = 0;
    
    for (int i = 0; i < MAX_SAMPLES; i++) {
        audio_buffer[i] = 0;
    }
    
    has_generated = 1;
    draw_screen();
    
    // Generate speech from input text
    TextToSpeechStart(input_text, NULL, WAVE_FORMAT_1M16);
    TextToSpeechSync();
    has_generated = 0;
    
    if (audio_length == 0) {
        draw_screen();
        return;
    }
    
    audio_playing = 1;
    play_position = 0;
    
    draw_screen();
    
    REG_SNDSTAT = SSTAT_ENABLE;
    REG_SNDDMGCNT = SDMG_LVOL(7) | SDMG_RVOL(7);
    REG_SNDDSCNT = SDS_ATMR0 | SDS_A100 | SDS_AL | SDS_AR;
    
    REG_TM0CNT = 0;
    REG_TM0D = 0x10000 - (16777216 / TARGET_RATE);
    REG_TM0CNT = TM_ENABLE;
    
    setup_next_chunk();
}

void add_char_to_input(char c) {
    if (input_len >= MAX_INPUT_LEN) return;
    input_text[input_len++] = c;
    input_text[input_len] = '\0';
    draw_screen();
}

void backspace_input() {
    if (input_len > 0) {
        input_len--;
        input_text[input_len] = '\0';
        draw_screen();
    }
}

void clear_input() {
    input_len = 0;
    input_text[0] = '\0';
    draw_screen();
}

int main(void) {
    irq_init(NULL);
    irq_add(II_VBLANK, vblank_isr);
    irq_enable(II_VBLANK);
    
    tte_init_chr4c_default(0, BG_CBB(0) | BG_SBB(31));
    
    REG_DISPCNT = DCNT_MODE0 | DCNT_BG0;
    
    TextToSpeechInit(write_wav, NULL);
    
    draw_screen();
    
    while (1) {
        VBlankIntrWait();
        key_poll();

        // Keyboard navigation
        if (key_hit(KEY_UP)) {
            cursor_y = (cursor_y - 1 + 4) % 4;
            // Clamp cursor_x to valid range for new row
            int row_len = 0;
            while (keyboard[cursor_y][row_len] != 0 && row_len < 12) row_len++;
            if (cursor_x >= row_len) cursor_x = row_len - 1;
            draw_screen();
        }
        if (key_hit(KEY_DOWN)) {
            cursor_y = (cursor_y + 1) % 4;
            // Clamp cursor_x to valid range for new row
            int row_len = 0;
            while (keyboard[cursor_y][row_len] != 0 && row_len < 12) row_len++;
            if (cursor_x >= row_len) cursor_x = row_len - 1;
            draw_screen();
        }
        if (key_hit(KEY_LEFT)) {
            cursor_x--;
            if (cursor_x < 0) {
                // Get length of current row
                int row_len = 0;
                while (keyboard[cursor_y][row_len] != 0 && row_len < 12) row_len++;
                cursor_x = row_len - 1;
            }
            draw_screen();
        }
        if (key_hit(KEY_RIGHT)) {
            cursor_x++;
            // Get length of current row
            int row_len = 0;
            while (keyboard[cursor_y][row_len] != 0 && row_len < 12) row_len++;
            if (cursor_x >= row_len) cursor_x = 0;
            draw_screen();
        }
        
        // Type character
        if (key_hit(KEY_A)) {
            char c = keyboard[cursor_y][cursor_x];
            if (c == '\x7F') {
                backspace_input();
            } else if (c != 0) {
                add_char_to_input(c);
            }
        }
        
        // Global controls
        if (key_hit(KEY_SELECT)) {
            clear_input();
        }
        
        if (key_hit(KEY_START)) {
            stop_audio();
            play_audio();
        }
        
        if (key_hit(KEY_B)) {
            stop_audio();
        }
    }
    
    return 0;
}
