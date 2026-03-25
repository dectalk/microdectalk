#include "epsonapi.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

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

static bool pipeIn = false;
static bool pipeOut = false;

FILE *outfile;

void init_wav(const char *name) {
  if (!pipeOut) {
    outfile = fopen(name, "wb");
    fwrite(header_raw, sizeof(header_raw), 1, outfile);
    total_samples = 0;
  } else {
    *((uint32_t *)&header_raw[10]) = 0xFFFFFFFF;
    *((uint32_t *)&header_raw[1]) = header_raw[10] + sizeof(header_raw) - 8;
    fwrite(header_raw, sizeof(header_raw), 1, outfile);
    total_samples = 0;
  }
}

short *write_wav(short *iwave, long length, int phoneme) {
    fwrite(iwave, sizeof(short), length, outfile);
    total_samples += length;

    return iwave;
}

void close_wav() {
  if (pipeOut) {return;}
  // write wav length
  *((uint32_t *)&header_raw[10]) = total_samples * sizeof(short);
  *((uint32_t *)&header_raw[1]) = header_raw[10] + sizeof(header_raw) - 8;

  // Seek back to beginning and write the final header
  fseek(outfile, 0, SEEK_SET);
  fwrite(header_raw, sizeof(header_raw), 1, outfile);
  fclose(outfile);
}

void usage(const char *prog) {
    fprintf(stderr, "Usage: %s [-p] [output.wav] <text>\n", prog);
    fprintf(stderr, "  -p          force pipe output to stdout\n");
    fprintf(stderr, "  output.wav  required unless piping stdout\n");
    fprintf(stderr, "  text        required unless piping stdin\n");
    exit(1);
}

int main(int argc, char *argv[]) {
    const char *output_file = NULL;
    const char *text = NULL;

    // detect cool unix pipes
    if (!isatty(STDIN_FILENO)) {
        pipeIn = true;
    }
    if (!isatty(STDOUT_FILENO)) {
        pipeOut = true;
    }

    // parse args, gross
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-p") == 0) {
            pipeOut = true;
        } else if (!output_file && !pipeOut) {
            output_file = argv[i];
        } else if (!text) {
            text = argv[i];
        } else {
            fprintf(stderr, "Unexpected argument: %s\n", argv[i]);
            usage(argv[0]);
        }
    }

    // tell the user if they doin' it wrong
    if (!pipeOut && !output_file) {
        usage(argv[0]);
    }
    if (!text && !pipeIn) {
        usage(argv[0]);
    }

    // magic unix stderr redirect
    if (pipeOut) {
        int stdout_fd = dup(STDOUT_FILENO);
        dup2(STDERR_FILENO, STDOUT_FILENO);
        outfile = fdopen(stdout_fd, "wb");
    }

    init_wav(output_file);
    TextToSpeechInit(write_wav, NULL);

    if (pipeIn) {
        int c;
        while ((c = getchar()) != EOF) {
            char arr[] = {c, 0};
            TextToSpeechStart((char *)arr, NULL, WAVE_FORMAT_1M16);
        }
    } else {
        TextToSpeechStart((char*)text, NULL, WAVE_FORMAT_1M16);
    }

    TextToSpeechSync();
    close_wav();
    return 0;
}
