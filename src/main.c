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

void write_wav(short *iwave, unsigned int length) {
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


int main() {
    init_wav("output.wav");
    TextToSpeechInit(NULL, NULL);
    TextToSpeechStart("[:phoneme on] [d<40,27> ey<860> z<40,24> iy<860> d<40,20> ey<860> z<40,15> iy<860>] [g<40,17> ih<220> v<40> m<40,19> iy<260> yx<40,20> or<260> ae<300,17> en<300> s<40,20> rr<260> d<40,15> uw<860> _<900>] [ay<860,22> m<40> hx<40,27> ae<810,27> f<30> k<30,24> r<30> ey<860> z<40,20> iy<860>] [ao<200,17> el<100> f<40,19> or<260> dh<40,20> ax<260> l<40,22> uh<560> v<40,24> ax<260> v<40,22> yu<860> _<600,24>] [ih<260,24> tx<40> w<40,25> ow<180> n<40> t<40> b<40,24> iy<260> ax<260,22> s<40> t<40,27> ay<560> l<40,24> ih<220> sh<40> m<40,22> ae<60> ae<100,24> ae<100,22> r<40,20> ih<560> jh<40> _<560,22>] [ay<300,22> k<40,24> ae<520> en<40> t<40,20> ax<260> f<40,17> or<560> d<40,20> ax<260> k<40,17> ae<260> r<40,15> ih<560> jh<40> _<560>] [b<40,15> uh<220> tx<40> yu<560,20> d<40> l<40,24> uh<210> k<30> s<30,22> w<30> iy<260> t<40> _<560,20>] [aa<560,20> en<40> dh<40,24> ax<260> s<40,22> iy<260> t<40> _<260,22>] [ah<150,24> v<40,25> ax<110> b<40,27> ay<260> s<40,24> ih<260> k<40,20> el<260> b<40,22> ih<260> el<260> tx<40> f<40,15> or<250> t<10> t<40> uw<860,20> _<900>]", NULL, WAVE_FORMAT_1M16);

    close_wav();
    //TextToSpeechSync
}
