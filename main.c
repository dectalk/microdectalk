#include <stdio.h>
#include <stdint.h>
#include "epsonapi.h"

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

// Global variables to track audio data
static int total_samples = 0;

FILE *outfile;

void init_wav(const char *name) {
  outfile = fopen(name, "wb");
  fwrite(header_raw, sizeof(header_raw), 1, outfile);
  total_samples = 0;
}

short *write_wav(short *iwave, long length, int phoneme) {
  fwrite(iwave, sizeof(short), length, outfile);
  total_samples += length;
}
void close_wav() {
  // write wav length
  *((uint32_t *)&header_raw[10]) = total_samples * sizeof(short);
  *((uint32_t *)&header_raw[1]) = header_raw[10] + sizeof(header_raw) - 8;

  // Seek back to beginning and write the final header
  fseek(outfile, 0, SEEK_SET);
  fwrite(header_raw, sizeof(header_raw), 1, outfile);
  fclose(outfile);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <output.wav> <text>\n", argv[0]);
        fprintf(stderr, "Example: %s output.wav \"Hello world\"\n", argv[0]);
        return 1;
    }

    const char *output_file = argv[1];
    const char *text = argv[2];

    init_wav(output_file);

    TextToSpeechInit(write_wav, NULL);
    TextToSpeechStart(text, NULL, WAVE_FORMAT_1M16);
    TextToSpeechSync();

    close_wav();

    return 0;
}
