#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <sys/stat.h>

#include <epsonapi.h>
#include "wav.c"

int main (int argc, char** argv) {
	struct stat s;
	char* str;
	FILE* f = fopen(argv[2], "r");

	stat(argv[2], &s);

	str = malloc(s.st_size + 1);
	memset(str, 0, s.st_size + 1);

	fread(str, 1, s.st_size, f);
	fclose(f);

	init_wav(argv[1]);
	TextToSpeechInit(write_wav, NULL);
	TextToSpeechStart(str, NULL, WAVE_FORMAT_1M16);
	TextToSpeechSync();
	close_wav();

	free(str);
}
