#include <stdint.h>
#include <stdio.h>

uint32_t header_raw[11] = {
    0x46464952, // RIFF
    0x00000000, // WAV size
    0x45564157, // WAVE
    0x20746d66, // fmt
    0x00000010, // fmt chunk size
    0x00010001, // Audio format 1=PCM & Number of channels 1=Mono
    0x00001F40, // Sampling Frequency in Hz
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

void write_wav(short *iwave, int length) {
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
