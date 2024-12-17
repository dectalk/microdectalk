/* THIS FILE FOR CALLER ID 8khz ONLY---eab 2/8/02*/
/*
 ***********************************************************************
 *
 *                           Coryright (c)
 *    © Digital Equipment Corporation 1995. All rights reserved.
 * Copyright (c)        1984                    by Dennis H. Klatt
 * Copyright (c) 1984 Digital Equipment Corporation, Maynard, Mass.
 * Copyright (c) 1993 Digital Equipment Corporation, Maynard, Mass.
 *
 *    Restricted Rights: Use, duplication, or disclosure by the U.S.
 *    Government is subject to restrictions as set forth in subparagraph
 *    (c) (1) (ii) of DFARS 252.227-7013, or in FAR 52.227-19, or in FAR
 *    52.227-14 Alt. III, as applicable.
 *
 *    This software is proprietary to and embodies the confidential
 *    technology of Digital Equipment Corporation and other parties.
 *    Possession, use, or copying of this software and media is authorized
 *    only pursuant to a valid written license from Digital or an
 *    authorized sublicensor.
 *
 ***********************************************************************
 *    File Name:	vtm_i.c
 *    Author:
 *    Creation Date:
 *
 *    Functionality:
 *
 ***********************************************************************
 *    Revision History:
 * 0001 23-MAR-95 CJL Add vtm_ip.h and auto-generate proto files
 *                    add vtm_idef.h to contain symbol definitions.
 */

/**********************************************************************/
/*  Include Files.                                                    */
/**********************************************************************/
//#define VARIABLE_OPEN_PERIOD
#include "vtm_ip.h"
#include "vtm_idef.h"
#include "viport.h"
#include "phdefs.h"
//#include "viphdefs.h"
#include "vismprat.h"   /*  Constants used by the vocal tract model   */
#include "vtm_i.h"      /*  Variables used by the vocal tract model   */
#include "vtmtabli.h"   /*  Tables used by the vocal tract model      */
#include "vtmfunci.h"   /*  Functions used by the vocal tract model   */
#include <stdio.h>

// help #include "AudioMapping.h"

extern void read_speaker_definition(void);
extern void write_wav(short *iwave, int length);

short inc=0;

static short count=0;
static short samples=0;
short ciwave=0;
short par_count=5;
short cas_count=20;
short vcount=0;

short ampsum;

//#define USEBUF

#define IWSIZE 71
short iwavefull[IWSIZE];
short togglebuff = 0;

extern short arg1,arg2,arg3; /*eab 3/18/95 for math functions */
/**********************************************************************/
/*  Function prototypes                                               */
/* 23-mar-95 cjl Replaced by vtm_ip.h.								  */
/**********************************************************************/
/*void speech_waveform_generator();                                   */
/*void read_speaker_definition();									  */
/*void SetSampleRate( unsigned int uiSampRate );                      */
/*void InitializeVTM();                                               */
/**********************************************************************/
/**********************************************************************/
/*  speech_waveform_generator                                         */
/**********************************************************************/
/**********************************************************************/

extern short	spc_dma_lengths[];

extern short *global_spc_buf;
extern short global_spc_v_buf[VOICE_PARS+2];

void VocalTract();

void output_data(void);

int16_t ranmul;
int16_t ranadd;
extern int16_t noisec;

void SetSampleRate( uint16_t uiSampRate ) {
  uint32_t qval;

  uiSampleRate = uiSampRate;
  SampleRate = (double)uiSampleRate;
  SamplePeriod = 1.0 / SampleRate;
  if ( uiSampleRate == PC_SAMPLE_RATE ) {
    bEightKHz = FALSE;

    uiSampleRateChange = SAMPLE_RATE_INCREASE; // 11025

    qval = ((((1<<14)*(uint32_t)uiSampleRate)+5000)/10000);
    rate_scale = qval;   /*  Equals 1.1025 in Q14 format for 11 KHz.    */
    qval = (((((1<<15)*(uint32_t)10000))+(uiSampleRate/2))/uiSampleRate);
    inv_rate_scale = qval;    /*  Equals 0.909 in Q15 format.        */
    uiNumberOfSamplesPerFrame = ((uiSampleRate*64)+5000)/10000;
  } else {
    if ( uiSampleRate == MULAW_SAMPLE_RATE ) {
      bEightKHz = TRUE;
      uiSampleRateChange = SAMPLE_RATE_DECREASE;
      rate_scale = 26214;    /*  Equals 0.8 in Q15 format for 8 KHz */
      inv_rate_scale = 20480;    /*  Equals 1.25 in Q14 format.     */
      uiNumberOfSamplesPerFrame = 51;
    } else {
      uiSampleRateChange = NO_SAMPLE_RATE_CHANGE;
    }
  }
  //async_voice = last_voice ;
  //async_change |= ASYNC_voice;

  return;
}

void vtm_main() {
    noisec = 1499;  /*  "c" coefficient                                    */
    ranmul = 20077; /*  Magic constant to make next random number          */
    ranadd = 12345; /*  Magic constant to make next random number          */

    t0jitr = 7;
    uiSampleRateChange = SAMPLE_RATE_INCREASE;
    rate_scale = 18063;
    inv_rate_scale = 29722;
    bEightKHz = FALSE;
    SampleRate = 11025;
    uiNumberOfSamplesPerFrame = 71;
    uiSampleRate = 11025;
    SamplePeriod = 9.07029478458E-5;
    SetSampleRate(PC_SAMPLE_RATE);
}
extern FILE *outfile;

void output_data(void) {
    write_wav(iwave, 71);
}

int16_t ldspdef;

void speech_waveform_generator() {
  // READ VARIABLE PARAMETERS FOR ONE FRAME (64 SAMPLES) OF SPEECH
  int16_t *variabpars = &global_spc_buf[1];

  /* EAB If we just loaded a speaker def zero all vocal tract gains
     so that this looks like a silence packet (last packet of previous frame
     stuck here*/
  if (ldspdef >= 1) {
    ldspdef++;
    variabpars[OUT_AV] = 0;
    variabpars[OUT_AP] = 0;
    variabpars[OUT_A2] = 0;
    variabpars[OUT_A3] = 0;
    variabpars[OUT_A4] = 0;
    variabpars[OUT_A5] = 0;
    variabpars[OUT_A6] = 0;
    variabpars[OUT_AB] = 0;
    avlin = 0;
  }

  if (ldspdef >= 3)
    ldspdef = -1;

  /********************************************************************/
  /********************************************************************/
  /*                                                                  */
  /*  Scaling for various sample rates is done in the following       */
  /*  fashion.                                                        */
  /*                                                                  */
  /*  If the sample rate is modified then the formant frequencies,    */
  /*  bandwidths, and the pitch period are all scaled. If the sample  */
  /*  rate is decreased the fifth formant is eliminated, and the gain */
  /*  r2pg is lowered to prevent overload.                            */
  /*                                                                  */
  /********************************************************************/
  /********************************************************************/

  /********************************************************************/
  /* T0inS4 is a time, so it should be scaled if fs != 10K.           */
  /********************************************************************/

  T0inS4 = variabpars[OUT_T0];
  switch( uiSampleRateChange ) {
  case SAMPLE_RATE_INCREASE:
    T0inS4 = frac1mul( rate_scale, T0inS4 ) << 1;
    break;
  case SAMPLE_RATE_DECREASE:
    T0inS4 = frac1mul( rate_scale, T0inS4 );
    break;
  default:
    break;
  }


  F1inHZ = variabpars[OUT_F1];
  F1inHZ = frac4mul( F1inHZ, fnscal ) +(int16_t) ((4096 - (int32_t)fnscal ) >> 4);
  F2inHZ = variabpars[OUT_F2];
  F2inHZ = frac4mul( F2inHZ, fnscal ) + (int16_t) ((4096 - (int32_t)fnscal ) >> 3);
  F3inHZ = variabpars[OUT_F3];
  F3inHZ = frac4mul( F3inHZ, fnscal );

  /********************************************************************/
  /*  Scale the nasal anti-resonator frequency for the sample rate.   */
  /********************************************************************/

  FZinHZ = variabpars[OUT_FZ];
  switch( uiSampleRateChange ) {
  case SAMPLE_RATE_INCREASE:
    FZinHZ = frac1mul( inv_rate_scale, FZinHZ );
    break;
  case SAMPLE_RATE_DECREASE:
    FZinHZ = frac1mul( inv_rate_scale, FZinHZ ) << 1;
    break;
  default:
    break;
  }


  B1inHZ = variabpars[OUT_B1];
  B2inHZ = variabpars[OUT_B2];
  B3inHZ = variabpars[OUT_B3];
  AVinDB = variabpars[OUT_AV];
  APinDB = variabpars[OUT_AP];
  A2inDB = variabpars[OUT_A2];
  A3inDB = variabpars[OUT_A3];
  A4inDB = variabpars[OUT_A4];
  A5inDB = variabpars[OUT_A5];
  A6inDB = variabpars[OUT_A6];
  ABinDB = variabpars[OUT_AB];
  TILTDB = variabpars[OUT_TLT] - 12;  /*  Tilt in dB at 3 kHz         */

  // FIXME: these %88 prevent a crash but dont solve the reason that the indices are out of bounds
  APlin = amptable[(APinDB + 10) % 88];      /*  Convert dB to linear        */
  r2pg = amptable[(A2inDB + 13)%88];       /*  Convert dB to linear        */
  r3pg = amptable[(A3inDB + 10)%88];       /*  Convert dB to linear        */
  r4pa = amptable[(A4inDB + 7)%88];        /*  Convert dB to linear        */
  r5pa = amptable[(A5inDB + 6)%88];        /*  Convert dB to linear        */
  r6pa = amptable[(A6inDB + 5)%88];        /*  Convert dB to linear        */
  ABlin = amptable[(ABinDB + 5)%88];       /*  Convert dB to linear        */

/*
  ampsum = A2inDB + A3inDB + A4inDB + A5inDB + A6inDB + ABinDB;
  if(ampsum)
	  par_count=5;
  else
	  if(par_count)
		  par_count--;
*/

  APlin = frac4mul( APlin, APgain );  /*  Scale asp by spdef GV       */
  r2pg = frac1mul(r2pg, AFgain  );    /*  Scale A2 by spdef GF        */
  r3pg = frac1mul(r3pg, AFgain );    /*  Scale A3 by spdef GF        */
  r4pa = frac1mul(r4pa, AFgain  );    /*  Scale A4 by spdef GF        */
  r5pa = frac1mul(r5pa, AFgain);    /*  Scale A5 by spdef GF        */
  r6pa = frac1mul(r6pa, AFgain);    /*  Scale A6 by spdef GF        */
  ABlin = frac4mul( ABlin, AFgain );  /*  Scale AB by spdef GF        */

//  d2pole_pf( &rnpb, &rnpc, 290, 70, 0 );
//  rlpa = d2pole_pf( &rlpb, &rlpc, 698, 453, 2400 );

  /********************************************************************/
  /*  Set coeficients of variable parallel resonator R2.              */
  /********************************************************************/
  b2p = 210;
  r2pa = d2pole_pf( &r2pb, &r2pc, F2inHZ, b2p, r2pg );

  /********************************************************************/
  /*  Set coeficients of variable parallel resonator R3.              */
  /********************************************************************/
  b3p = 280;
  r3pa = d2pole_pf( &r3pb, &r3pc, F3inHZ, b3p, r3pg );

  /********************************************************************/
  /********************************************************************/
  /*  MAIN LOOP. Calculate each sample of the current frame.          */
  /********************************************************************/
  /********************************************************************/

  VocalTract();
}

/**********************************************************************/
/**********************************************************************/
/*  Function: read_speaker_definition                                 */
/*  Read a 22 words speaker definition packet.                        */
/**********************************************************************/
/**********************************************************************/

/**********************************************************************/
/**********************************************************************/
/*                                                                    */
/*  Function: InitializeVTM                                           */
/*  Author: Bill Hallahan                                             */
/*  Date: September 27, 1993                                          */
/*                                                                    */
/*  Vocal Tract Model Initialization                                  */
/*                                                                    */
/**********************************************************************/
/**********************************************************************/
void InitializeVTM() {

  /********************************************************************/
  /*  Zero Vocal-Tract-Model parameters.                              */
  /********************************************************************/

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

        one_minus_decay = 0;    /* Second sample of the tilt filter.          */

//        SpeakerGain = 0;        /* Overall speaker gain */
}
