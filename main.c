#include "epsonapi.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define SAMPLE_RATE 11025

// Phoneme names indexed by the phoneme code the epsonapi callback delivers.
static const char *phoneme_names[] = {
    "SIL", "IY", "IH", "EY", "EH", "AE", "AA", "AY", "AW", "AH",
    "AO", "OW", "OY", "UH", "UW", "RR", "YU", "AX", "IX", "IR",
    "ER", "AR", "OR", "UR", "W", "Y", "R", "LL", "HX", "RX",
    "LX", "M", "N", "NX", "EL", "D_DENTALIZED", "EN", "F", "V",
    "TH", "DH", "S", "Z", "SH", "ZH", "P", "B", "T", "D",
    "K", "G", "DX", "TX", "Q", "CH", "JH", "DF"
};

static int  last_phoneme = -1;
static FILE *phoneme_log  = NULL;

#ifdef _WIN32
  #include <io.h>
  #include <fcntl.h>
#ifndef isatty
  #define isatty _isatty
#endif
#ifndef fileno
  #define fileno _fileno
#endif
#ifndef dup
  #define dup    _dup
#endif
#ifndef dup2
  #define dup2   _dup2
#endif
#ifndef STDIN_FILENO
  #define STDIN_FILENO  0
#endif
#ifndef STDOUT_FILENO
  #define STDOUT_FILENO 1
#endif
#ifndef STDERR_FILENO
  #define STDERR_FILENO 2
#endif
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
    if (phoneme_log && phoneme != last_phoneme) {
        int ms = (total_samples * 1000) / SAMPLE_RATE;
        const char *name = (phoneme >= 0 && phoneme < (int)(sizeof(phoneme_names) / sizeof(phoneme_names[0])))
                               ? phoneme_names[phoneme]
                               : "?";
        fprintf(phoneme_log, "%d\t%s\n", ms, name);
        last_phoneme = phoneme;
    }

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
    fprintf(stderr, "Usage: %s [-p] [-o output.wav] [-i input.txt] <text>\n", prog);
    fprintf(stderr, "  -p, --output-pipe       force pipe output to stdout\n");
    fprintf(stderr, "  -o, --output <file>     output WAV file, required unless piping stdout\n");
    fprintf(stderr, "  -i, --input <file>      input text file, required unless piping stdin or text given\n");
    fprintf(stderr, "  text                    required unless piping stdin or -i given\n");
    exit(1);
}

int main(int argc, char *argv[]) {
    const char *output_file = NULL;
    const char *input_file = NULL;
    const char *text = NULL;
    FILE *input_fp = NULL;
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
        } else if (strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--output") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "%s requires an argument\n", argv[i]);
                usage(argv[0]);
            }
            output_file = argv[++i];
            if (strcmp(output_file, "-") == 0) {
                pipeOut = true;
                output_file = NULL;
            }
        } else if (strcmp(argv[i], "-i") == 0 || strcmp(argv[i], "--input") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "%s requires an argument\n", argv[i]);
                usage(argv[0]);
            }
            input_file = argv[++i];
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
    if (!text && !pipeIn && !input_file) {
        usage(argv[0]);
    }

    // if an input file was given, open it now and treat it like piped stdin
    if (input_file) {
        input_fp = fopen(input_file, "r");
        if (!input_fp) {
            fprintf(stderr, "Could not open input file: %s\n", input_file);
            return 1;
        }
        pipeIn = true; // reuse the char-by-char streaming path below
    } else if (pipeIn) {
        input_fp = stdin;
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

    const char *home = getenv("HOME");
    char log_path[512];
    snprintf(log_path, sizeof(log_path), "%s/DECphonetiming.txt", home ? home : ".");
    phoneme_log = fopen(log_path, "w");
    if (!phoneme_log) {
        fprintf(stderr, "warning: could not open %s\n", log_path);
    }

    TextToSpeechInit(write_wav, NULL);

    if (pipeIn) {
        int c;
        while ((c = getc(input_fp)) != EOF) {
            char arr[2];
            arr[0] = c;
            arr[1] = 0;
            TextToSpeechStart((char *)arr, NULL, WAVE_FORMAT_1M16);
        }
        if (input_fp != stdin) {
            fclose(input_fp);
        }
    } else {
	TextToSpeechStart((char*)text, NULL, WAVE_FORMAT_1M16);
    }

    TextToSpeechSync();
    close_wav();
    if (phoneme_log) {
        fclose(phoneme_log);
    }
    return 0;
}
