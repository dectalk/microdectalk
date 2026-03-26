#include "epsonapi.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef _WIN32
  #include <windows.h>
  #include <io.h>
  #include <fcntl.h>
  #define isatty _isatty
  #define fileno _fileno
  #define dup    _dup
  #define dup2   _dup2
  #define STDIN_FILENO  0
  #define STDOUT_FILENO 1
  #define STDERR_FILENO 2
#else
  #include <unistd.h>
#endif

uint32_t header_raw[11] = {
    0x46464952, // RIFF
    0x00000000, // WAV size
    0x45564157, // WAVE
    0x20746d66, // fmt
    0x00000010, // fmt chunk size
    0x00010001, // Audio format 1=PCM & Number of channels 1=Mono
    0x00002B11, // Sampling Frequency in Hz
    0x00005622, // bytes per second
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
    int i;

#ifdef _WIN32
    // On Windows, stdout is text mode by default which will corrupt binary WAV data.
    // Switch both stdin and stdout to binary mode immediately.
    _setmode(_fileno(stdin),  _O_BINARY);
    _setmode(_fileno(stdout), _O_BINARY);
#endif

    // detect cool unix pipes (works on Windows too via _isatty/_fileno aliases above)
    if (!isatty(fileno(stdin))) {
        pipeIn = true;
    }
    if (!isatty(fileno(stdout))) {
        pipeOut = true;
    }

    // parse args, gross
    for (i = 1; i < argc; i++) {
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

    // redirect stdout to stderr for pipe output so diagnostic prints don't corrupt WAV stream
    if (pipeOut) {
#ifdef _WIN32
        // Windows: reopen stdout as stderr, save original stdout fd for WAV output
        int stdout_fd = dup(STDOUT_FILENO);
        dup2(STDERR_FILENO, STDOUT_FILENO);
        outfile = _fdopen(stdout_fd, "wb");
#else
        int stdout_fd = dup(STDOUT_FILENO);
        dup2(STDERR_FILENO, STDOUT_FILENO);
        outfile = fdopen(stdout_fd, "wb");
#endif
    }

    init_wav(output_file);
    TextToSpeechInit(write_wav, NULL);

    if (pipeIn) {
        int c;
        while ((c = getchar()) != EOF) {
            char arr[2];
	    arr[0] = c;
	    arr[1] = 0;
            TextToSpeechStart((char *)arr, NULL, WAVE_FORMAT_1M16);
        }
    } else {
        TextToSpeechStart((char*)text, NULL, WAVE_FORMAT_1M16);
    }

    TextToSpeechSync();
    close_wav();
    return 0;
}
