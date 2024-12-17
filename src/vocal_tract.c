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
//#include "vtm_idef.h"
#include "viport.h"
#include "phdefs.h"
//#include "viphdefs.h"
//#include "vismprat.h"   /*  Constants used by the vocal tract model   */
//#include "vtm_i.h"      /*  Variables used by the vocal tract model   */
//#include "vtmtabli.h"   /*  Tables used by the vocal tract model      */
#include "vtmfunci.h"   /*  Functions used by the vocal tract model   */
#include <stdio.h>

#include "vtm_idef.h"

#define OUT_PH  17
/**********************************************************************/
/*  Temporary variables used by in-line functions and by the vocal    */
/*  tract model.                                                      */
/**********************************************************************/

extern int32_t temp0;
extern int32_t temp1;

extern const int16_t B0[224];

/**********************************************************************/
/*  VARIABLES TO HOLD SPEAKER DEFINITION FROM HOST:                   */
/**********************************************************************/

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

/**********************************************************************/
/*  CONSTANTS OF SPEAKER DEFINITION                                   */
/*  below are r6pa, r6pb, r6pc, rnpa, rnpb, rnpc, rlpa, rlpb, rlpc    */
/**********************************************************************/

/**********************************************************************/
/*  VARIABLES TO HOLD INPUT PARAMETERS FROM HOST:                     */
/**********************************************************************/

extern int16_t T0inS4;  /*  Voicing fund freq in Hz*10,      500 to 1650         */
extern int16_t F1inHZ;  /*  First formant freq in Hz,        200 to 1300         */
extern int16_t F2inHZ;  /*  Second formant freq in Hz,       550 to 3000         */
extern int16_t F3inHZ;  /*  Third formant freq in Hz,       1200 to 4800         */
extern int16_t FZinHZ;  /*  Nasal zero freq in Hz,           248 to  528         */
extern int16_t B1inHZ;  /*  First formant bw in Hz,           40 to 1000         */
extern int16_t B2inHZ;  /*  Second formant bw in Hz,          40 to 1000         */
extern int16_t B3inHZ;  /*  Third formant bw in Hz,           40 to 1000         */
extern int16_t AVinDB;  /*  Amp of voicing in dB,             0 to   70          */
extern int16_t APinDB;  /*  Amp of aspiration in dB,          0 to   70          */
extern int16_t A2inDB;  /*  Amp of F2 frication in dB,        0 to   80          */
extern int16_t A3inDB;  /*  Amp of F3 frication in dB,        0 to   80          */
extern int16_t A4inDB;  /*  Amp of F4 frication in dB,        0 to   80          */
extern int16_t A5inDB;  /*  Amp of F5 frication in dB,        0 to   80          */
extern int16_t A6inDB;  /*  Amp of F6                         0 to   80          */
extern int16_t ABinDB;  /*  Amp of bypass fric. in dB,        0 to   80          */
extern int16_t TILTDB;  /*  Voicing spectral tilt in dB,      0 to   24          */
extern int16_t Aturb;   /*  Amplitude of breathiness in dB,   0 to   24          */

/**********************************************************************/
/*  INTERNAL MEMORY FOR DIGITAL RESONATORS AND ANTIRESONATOR          */
/**********************************************************************/

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

/**********************************************************************/
/*  MEMORY VARIABLES FOR ONE-POLE and ONE-ZERO FILTERING              */
/**********************************************************************/

extern int16_t nolast;    /*  Previous value of noisea                           */

/**********************************************************************/
/*  COEFFICIENTS FOR DIGITAL RESONATORS AND ANTIRESONATOR             */
/**********************************************************************/

extern int16_t r2pa;      /*  "a" coef for parallel 2nd formant                  */
extern int16_t r2pb;      /*  "b" coefficient                                    */
extern int16_t r2pc;      /*  "c" coefficient                                    */

extern int16_t r3pa;      /*  "a" coef for parallel 3rd formant                  */
extern int16_t r3pb;      /*  "b" coefficient                                    */
extern int16_t r3pc;      /*  "c" coefficient                                    */

extern int16_t r4pa;      /*  "a" coef for parallel 4th formant                  */
extern int16_t R4pb;      /*  "b" coefficient                                    */
extern int16_t r4pc;      /*  "c" coefficient                                    */

extern int16_t r5pa;      /*  "a" coef for parallel 5th formant                  */
extern int16_t R5pb;      /*  "b" coefficient                                    */
extern int16_t r5pc;      /*  "c" coefficient                                    */

extern int16_t r6pa;      /*  "a" coef for parallel 6th formant                  */
int16_t r6pb;      /*  "b" coefficient                                    */
int16_t r6pc;      /*  "c" coefficient                                    */

extern int16_t R1ca;      /*  "a" coefficient for cascade 1st formant            */
extern int16_t r1cb;      /*  "b" coefficient                                    */
extern int16_t r1cc;      /*  "c" coefficient                                    */

extern int16_t R2ca;      /*  "a" coefficient for cascade 2nd formant            */
extern int16_t r2cb;      /*  "b" coefficient                                    */
extern int16_t r2cc;      /*  "c" coefficient                                    */

extern int16_t R3ca;      /*  "a" coefficient for cascade 3rd formant            */
extern int16_t r3cb;      /*  "b" coefficient                                    */
extern int16_t r3cc;      /*  "c" coefficient                                    */

extern int16_t R4ca;      /*  "a" coefficient for cascade 4th formant            */
extern int16_t R4cb;      /*  "b" coefficient                                    */
extern int16_t R4cc;      /*  "c" coefficient                                    */

int16_t R5ca;      /*  "a" coefficient for cascade 5th formant            */
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
int16_t noisec;    /*  "c" coefficient                                    */

extern int16_t decay;     /*  decay coefficient for tilt low-pass filter         */
extern int16_t one_minus_decay;  /*  tilt filter first order iir filter.         */

/**********************************************************************/
/*  OUTPUT SIGNAL FROM EACH ANTIRESONATOR                             */
/*  (Output of a resonator is r__d1 variable defined above)           */
/**********************************************************************/

extern int16_t rnzout;    /*  Output signal from nasal zero                      */
extern int16_t about;     /*  Output signal from bypass path                     */

/**********************************************************************/
/*  OTHER OUTPUT SIGNALS                                              */
/**********************************************************************/

extern int16_t noise;     /*  Output of random number generator                  */
extern int16_t voice0;    /*  Used for glottal pulse waveform generation         */
extern int16_t voice;     /*  Current sample of voicing waveform                 */

/**********************************************************************/
/*  FIXED AND VARIABLE GAIN FACTORS                                   */
/**********************************************************************/

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

extern int16_t k1;        /*  Equals nopen1 for glottal period duration.         */
extern int16_t k2;        /*  Equals nopen2 for glottal period duration.         */

/**********************************************************************/
/*  COUNTERS                                                          */
/**********************************************************************/

extern int16_t ns;  /*  Number of samples into current frame            */
/* eab was this U32 ns;*/  /*  Number of samples into current frame   */
extern int16_t nper;      /*  Current loc in voicing period   40000 samp/s       */
extern int16_t nsr4;      /*  Counter of 4 samples in glottal source loop        */

/**********************************************************************/
/*  COUNTER LIMITS                                                    */
/**********************************************************************/

extern int16_t T0;        /*  Fundamental period in output samples times 4       */
extern int16_t nopen;     /*  Number of samples in open phase of period          */
extern int16_t nmod;      /*  Position in period to begin noise amp. modul       */

/**********************************************************************/
/*  ALL-PURPOSE TEMPORARY VARIABLES                                   */
/**********************************************************************/

extern int16_t temp;
extern int16_t f;         /*  Frequency argument to setb()                       */
extern int16_t bw;        /*  Bandwidth argument to setc()                       */
extern int16_t gain;      /*  Gain term used in seta() calculations              */

/**********************************************************************/
/*  OTHER VARIABLES                                                   */
/**********************************************************************/

extern int16_t randomx;    /*  Varible used by random unmber generator           */
/*int32_t ranmul;*/    /*  Magic constant to make next random number          */
//extern int16_t ranmul;    /*  Magic constant to make next random number          */
/*int32_t ranadd;*/    /*  Magic constant to make next random number          */
//extern int16_t  ranadd;    /*  Magic constant to make next random number          */

extern int16_t out;       /*  Output signal from the vocal tract model           */


extern const int16_t voices[14];
extern const int16_t radius_table[];
extern const int16_t amptable[88];
extern const int16_t azero_tab[35];
extern const int16_t bzero_tab[35];
extern const int16_t czero_tab[35];
extern int16_t iwave[];

extern short vcount;
extern short cas_count;
extern short par_count;


int16_t rampdown;
extern short *global_spc_buf;
extern short global_spc_v_buf[VOICE_PARS+2];
extern short arg1,arg2; /*eab 3/18/95 for math functions*/	

extern int16_t ranmul;
extern int16_t ranadd;
extern UINT uiNumberOfSamplesPerFrame;

void VocalTract() {
    int16_t *variabpars;
    variabpars = &global_spc_buf[1];

    for ( ns = 0; ns < uiNumberOfSamplesPerFrame; ns++ ) {
        /******************************************************************/
        /*  NOISE GENERATOR                                               */
        /******************************************************************/
        randomx = (randomx * ranmul) + ranadd;
        noise = randomx >> 2;

        /******************************************************************/
        /*  RANDOM NUMBER FOR FRICATION AND ASPIRATION                    */
        /*  Tilt down aspiration noise spectrum at high freqs by low-pass */
        /*  filtering. noise = noise + 0.75 * last noise                  */
        /******************************************************************/

        noise += frac1mul( 24574, nolast );
        nolast = noise;

        /******************************************************************/
        /*                                                                */
        /*  Filter with Pi-rotated antiresonator. (This is the same as an */
        /*  ordinary antiresonator except for the b-coef has it's sign    */
        /*  flipped). Frequency = 3500, Bandwidth = 1600.                 */
        /*                                                                */
        /*  This is a three zero filter, but is implemented as a special  */
        /*  case because one of the multipliers is 1.0. The input and     */
        /*  output variable is "noise".                                   */
        /*                                                                */
        /******************************************************************/
        two_zero_filter_2( noise, ablas1, ablas2, -1873, 1499 );//noiseb, noisec );

        /******************************************************************/
        /*  Amplitude modulate noise. Reduce noise amplitude during the   */
        /*  second half of the glottal period if "avlin" > 0. "nmod" is   */
        /*  set to 0 if "avlin" is zero, i.e. the glottal source is off.  */
        /******************************************************************/

        if ( nper < nmod )
            noise >>= 1;
        /******************************************************************/
        /*  PERIODIC VOICING WAVEFORM                                     */
        /*  A period has duration of T0 samples, nper is the counter of   */
        /*  current time. During the first nper = 0 to ( T0 - nopen )     */
        /*  samples, the glottis is closed. The glottis opens at time     */
        /*  ( T0 - nopen ), and closes at time T0.                        */
        /******************************************************************/
        for ( nsr4 = 0; nsr4 < 4; nsr4++ ) {
            /****************************************************************/
            /*  Compute next value of voicing wave. Calculate glottal pulse */
            /*  at 4 times normal sample rate to minimize the quantization  */
            /*  of the glottal period.                                      */
            /****************************************************************/
            /****************************************************************/
            /*  voicing has fixed waveshape, at**2 - bt**3                  */
            /****************************************************************/
            if ( nper > ( T0 - nopen )) {
                a -= b;             /*  Fixed waveshape at**2 - bt**3         */
                voice0 += a >> 4;	/*  Differentiated glottal flow.          */
                avlind = avlin;     /*  Delay action of "avlin" change.       */
            } else {
                voice0  = 0;
            }
            /****************************************************************/
            /*  Scale the glottal waveform using the speaker dependant      */
            /*  parameter "avgain".                                         */
            /****************************************************************/
            voice = frac4mul( voice0, avgain );
            /****************************************************************/
            /*  SOME PARAMETERS ARE UPDATED PITCH SYNCHRONOUSLY             */
            /*  (at sample before glottal close time)                       */
            /****************************************************************/
            if ( nper == T0 ) {
                /**************************************************************/
                /*  Reset period when 'nper' reaches T0, glottis about to     */
                /*  open.                                                     */
                /**************************************************************/
                nper = 0;
                /**************************************************************/
                /*  'avlin' moved to 'avlind' after half period.              */
                /**************************************************************/
                avlin = amptable[AVinDB + 4];  /*  Convert from dB to linear. */
                T0 = T0inS4;
                T0 += frac4mul( t0jitr, T0 ); /*  Add jitter, if any.        */
                t0jitr = -t0jitr;     /*  Change sign for alternating jitter. */
                /**************************************************************/
                /*  aturb1 is the Speaker definition breathiness coeficient   */
                /**************************************************************/
                aturb1 = Aturb << 2;
                if ( F1inHZ < 250 )
                    F1inHZ = 250;

                /**************************************************************/
                /*  Use a one pole iir filter to tilt the glottal source.     */
                /**************************************************************/
                switch( uiSampleRateChange ) {
                    case SAMPLE_RATE_INCREASE:
                        decay = 1094 * (int32_t)TILTDB;
                        break;
                    case SAMPLE_RATE_DECREASE:
                        decay = 1073 * (int32_t)TILTDB;
                        break;
                    case NO_SAMPLE_RATE_CHANGE:
                        decay = 1094 * (int32_t)TILTDB;
                        break;
                    default:
                        break;
                }

                if ( decay >= 0 )
                    one_minus_decay = 32767 - decay;
                else
                    one_minus_decay = 32767;
                /**************************************************************/
                /*  Set to "nmod" during the first half of the period.        */
                /*  Modulate the amplitude of the noise if "avlin" > 0, i.e.  */
                /*  the glottal source is enabled.                            */
                /**************************************************************/

                nmod = 0;

                if ( avlin > 0 )
                    nmod = T0 >> 1;
                nopen = frac1mul( k1, T0 ) + k2;
                nopen += ( TILTDB<<2  );

                nopen += ( TILTDB<<2  );      /*  Longer if TILTDB increases */
                if ( nopen < 40 )
                    nopen = 40;        /*  Min is 40                            */
                else
                    if ( nopen > 263 )
                        nopen = 263;     /*  Max is 263                           */

                if ( nopen >= (( T0 * 3 ) >> 2 ))
                    nopen = (( T0 * 3 ) >> 2 );    /*  or 3/4 T0                */

                /**************************************************************/
                /*  Reset a & b, which determine shape of glottal waveform.   */
                /*  Let a = (b * nopen) / 3 without doing the divide.         */
                /**************************************************************/
                b = B0[nopen-40];
                temp = b + 1;

                if ( nopen > 95 ) {
                    temp = (int32_t)temp * nopen;
                    a = frac1mul(10923, temp ); /* Q1.15 -> 0.333 */
                } else {
                    temp = frac1mul(10923, temp ); /* Q1.15 -> 0.333 */
                    a = (int32_t)temp * nopen;
                }
                /**************************************************************/
                /*  Set coeficients of variable cascade resonators.           */
                /**************************************************************/

                R3ca = d2pole_cf123( &r3cb, &r3cc, F3inHZ, B3inHZ, r3cg );
                R2ca = d2pole_cf123( &r2cb, &r2cc, F2inHZ, B2inHZ, r2cg );
                R1ca = d2pole_cf123( &r1cb, &r1cc, F1inHZ, B1inHZ, r1cg );

                /**************************************************************/
                /*  Scale up R1 gain here.                                    */
                /**************************************************************/

                if ( R1ca > 16383 )
                    R1ca = 16383;

                R1ca = R1ca << 1;

                /**************************************************************/
                /*  Set coeficients of nasal zero antiresonator by table      */
                /*  lookup.                                                   */
                /**************************************************************/

                temp = ( FZinHZ >> 3 ) - 31;

                if ( temp > 34 )
                    temp = 34;

                rnza = azero_tab[temp];
                rnzb = bzero_tab[temp];
                rnzc = czero_tab[temp];
            }

            /****************************************************************/
            /*  Downsampling low-pass filter.                               */
            /*  Decimate the glottal pulse from a 40 KHz rate to 10 KHz.    */
            /*  The variable 'rlpd1' is the output of downsampling low-pass */
            /*  filter and is copied back to the variable 'voice'. 'nper'   */
            /*  is the 40 KHz sample counter.                               */
            /****************************************************************/

            two_pole_filter( voice, rlpd1, rlpd2, rlpa, rlpb, rlpc );

            voice = rlpd1;
            nper++;
        }
        /******************************************************************/
        /*  Tilt spectrum down by TILTDB dB at 3 kHz,                     */
        /*  use 1-pole iir filter.                                        */
        /******************************************************************/
        // decay = 1073 * TILTDB; /* fixed decay ebruckert 9-04-2002*/
        voice = frac1mul(one_minus_decay,voice) + frac1mul(decay,vlast);
        vlast = voice;
        /******************************************************************/
        /*  Add breathiness to voicing. Increase noise if Aspiration.     */
        /******************************************************************/
        voice += frac1mul(aturb1, noise);
        /******************************************************************/
        /*  Set variable gain of voicing.                                 */
        /*  24-Jul-85  "avlin" moved to "avlind" after half a period.     */
        /******************************************************************/
        voice = frac4mul( avlind, voice );
        /******************************************************************/
        /*  Add aspiration to voicing.                                    */
        /******************************************************************/
        voice += frac1mul(APlin, noise);

        /******************************************************************/
        /******************************************************************/
        /*                                                                */
        /*  CASCADE VOCAL TRACT, EXCITED BY LARYNGEAL SOURCES             */
        /*                                                                */
        /*  Nasal antiresonator, then formants fnp, f5c, f4c, F3inHZ      */
        /*  F2inHZ, and F1inHZ                                            */
        /*                                                                */
        /******************************************************************/
        /******************************************************************/

        /******************************************************************/
        /*  Nasal Antiresonator of Cascade Vocal Tract:                   */
        /*  rnzout = (rnza * voice) + (rnzb * rnzd1) + (rnzc * rnzd2)     */
        /******************************************************************/
        two_zero_filter( voice, rnzout, rnzd1, rnzd2, rnza, rnzb, rnzc );
        /******************************************************************/
        /*  Nasal Resonator of Cascade Vocal Tract                        */
        /*  output is rnpd1.                                              */
        /******************************************************************/
        two_pole_filter( rnzout, rnpd1, rnpd2, rnpa, rnpb, rnpc );
        /******************************************************************/
        /*  Fifth Formant                                                 */
        /*  output is r5cd1.                                              */
        /******************************************************************/
        // sample rate > 9500
        two_pole_filter( rnpd1, r5cd1, r5cd2, R5ca, R5cb, R5cc );
        /******************************************************************/
        /*  Fourth Formant                                                */
        /*  output is r4cd1.                                              */
        /******************************************************************/
        two_pole_filter( rnpd1, r4cd1, r4cd2, R4ca, R4cb, R4cc );
        /******************************************************************/
        /*  Third Formant                                                 */
        /*  output is r3cd1.                                              */
        /******************************************************************/
        two_pole_filter( r4cd1, r3cd1, r3cd2, R3ca, r3cb, r3cc );
        /******************************************************************/
        /*  Second Formant                                                */
        /*  output is r2cd1.                                              */
        /******************************************************************/
        two_pole_filter( r3cd1, r2cd1, r2cd2, R2ca, r2cb, r2cc );
        /******************************************************************/
        /*  First Formant of Cascade Vocal Tract                          */
        /*  output is r1cd1.                                              */
        /******************************************************************/
        two_pole_filter( r2cd1, r1cd1, r1cd2, R1ca, r1cb, r1cc );
        out = r1cd1;
        /******************************************************************/
        /******************************************************************/
        /*                                                                */
        /*  PARALLEL VOCAL TRACT                                          */
        /*                                                                */
        /*  Excited by frication noise source. Uses formant frequencies   */
        /*  f6p, f5p, f4p, F3inHz, F2inHz and bypass path, outputs added  */
        /*  with alternating sign to prevent overflow.                    */
        /*                                                                */
        /******************************************************************/
        /******************************************************************/

        /******************************************************************/
        /*  Sixth Formant of Parallel Vocal Tract                         */
        /******************************************************************/
        two_pole_filter( noise, r6pd1, r6pd2, r6pa, -5702, -1995 );
        out = r6pd1 - out;

        /******************************************************************/
        /*  Fifth Formant of Parallel Vocal Tract                         */
        /*  Fifth parallel formant effectively out of circuit if the      */
        /*  sample rate is low enough.                                    */
        /******************************************************************/

        // uiSampleRate > 9600
        two_pole_filter( noise, r5pd1, r5pd2, r5pa, R5pb, r5pc );
        out = r5pd1 - out;
        /******************************************************************/
        /*  Fourth Formant of Parallel Vocal Tract                        */
        /******************************************************************/
        two_pole_filter( noise, r4pd1, r4pd2, r4pa, R4pb, r4pc );
        out = r4pd1 - out;
        /******************************************************************/
        /*  Third Formant of Parallel Vocal Tract                         */
        /******************************************************************/
        two_pole_filter( noise, r3pd1, r3pd2, r3pa, r3pb, r3pc );
        out = r3pd1 - out;
        /******************************************************************/
        /*  Second Formant of Parallel Vocal Tract                        */
        /******************************************************************/
        two_pole_filter( noise, r2pd1, r2pd2, r2pa, r2pb, r2pc );
        out = r2pd1 - out;

        about = frac1mul(ABlin, noise);  /*  Output of bypass path      */
        out = about - out;

        if (avlind == 0 && (variabpars[OUT_PH] & PVALUE) == 0 ) {
            rampdown += 4;
            if (rampdown >= 4096)
                rampdown = 4096;
            if (rampdown >= 0)
                out = frac4mul( out,(4096 - rampdown));
        } else
            rampdown=0;


        /******************************************************************/
        /*  Bring the signal level up near +/-32767.                      */
        /*  If the sample rate is 8000 Hz. then the peak signal level is  */
        /*  approximately 8000. This is because of the parameters in file */
        /*  phvdef.c for a sample rate of 8000 Hz.                        */
        /*                                                                */
        /*  Very occasionaly a large impulse will be generated. Clip it.  */
        /*  This allows running the volume level much higher.             */
        /******************************************************************/
        if ( out > 16383 )
            out = 16383;
        else if ( out < -16384 )
            out = -16384;
        iwave[ns] = out << 1; //HELP: left shifting causes clipping despite matching original code, likely missing something
  }
  return;
}

