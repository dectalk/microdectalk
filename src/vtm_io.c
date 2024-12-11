#include "vtm_ip.h"
#include "vtm_idef.h"
#include "viport.h"
#include "phdefs.h"
#include "dtk/dtmmedefs.h"
#include "vtm_idef.h"
#include <stdio.h>

extern short *global_spc_buf;
short noiseb;

#define SPC_TYPE_MASK			(0x00ff)
#define TONE_PARS        5

extern S16 R4ca;      /*  "b" coefficient                                    */
extern S16 R4cb;      /*  "b" coefficient                                    */
extern S16 R4cc;      /*  "c" coefficient                                    */
extern S16 R5cb;      /*  "b" coefficient                                    */
extern S16 R5cc;      /*  "c" coefficient                                    */
extern S16 R4pb;      /*  "b" coefficient                                    */
extern S16 r4pc;      /*  "c" coefficient                                    */
extern S16 R5pb;      /*  "b" coefficient                                    */
extern S16 r3cg;      /*  Gain factor for 3rd formant                        */
extern S16 r2cg;      /*  Gain factor for 2nd formant                        */
extern S16 r1cg;      /*  Gain factor for 1st formant                        */
extern S16 r5pc;      /*  "c" coefficient                                    */
extern S16 fnscal;    /*  Q12 scale factor for F1inHZ, F2inHZ, and F3inHZ    */
extern S16 avgain;    /*  Gain factor for voicing source                     */
extern S16 APgain;    /*  Gain factor for aspiration source                  */
extern S16 AFgain;    /*  Gain factor for frication source                   */
extern S16 rnpa;      /*  "a" coef for nasal pole                            */

extern UINT uiNumberOfSamplesPerFrame;

unsigned short control;

extern BOOL PlayTones(double DurationInMsec, double Freq_0, double Amp_0, double Freq_1, double Amp_1, double SampleRate );
extern void output_data();

int vtm_loop(unsigned short *input) {
    int i,tmp;
    S16 temp2;
    S16 temp3;
    S16 temp4;
    S16 temp5;
    S16 temp6;
    S16 tempAB;
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
                      (double)global_spc_buf[5], /*pVtm_t->SampleRate*/ 11025 )) {
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
            S16 spf = ((((uiSampleRate * 64) + 5000) / 10000) * global_spc_buf[1]) / 100;
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




void read_speaker_definition() 
{


  short *spdeftochip;

S16 a5gain;    /*  Gain in dB for the 5th cascade resonator.          */
S16 a4gain;    /*  Gain in dB for the 4th cascade resonator.          */
S16 a3gain;    /*  Gain in dB for the 3rd cascade resonator.          */
S16 a2gain;    /*  Gain in dB for the 2nd cascade resonator.          */
S16 a1gain;    /*  Gain in dB for the 1st cascade resonator.          */
S16 apg;       /*  Gain factor for aspiration                         */
S16 afg;       /*  Gain factor for frication                          */
S16 avg;       /*  Gain factor for voicing                            */
S16 rnpg;      /*  Gain factor for nasal pole resonator               */
S16 R5ca;      /*  "a" coefficient for cascade 5th formant            */
S16 f4c;       /*  Frequency of fixed cascade 4th formant             */
S16 f4p;       /*  Frequency of fixed parallel 4th formant            */
S16 b4c;       /*  Frequency of fixed cascade 4th formant             */
S16 f5c;       /*  Frequency of fixed cascade 4th formant             */
S16 f5p;       /*  Frequency of fixed parallel 4th formant            */
S16 b5c;       /*  Bandwidth of fixed cascade 5th formant             */


  spdeftochip = &global_spc_buf[1];
  /********************************************************************/
  /*  Constants used in random number generation.                     */
  /********************************************************************/

//  ranmul = 20077;
 // ranadd = 12345;

  /********************************************************************/
  /*  Constants used to filter the noise.                             */
  /********************************************************************/

 //   noiseb = -1873;
//    noisec = 1499;
  /********************************************************************/
  /*  Parallel 6th formant                                            */
  /*  Resonator constant "r6pa" is set elsewhere from A6inDB.         */
  /********************************************************************/

//  r6pb = -5702;
 // r6pc = -1995;

  /********************************************************************/
  /*  Coefficients for fixed nasal formant.                           */
  /*  Gain parameter "rnpa" is set below by speaker definition        */
  /*  parameter "rnpg". Note possible future change, this could vary  */
  /*  from speaker to speaker.                                        */
  /********************************************************************/

 // fnp = 290;
 // bnp = 70;


  /********************************************************************/
  /*  Coefficients for fixed downsampling low-pass filter             */
  /********************************************************************/



 //   flp = 698;
 //   blp = 453;
 //   rlpg = 2400;

  

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

  d2pole_cf45( &R5cb, &R5cc, f5c, b5c, 0 );

  /********************************************************************/
  /*  Parallel fourth formant                                         */
  /********************************************************************/

  f4p = ((SP_CHIP *)spdeftochip)->r4pb;   /*  5                       */
 // b4p = 400;

  d2pole_pf( &R4pb, &r4pc, f4p, 400, 0 );

  /********************************************************************/
  /*  Parallel fifth formant                                          */
  /********************************************************************/

  f5p = ((SP_CHIP *)spdeftochip)->r5pb;    /*  6                      */
//  b5p = 500;

  d2pole_pf( &R5pb, &r5pc, f5p, 500, 0 );

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
  r2cg = amptable[a2gain];
  a1gain = ((SP_CHIP *)spdeftochip)->r1ca;    /*  13                  */
  r1cg = amptable[a1gain];

  
  /********************************************************************/
  /*  Breathiness coefficient.                                        */
  /********************************************************************/

  //Aturb = ((SP_CHIP *)spdeftochip)->aturb;    /*  16                  */
  //Aturb = amptable[Aturb];

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
  AFgain = amptable[afg];

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

  /********************************************************************/
  /*  Overall gain of aspiration source relative to other sources     */
  /********************************************************************/

  apg = ((SP_CHIP *)spdeftochip)->apgain;     /*  22                  */
  APgain = amptable[apg];

  //uiCurrentSpeaker = (DWORD)((SP_CHIP *)spdeftochip)->speaker;

  return;
}
