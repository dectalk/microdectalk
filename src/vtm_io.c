#include "vtm_ip.h"
#include "vtm_idef.h"
#include "viport.h"
#include "phdefs.h"
#include "vtm_idef.h"
#include <stdio.h>

extern short *global_spc_buf;
short noiseb;

#define SPC_TYPE_MASK			(0x00ff)
#define TONE_PARS        5

extern int16_t ldspdef;

extern int16_t rampdown;
extern int16_t decay;     /*  decay coefficient for tilt low-pass filter         */
extern int16_t one_minus_decay;  /*  tilt filter first order iir filter.         */


extern int16_t R5ca;      /*  "a" coefficient for cascade 5th formant            */
extern int16_t R5cb;      /*  "b" coefficient                                    */
extern int16_t R5cc;      /*  "c" coefficient                                    */

extern int16_t rnpa;      /*  "a" coef for nasal pole                            */
extern int16_t rnpb;      /*  "b" coefficient                                    */
extern int16_t rnpc;      /*  "c" coefficient                                    */

extern int16_t rnza;      /*  "a" coefficient for nasal zero                     */
extern int16_t rnzb;      /*  "b" coefficient                                    */
extern int16_t rnzc;      /*  "c" coefficient                                    */

extern int16_t rlpa;      /*  "a" coefficient for low-pass filter                */
extern int16_t rlpb;      /*  "b" coefficient                                    */
extern int16_t rlpc;      /*  "c" coefficient                                    */

extern int16_t noiseb;    /*  "b" coefficient                                    */
extern int16_t noisec;    /*  "c" coefficient */

extern int16_t R4ca;      /*  "b" coefficient                                    */
extern int16_t R4cb;      /*  "b" coefficient                                    */
extern int16_t R4cc;      /*  "c" coefficient                                    */
extern int16_t R5cb;      /*  "b" coefficient                                    */
extern int16_t R5cc;      /*  "c" coefficient                                    */
extern int16_t R4pb;      /*  "b" coefficient                                    */
extern int16_t r4pc;      /*  "c" coefficient                                    */
extern int16_t R5pb;      /*  "b" coefficient                                    */
extern int16_t r3cg;      /*  Gain factor for 3rd formant                        */
extern int16_t r2cg;      /*  Gain factor for 2nd formant                        */
extern int16_t r1cg;      /*  Gain factor for 1st formant                        */
extern int16_t r5pc;      /*  "c" coefficient                                    */
extern int16_t fnscal;    /*  Q12 scale factor for F1inHZ, F2inHZ, and F3inHZ    */
extern int16_t avgain;    /*  Gain factor for voicing source                     */
extern int16_t APgain;    /*  Gain factor for aspiration source                  */
extern int16_t AFgain;    /*  Gain factor for frication source                   */
extern int16_t rnpa;      /*  "a" coef for nasal pole                            */
//extern int16_t f4c;       /*  Frequency of fixed cascade 4th formant             */
//extern int16_t b4c;       /*  Bandwidth of fixed cascade 4th formant             */
//extern int16_t f5c;       /*  Frequency of fixed cascade 5th formant             */
//extern int16_t b5c;       /*  Bandwidth of fixed cascade 5th formant             */
extern int16_t b2p;       /*  Bandwidth of parallel 2th formant                  */
extern int16_t b3p;       /*  Bandwidth of parallel 3th formant                  */
//extern int16_t f4p;       /*  Frequency of fixed parallel 4th formant            */
extern int16_t b4p;       /*  Bandwidth of fixed parallel 4th formant            */
//extern int16_t f5p;       /*  Frequency of fixed parallel 5th formant            */
extern int16_t b5p;       /*  Bandwidth of fixed parallel 5th formant            */
extern int16_t f6p;       /*  Frequency of fixed parallel 6th formant            */
extern int16_t b6p;       /*  Bandwidth of fixed parallel 6th formant            */
//extern int16_t fnp;       /*  Frequency of fixed nasal formant                   */
//extern int16_t bnp;       /*  Bandwidth of fixed nasal formant                   */
extern int16_t flp;       /*  Frequency of fixed glottal decimating filter       */
extern int16_t blp;       /*  Bandwidth of fixed glottal decimating filter       */
extern int16_t rlpg;      /*  Gain of down-sampling low-pass resonator           */
#ifndef MINIMAL_SYNTH
extern int16_t t0jitr;    /*  Alternating jitter for voicing period              */
#endif
extern int16_t fnscal;    /*  Q12 scale factor for F1inHZ, F2inHZ, and F3inHZ    */
extern int16_t avg;       /*  Gain factor for voicing                            */
extern int16_t abrg;      /*  Gain factor for breathiness                        */
extern int16_t apg;       /*  Gain factor for aspiration                         */
extern int16_t afg;       /*  Gain factor for frication                          */
extern int16_t rnpg;      /*  Gain factor for nasal pole resonator               */
extern int16_t a5gain;    /*  Gain in dB for the 5th cascade resonator.          */
extern int16_t a4gain;    /*  Gain in dB for the 4th cascade resonator.          */
extern int16_t a3gain;    /*  Gain in dB for the 3rd cascade resonator.          */
extern int16_t a2gain;    /*  Gain in dB for the 2nd cascade resonator.          */
extern int16_t a1gain;    /*  Gain in dB for the 1st cascade resonator.          */

extern int16_t r6pb;      /*  "b" coefficient                                    */
extern int16_t r6pc;      /*  "c" coefficient                                    */

extern int16_t noise;     /*  Output of random number generator                  */
extern int16_t voice0;    /*  Used for glottal pulse waveform generation         */
extern int16_t voice;     /*  Current sample of voicing waveform                 */
extern int16_t avgain;    /*  Gain factor for voicing source                     */
extern int16_t aturb1;    /*  Gain factor for breathiness of voice               */
extern int16_t APgain;    /*  Gain factor for aspiration source                  */
extern int16_t AFgain;    /*  Gain factor for frication source                   */
extern int16_t r3cg;      /*  Gain factor for 3rd formant                        */
extern int16_t r2cg;      /*  Gain factor for 2nd formant                        */
extern int16_t r1cg;      /*  Gain factor for 1st formant                        */

extern int16_t avlin;     /*  AVinDB converted to linear ampl. control           */
extern int16_t avlind;    /*  AV after convert from dB to linear, delayed        */
extern int16_t APlin;     /*  AP linear ampl. control                            */
extern int16_t ABlin;     /*  AB linear ampl. control                            */
extern int16_t r2pg;      /*  amp of parameter F2 for iir filter                 */
extern int16_t r3pg;      /*  amp of parameter F3 for iir filter                 */

extern int16_t a;         /*  Makes waveshape of glottal pulse when open         */
extern int16_t b;         /*  Makes waveshape of glottal pulse when open         */

extern int16_t r2pd1;    /*  Last output sample from parallel 2nd formant        */
extern int16_t r2pd2;    /*  Second-previous output sample                       */

extern int16_t r3pd1;    /*  Last output sample from parallel 3rd formant        */
extern int16_t r3pd2;    /*  Second-previous output sample                       */

extern int16_t r4pd1;    /*  Last output sample from parallel 4th formant        */
extern int16_t r4pd2;    /*  Second-previous output sample                       */

extern int16_t r5pd1;    /*  Last output sample from parallel 5th formant        */
extern int16_t r5pd2;    /*  Second-previous output sample                       */

extern int16_t r6pd1;    /*  Last output sample from parallel 6th formant        */
extern int16_t r6pd2;    /*  Second-previous output sample                       */

extern int16_t r1cd1;    /*  Last output sample from cascade 1st formant         */
extern int16_t r1cd2;    /*  Second-previous output sample                       */

extern int16_t r2cd1;    /*  Last output sample from cascade 2nd formant         */
extern int16_t r2cd2;    /*  Second-previous output sample                       */

extern int16_t r3cd1;    /*  Last output sample from cascade 3rd formant         */
extern int16_t r3cd2;    /*  Second-previous output sample                       */

extern int16_t r4cd1;    /*  Last output sample from cascade 4th formant         */
extern int16_t r4cd2;    /*  Second-previous output sample                       */

extern int16_t r5cd1;    /*  Last output sample from cascade 5th formant         */
extern int16_t r5cd2;    /*  Second-previous output sample                       */

extern int16_t rnpd1;    /*  Last output sample from cascade nasal pole          */
extern int16_t rnpd2;    /*  Second-previous output sample                       */

extern int16_t rnzd1;    /*  Last output sample from cascade nasal zero          */
extern int16_t rnzd2;    /*  Second-previous output sample                       */

extern int16_t rlpd1;    /*  Last output sample from low-pass filter             */
extern int16_t rlpd2;    /*  Second-previous output sample                       */

extern int16_t ablas1;   /*  Last output sample from nasal anti-resonator        */
extern int16_t ablas2;   /*  Second-previous output sample                       */

extern int16_t vlast;    /*  Last output sample of the tilt filter.              */


extern int16_t ranmul;
extern int16_t ranadd;
extern UINT uiNumberOfSamplesPerFrame;

unsigned short control;

extern int PlayTones(double DurationInMsec, double Freq_0, double Amp_0, double Freq_1, double Amp_1, double SampleRate );
extern void output_data();

int vtm_loop(unsigned short *input) {
    int i,tmp;
    int16_t temp2;
    int16_t temp3;
    int16_t temp4;
    int16_t temp5;
    int16_t temp6;
    int16_t tempAB;
    control=input[0];

    switch ( control & SPC_TYPE_MASK) {
        case SPC_type_voice:
            for (i = 1; i <= (VOICE_PARS); i++) {
              global_spc_buf[i] = input[i];
            }
            if ( !KS.halting ) {
                speech_waveform_generator();
                output_data();
            }
            break;
        case SPC_type_tone:
            for (i = 1; i <= (TONE_PARS); i++) {
                global_spc_buf[i] = input[i];
            }
            //  If not halting then generate tone samples.
            if (!KS.halting) {
                /* The elements of pVtm_t are used ,not been modified in PlayTones() function MVP MI */
                if (PlayTones((double)(global_spc_buf[1]),
                      (double)global_spc_buf[2],
                      (double)global_spc_buf[3],
                      (double)global_spc_buf[4],
                      (double)global_spc_buf[5], /*SampleRate*/ 11025 )) {
                    printf("Failed!\n");
                }
            }
            break;
        case SPC_type_speaker:
            // always init the VTM when we see a speakerdef!
            InitializeVTM();
            for (i = 1; i <= (SPDEF_PARS); i++) {
                global_spc_buf[i] = input[i];
            }
      read_speaker_definition();
            break;
        //  Process a Sync. packet.
        case SPC_type_sync:
            break;
        // TODO: SPC_type_index & SPC_type_force
        case SPC_type_samples_per_frame:
            InitializeVTM();
            global_spc_buf[1] = input[1];
            int16_t spf = ((((uiSampleRate * 64) + 5000) / 10000) * global_spc_buf[1]) / 100;
            if (spf > MAXIMUM_FRAME_SIZE) {
                spf = MAXIMUM_FRAME_SIZE;
            } else if (spf <= 0) {
                spf = 1;
            }
            uiNumberOfSamplesPerFrame = spf;
            break;
        default:
            break;
    }
}



extern int16_t Aturb;
extern int16_t k1;
extern int16_t k2;
extern int16_t R5ca;
void read_speaker_definition() {
  short *spdeftochip;

  int16_t f4c;	 /*  Frequency of fixed cascade 4th formant *//* MVP : Moved from vtm.h*/
  int16_t b4c;	 /*  Bandwidth of fixed cascade 4th formant *//* MVP : Moved from vtm.h*/
  int16_t f5c;	 /*  Frequency of fixed cascade 5th formant *//* MVP : Moved from vtm.h*/
  int16_t b5c;	 /*  Bandwidth of fixed cascade 5th formant *//* MVP : Moved from vtm.h*/
  int16_t f4p;	 /*  Frequency of fixed parallel 4th formant*//* MVP : Moved from vtm.h*/
  int16_t b4p;	 /*  Bandwidth of fixed parallel 4th formant*/
  int16_t f5p;	 /*  Frequency of fixed parallel 5th formant            */
  int16_t b5p;	 /*  Bandwidth of fixed parallel 5th formant            */
  int16_t fnp;	 /*  Frequency of fixed nasal formant                   */
  int16_t bnp;	 /*  Bandwidth of fixed nasal formant                   */
  int16_t flp;	 /*  Frequency of fixed glottal decimating filter       */
  int16_t blp;	 /*  Bandwidth of fixed glottal decimating filter       */
  int16_t rlpg;	 /*  Gain of down-sampling low-pass resonator           */
  int16_t avg;	 /*  Gain factor for voicing                            */
  int16_t apg;	 /*  Gain factor for aspiration                         */
  int16_t afg;	 /*  Gain factor for frication                          */
  int16_t rnpg;	 /*  Gain factor for nasal pole resonator               */
  int16_t a5gain;    /*  Gain in dB for the 5th cascade resonator.          */
  int16_t a4gain;    /*  Gain in dB for the 4th cascade resonator.          */
  int16_t a3gain;    /*  Gain in dB for the 3rd cascade resonator.          */
  int16_t a2gain;    /*  Gain in dB for the 2nd cascade resonator.          */
  int16_t a1gain;    /*  Gain in dB for the 1st cascade resonator.          */

  /********************************************************************/
  /*  Zero Vocal-Tract-Model parameters.                              */
  /********************************************************************/
  ldspdef=1;	 /* flag that we loaded a speaker def eab 10/96*/
  r2pd1 = 0;    /*  Last output sample from parallel 2nd formant        */
  r2pd2 = 0;    /*  Second-previous output sample                       */
  r3pd1 = 0;    /*  Last output sample from parallel 3rd formant        */
  r3pd2 = 0;    /*  Second-previous output sample                       */
  r4pd1 = 0;    /*  Last output sample from parallel 4th formant        */
  r4pd2 = 0;    /*  Second-previous output sample                       */
  r5pd1 = 0;    /*  Last output sample from parallel 5th formant        */
  r5pd2 = 0;    /*  Second-previous output sample                       */
  r6pd1 = 0;    /*  Last output sample from parallel 6th formant        */
  r6pd2 = 0;    /*  Second-previous output sample                       */
  r1cd1 = 0;    /*  Last output sample from cascade 1st formant         */
  r1cd2 = 0;    /*  Second-previous output sample                       */
  r2cd1 = 0;    /*  Last output sample from cascade 2nd formant         */
  r2cd2 = 0;    /*  Second-previous output sample                       */
  r3cd1 = 0;    /*  Last output sample from cascade 3rd formant         */
  r3cd2 = 0;    /*  Second-previous output sample                       */
  r4cd1 = 0;    /*  Last output sample from cascade 4th formant         */
  r4cd2 = 0;    /*  Second-previous output sample                       */
  r5cd1 = 0;    /*  Last output sample from cascade 5th formant         */
  r5cd2 = 0;    /*  Second-previous output sample                       */
  rnpd1 = 0;    /*  Last output sample from cascade nasal pole          */
  rnpd2 = 0;    /*  Second-previous output sample                       */
  rnzd1 = 0;    /*  Last output sample from cascade nasal zero          */
  rnzd2 = 0;    /*  Second-previous output sample                       */
  rlpd1 = 0;    /*  Last output sample from low-pass filter             */
  rlpd2 = 0;    /*  Second-previous output sample                       */
  ablas1 = 0;   /*  Last output sample from nasal anti-resonator        */
  ablas2 = 0;   /*  Second-previous output sample                       */
  vlast = 0;    /*  Last output sample of the tilt filter.              */
  /* Second sample of the tilt filter. 
     10/96 eab this filter parameter needs to be zeroed also */
  one_minus_decay = 0;
  rampdown = 0;

  avlind = 0;             // tek 08oct96
  voice0 = 0;             // tek 08oct96

  spdeftochip = &global_spc_buf[1];

  ranmul = 20077;
  ranadd = 12345;

  /********************************************************************/
  /*  Constants used to filter the noise.                             */
  /********************************************************************/

  switch( uiSampleRateChange )
  {
  case SAMPLE_RATE_INCREASE:

    noiseb = -2913; /* Q4.12 -> -0.711181640625 */
#ifndef CHANGES_AFTER_V43
    noisec = 1499; /* Q4.12 -> 0.365966796875 */
#endif
    break;

  case SAMPLE_RATE_DECREASE:

    noiseb = -1873; /* Q4.12 -> -0.457275390625 */
#ifndef CHANGES_AFTER_V43
    noisec = 1499; /* Q4.12 -> 0.365966796875 */
#endif
    break;

  case NO_SAMPLE_RATE_CHANGE:

    noiseb = -2913; /* Q4.12 -> -0.711181640625 */
#ifndef CHANGES_AFTER_V43
    noisec = 1499; /* Q4.12 -> 0.365966796875 */
#endif
    break;

  default:

    break;
  }

  /******************************************************************/
  /*  Coefficients for Pi-rotated anti-resonator. (This is the      */
  /*  same as an ordinary anti-resonator except for the c           */
  /*  coefficient has it's sign flipped).                           */
  /*  Frequency = 3500, Bandwidth = 1600.                           */
  /******************************************************************/
  // This produces the correct result @10kHz, but sounds strange at
  // other sample rates, so just use the constants above:
  //d2pole_pf( pVtm_t, &noiseb, &noisec, 3500, 1600, 0 );
  //noisec = -noisec;

  /********************************************************************/
  /*  Parallel 6th formant                                            */
  /*  Resonator constant "r6pa" is set elsewhere from A6inDB.         */
  /********************************************************************/

  /*r6pb = -5702;*/     /* Q4.12, MVP : moved as a local constant */
  /*r6pc = -1995;                */     /* Q4.12, MVP : moved as a local constant */

  /********************************************************************/
  /*  Coefficients for fixed nasal formant.                           */
  /*  Gain parameter "rnpa" is set below by speaker definition        */
  /*  parameter "rnpg". Note possible future change, this could vary  */
  /*  from speaker to speaker.                                        */
  /********************************************************************/

  fnp = 290;
  bnp = 70;

  d2pole_pf( &rnpb, &rnpc, fnp, bnp, 0 );

  switch( uiSampleRateChange )
  {
  case SAMPLE_RATE_INCREASE:
    flp = 948; /* 860 * 1.1025 */
    blp = 615; /* 558 * 1.1025 */
    rlpg = 2400; /* Q4.12 -> 0.5859375 */
    break;
  case SAMPLE_RATE_DECREASE:
    flp = 698;
    blp = 453;
    rlpg = 2400; /* Q4.12 -> 0.5859375 */
    break;
  case NO_SAMPLE_RATE_CHANGE:
  default:
    flp = 860;
    blp = 558;
    rlpg = 2400; /* Q4.12 -> 0.5859375 */
    break;
  }

  rlpa = d2pole_pf( &rlpb, &rlpc, flp, blp, rlpg );

  /********************************************************************/
  /*  Begin set coeficients of speaker-def controlled resonators.     */
  /*  Cascade fourth formant                                          */
  /********************************************************************/

  f4c = ((SP_CHIP *)spdeftochip)->r4cb;   /*  1                       */
  b4c = ((SP_CHIP *)spdeftochip)->r4cc;   /*  2                       */

  d2pole_cf45( &R4cb, &R4cc, f4c, b4c, 0 );

  /********************************************************************/
  /*  Cascade fifth formant                                           */
  /********************************************************************/

  f5c = ((SP_CHIP *)spdeftochip)->r5cb;   /*  3                       */
  b5c = ((SP_CHIP *)spdeftochip)->r5cc;   /*  4                       */

  d2pole_cf45(&R5cb, &R5cc, f5c, b5c, 0 );

  /********************************************************************/
  /*  Parallel fourth formant                                         */
  /********************************************************************/

  f4p = ((SP_CHIP *)spdeftochip)->r4pb;   /*  5                       */
  b4p = 400;

  d2pole_pf( &R4pb, &r4pc, f4p, b4p, 0 );

  /********************************************************************/
  /*  Parallel fifth formant                                          */
  /********************************************************************/

  f5p = ((SP_CHIP *)spdeftochip)->r5pb;    /*  6                      */
  b5p = 500;

  d2pole_pf( &R5pb, &r5pc, f5p, b5p, 0 );

  /********************************************************************/
  /*  Parallel sixth formant                                          */
  /*  Resonator constant "r6pa" is set in function                    */
  /*  speech_waveform_generator() from A6inDB.                        */
  /*  f6p = 4884.0 Hz. and b6p = 1145.0 Hz.                           */
  /*  It is not necessary to change these with a nominal change in    */
  /*  sample rate.                                                    */
  /********************************************************************/

  r6pb = -5702;
  r6pc = -1995;

  /********************************************************************/
  /*  End of set coeficients of speaker definition controlled         */
  /*  resonators.                                                     */
  /********************************************************************/

  /********************************************************************/
  /*  The following parameter deleted 12-JUL-85                       */
  /*  TLToff = ((SP_CHIP *)spdeftochip)->tltoff;      7               */
  /********************************************************************/

  /********************************************************************/
  /*  Jitter parameter has it's sign flipped from the current value.  */
  /*  t0jitr is a time, so it should be scaled if fs != 10K.          */
  /********************************************************************/

  if ( t0jitr < 0 )
    t0jitr = -((SP_CHIP *)spdeftochip)->t0jit;    /*  8               */
  else
    t0jitr = ((SP_CHIP *)spdeftochip)->t0jit;

  switch( uiSampleRateChange )
  {
  case SAMPLE_RATE_INCREASE:

    t0jitr = frac1mul( rate_scale, t0jitr ) << 1;
    break;

  case SAMPLE_RATE_DECREASE:

    t0jitr = frac1mul( rate_scale, t0jitr );
    break;

  case NO_SAMPLE_RATE_CHANGE:

    break;

  default:

    break;
  }

  /********************************************************************/
  /*  Gains of cascade resonators (used to keep signal in high order  */
  /*  bits).                                                          */
  /********************************************************************/
  a5gain = ((SP_CHIP *)spdeftochip)->r5ca;    /*  9                   */
  R5ca = amptable[a5gain];
  a4gain = ((SP_CHIP *)spdeftochip)->r4ca;    /*  10                  */
  R4ca = amptable[a4gain];
  a3gain = ((SP_CHIP *)spdeftochip)->r3ca;    /*  11                  */
  r3cg = amptable[a3gain];
  a2gain = ((SP_CHIP *)spdeftochip)->r2ca;    /*  12                  */
#ifdef LOW_COST_VERSION
  r2cg = amptable[a2gain-10];
#else
        r2cg = amptable[a2gain];
#endif
  a1gain = ((SP_CHIP *)spdeftochip)->r1ca;    /*  13                  */
  r1cg = amptable[a1gain];

  /********************************************************************/
  /*  Constants that determine the duration of the open phase of the  */
  /*  glottal period.                                                 */
  /********************************************************************/

  k1 = ((SP_CHIP *)spdeftochip)->nopen1;      /* 14                   */
  k2 = ((SP_CHIP *)spdeftochip)->nopen2;      /* 15                   */

  /********************************************************************/
  /*  Breathiness coefficient.                                        */
  /********************************************************************/

  Aturb = ((SP_CHIP *)spdeftochip)->aturb;    /*  16                  */
  Aturb = amptable[Aturb];

  /********************************************************************/
 /*  T0range = ((SP_CHIP *)spdeftochip)->f0scale;  17  (Not used)    */
 /********************************************************************/

  /********************************************************************/
  /*  Scale factor for variable formants F1inHZ, F2inHZ, and F3inHZ.  */
  /********************************************************************/

  fnscal = ((SP_CHIP *)spdeftochip)->fnscale;     /*  18              */

  /********************************************************************/
  /*  Overall gain of frication source re other sources               */
  /********************************************************************/

  afg = ((SP_CHIP *)spdeftochip)->afgain;     /*  19                  */

  AFgain = amptable[afg ];


  /********************************************************************/
  /*  Gain of the nasal pole resonator of cascade filter section.     */
  /********************************************************************/

  rnpg = ((SP_CHIP *)spdeftochip)->rnpgain;   /*  20                  */
  rnpa = amptable[rnpg];
  

  /********************************************************************/
  /*  Overall gain of voicing source relative to other sources        */
  /********************************************************************/

  avg = ((SP_CHIP *)spdeftochip)->azgain;     /*  21                  */
  avgain = amptable[avg];

  apg = ((SP_CHIP *)spdeftochip)->apgain;     /*  22                  */
  APgain = amptable[apg];


  /********************************************************************/
  /*  Overall speaker gain                                            */
  /********************************************************************/

  //SpeakerGain = ((SP_CHIP *)spdeftochip)->osgain;

  //phTTS->pKernelShareData->uiCurrentSpeaker = (DWORD)((SP_CHIP *)spdeftochip)->speaker;
  return;
}
