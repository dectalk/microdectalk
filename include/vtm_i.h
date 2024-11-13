/*
 ***********************************************************************
 *
 *                           Coryright (c)
 *    � Digital Equipment Corporation 1995. All rights reserved.
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
 *    File Name:	vtm_i.h
 *    Author:
 *    Creation Date:
 *
 *    Functionality:
 *  This include declares most of the variables used by the DECtalk
 *  vocal tract model vtm_i.c
 *
 ***********************************************************************
 *    Revision History:
 * 0001 23-Mar-95 CJL Change this file from vtm.h to vtm_i.h, add #define
 *                      to prevent multiple includes. Move UINT, DWORD
 *						MAXIMUM_FRAME_SIZE, SAMPLE_RATE_INCREASE,
 *						SAMPLE_RATE_DECREASE and NO_SAMPLE_RATE_CHANGE
 *						defines to viport.h
 */

#ifndef VTM_I_H
#define VTM_I_H

#include "viport.h"
#include "defs.h"

/**********************************************************************/
/*  Define input parameter array and the audio sample output array    */
/*  for the vocal tract model. The input parameter array is used to   */
/*  store "speaker" and "voice" parameters from function "ph".        */
/**********************************************************************/

//DT_PIPE_T parambuff[33];
#ifndef MATTHELP
S16 iwave[MAXIMUM_FRAME_SIZE];
#else 
S16 iwave[71];
#endif

/**********************************************************************/
/*  Temporary variables used by in-line functions and by the vocal    */
/*  tract model.                                                      */
/**********************************************************************/

S32 temp0;
S32 temp1;

/**********************************************************************/
/*  VARIABLES TO HOLD SPEAKER DEFINITION FROM HOST:                   */
/**********************************************************************/

//S16 f4c=0;       /*  Frequency of fixed cascade 4th formant             */
//S16 b4c=0;       /*  Bandwidth of fixed cascade 4th formant             */
//S16 f5c=0;       /*  Frequency of fixed cascade 5th formant             */
//S16 b5c=0;       /*  Bandwidth of fixed cascade 5th formant             */
S16 b2p=0;       /*  Bandwidth of parallel 2th formant                  */
S16 b3p=0;       /*  Bandwidth of parallel 3th formant                  */
//S16 f4p=0;       /*  Frequency of fixed parallel 4th formant            */
S16 b4p=0;       /*  Bandwidth of fixed parallel 4th formant            */
//S16 f5p=0;       /*  Frequency of fixed parallel 5th formant            */
S16 b5p=0;       /*  Bandwidth of fixed parallel 5th formant            */
S16 f6p=0;       /*  Frequency of fixed parallel 6th formant            */
S16 b6p=0;       /*  Bandwidth of fixed parallel 6th formant            */
//S16 fnp=0;       /*  Frequency of fixed nasal formant                   */
//S16 bnp=0;       /*  Bandwidth of fixed nasal formant                   */
S16 flp=0;       /*  Frequency of fixed glottal decimating filter       */
S16 blp=0;       /*  Bandwidth of fixed glottal decimating filter       */
S16 rlpg=0;      /*  Gain of down-sampling low-pass resonator           */
#ifndef MINIMAL_SYNTH
S16 t0jitr=0;    /*  Alternating jitter for voicing period              */
#endif
S16 fnscal=0;    /*  Q12 scale factor for F1inHZ, F2inHZ, and F3inHZ    */
S16 avg=0;       /*  Gain factor for voicing                            */
S16 abrg=0;      /*  Gain factor for breathiness                        */
S16 apg=0;       /*  Gain factor for aspiration                         */
S16 afg=0;       /*  Gain factor for frication                          */
S16 rnpg=0;      /*  Gain factor for nasal pole resonator               */
S16 a5gain=0;    /*  Gain in dB for the 5th cascade resonator.          */
S16 a4gain=0;    /*  Gain in dB for the 4th cascade resonator.          */
S16 a3gain=0;    /*  Gain in dB for the 3rd cascade resonator.          */
S16 a2gain=0;    /*  Gain in dB for the 2nd cascade resonator.          */
S16 a1gain=0;    /*  Gain in dB for the 1st cascade resonator.          */

/**********************************************************************/
/*  CONSTANTS OF SPEAKER DEFINITION                                   */
/*  below are r6pa, r6pb, r6pc, rnpa, rnpb, rnpc, rlpa, rlpb, rlpc    */
/**********************************************************************/

/**********************************************************************/
/*  VARIABLES TO HOLD INPUT PARAMETERS FROM HOST:                     */
/**********************************************************************/

S16 T0inS4=0;  /*  Voicing fund freq in Hz*10,      500 to 1650         */
S16 F1inHZ=0;  /*  First formant freq in Hz,        200 to 1300         */
S16 F2inHZ=0;  /*  Second formant freq in Hz,       550 to 3000         */
S16 F3inHZ=0;  /*  Third formant freq in Hz,       1200 to 4800         */
S16 FZinHZ=0;  /*  Nasal zero freq in Hz,           248 to  528         */
S16 B1inHZ=0;  /*  First formant bw in Hz,           40 to 1000         */
S16 B2inHZ=0;  /*  Second formant bw in Hz,          40 to 1000         */
S16 B3inHZ=0;  /*  Third formant bw in Hz,           40 to 1000         */
S16 AVinDB=0;  /*  Amp of voicing in dB,             0 to   70          */
S16 APinDB=0;  /*  Amp of aspiration in dB,          0 to   70          */
S16 A2inDB=0;  /*  Amp of F2 frication in dB,        0 to   80          */
S16 A3inDB=0;  /*  Amp of F3 frication in dB,        0 to   80          */
S16 A4inDB=0;  /*  Amp of F4 frication in dB,        0 to   80          */
S16 A5inDB=0;  /*  Amp of F5 frication in dB,        0 to   80          */
S16 A6inDB=0;  /*  Amp of F6                         0 to   80          */
S16 ABinDB=0;  /*  Amp of bypass fric. in dB,        0 to   80          */
S16 TILTDB=0;  /*  Voicing spectral tilt in dB,      0 to   24          */
S16 Aturb=0;   /*  Amplitude of breathiness in dB,   0 to   24          */

/**********************************************************************/
/*  INTERNAL MEMORY FOR DIGITAL RESONATORS AND ANTIRESONATOR          */
/**********************************************************************/

S16 r2pd1=0;    /*  Last output sample from parallel 2nd formant        */
S16 r2pd2=0;    /*  Second-previous output sample                       */

S16 r3pd1=0;    /*  Last output sample from parallel 3rd formant        */
S16 r3pd2=0;    /*  Second-previous output sample                       */

S16 r4pd1=0;    /*  Last output sample from parallel 4th formant        */
S16 r4pd2=0;    /*  Second-previous output sample                       */

S16 r5pd1=0;    /*  Last output sample from parallel 5th formant        */
S16 r5pd2=0;    /*  Second-previous output sample                       */

S16 r6pd1=0;    /*  Last output sample from parallel 6th formant        */
S16 r6pd2=0;    /*  Second-previous output sample                       */

S16 r1cd1=0;    /*  Last output sample from cascade 1st formant         */
S16 r1cd2=0;    /*  Second-previous output sample                       */

S16 r2cd1=0;    /*  Last output sample from cascade 2nd formant         */
S16 r2cd2=0;    /*  Second-previous output sample                       */

S16 r3cd1=0;    /*  Last output sample from cascade 3rd formant         */
S16 r3cd2=0;    /*  Second-previous output sample                       */

S16 r4cd1=0;    /*  Last output sample from cascade 4th formant         */
S16 r4cd2=0;    /*  Second-previous output sample                       */

S16 r5cd1=0;    /*  Last output sample from cascade 5th formant         */
S16 r5cd2=0;    /*  Second-previous output sample                       */

S16 rnpd1=0;    /*  Last output sample from cascade nasal pole          */
S16 rnpd2=0;    /*  Second-previous output sample                       */

S16 rnzd1=0;    /*  Last output sample from cascade nasal zero          */
S16 rnzd2=0;    /*  Second-previous output sample                       */

S16 rlpd1=0;    /*  Last output sample from low-pass filter             */
S16 rlpd2=0;    /*  Second-previous output sample                       */

S16 ablas1=0;   /*  Last output sample from nasal anti-resonator        */
S16 ablas2=0;   /*  Second-previous output sample                       */

S16 vlast=0;    /*  Last output sample of the tilt filter.              */

/**********************************************************************/
/*  MEMORY VARIABLES FOR ONE-POLE and ONE-ZERO FILTERING              */
/**********************************************************************/

S16 nolast=0;    /*  Previous value of noisea                           */

/**********************************************************************/
/*  COEFFICIENTS FOR DIGITAL RESONATORS AND ANTIRESONATOR             */
/**********************************************************************/

S16 r2pa=0;      /*  "a" coef for parallel 2nd formant                  */
S16 r2pb=0;      /*  "b" coefficient                                    */
S16 r2pc=0;      /*  "c" coefficient                                    */

S16 r3pa=0;      /*  "a" coef for parallel 3rd formant                  */
S16 r3pb=0;      /*  "b" coefficient                                    */
S16 r3pc=0;      /*  "c" coefficient                                    */

S16 r4pa=0;      /*  "a" coef for parallel 4th formant                  */
S16 R4pb=0;      /*  "b" coefficient                                    */
S16 r4pc=0;      /*  "c" coefficient                                    */

S16 r5pa=0;      /*  "a" coef for parallel 5th formant                  */
S16 R5pb=0;      /*  "b" coefficient                                    */
S16 r5pc=0;      /*  "c" coefficient                                    */

S16 r6pa=0;      /*  "a" coef for parallel 6th formant                  */
//S16 r6pb=0;      /*  "b" coefficient                                    */
//S16 r6pc=0;      /*  "c" coefficient                                    */

S16 R1ca=0;      /*  "a" coefficient for cascade 1st formant            */
S16 r1cb=0;      /*  "b" coefficient                                    */
S16 r1cc=0;      /*  "c" coefficient                                    */

S16 R2ca=0;      /*  "a" coefficient for cascade 2nd formant            */
S16 r2cb=0;      /*  "b" coefficient                                    */
S16 r2cc=0;      /*  "c" coefficient                                    */

S16 R3ca=0;      /*  "a" coefficient for cascade 3rd formant            */
S16 r3cb=0;      /*  "b" coefficient                                    */
S16 r3cc=0;      /*  "c" coefficient                                    */

S16 R4ca;      /*  "a" coefficient for cascade 4th formant            */
S16 R4cb=0;      /*  "b" coefficient                                    */
S16 R4cc=0;      /*  "c" coefficient                                    */

//S16 R5ca=0;      /*  "a" coefficient for cascade 5th formant            */
S16 R5cb=0;      /*  "b" coefficient                                    */
S16 R5cc=0;      /*  "c" coefficient                                    */

S16 rnpa=0;      /*  "a" coef for nasal pole                            */
S16 rnpb=0;      /*  "b" coefficient                                    */
S16 rnpc=0;      /*  "c" coefficient                                    */

S16 rnza=0;      /*  "a" coefficient for nasal zero                     */
S16 rnzb=0;      /*  "b" coefficient                                    */
S16 rnzc=0;      /*  "c" coefficient                                    */

S16 rlpa=0;      /*  "a" coefficient for low-pass filter                */
S16 rlpb=0;      /*  "b" coefficient                                    */
S16 rlpc=0;      /*  "c" coefficient                                    */

//S16 noiseb=0;    /*  "b" coefficient                                    */
//S16 noisec=0;    /*  "c" coefficient                                    */

S16 decay=0;     /*  decay coefficient for tilt low-pass filter         */
S16 one_minus_decay=0;  /*  tilt filter first order iir filter.         */

/**********************************************************************/
/*  OUTPUT SIGNAL FROM EACH ANTIRESONATOR                             */
/*  (Output of a resonator is r__d1 variable defined above)           */
/**********************************************************************/

S16 rnzout=0;    /*  Output signal from nasal zero                      */
S16 about=0;     /*  Output signal from bypass path                     */

/**********************************************************************/
/*  OTHER OUTPUT SIGNALS                                              */
/**********************************************************************/

S16 noise=0;     /*  Output of random number generator                  */
S16 voice0=0;    /*  Used for glottal pulse waveform generation         */
S16 voice=0;     /*  Current sample of voicing waveform                 */

/**********************************************************************/
/*  FIXED AND VARIABLE GAIN FACTORS                                   */
/**********************************************************************/

S16 avgain=0;    /*  Gain factor for voicing source                     */
S16 aturb1=0;    /*  Gain factor for breathiness of voice               */
S16 APgain=0;    /*  Gain factor for aspiration source                  */
S16 AFgain=0;    /*  Gain factor for frication source                   */
S16 r3cg=0;      /*  Gain factor for 3rd formant                        */
S16 r2cg=0;      /*  Gain factor for 2nd formant                        */
S16 r1cg=0;      /*  Gain factor for 1st formant                        */

S16 avlin=0;     /*  AVinDB converted to linear ampl. control           */
S16 avlind=0;    /*  AV after convert from dB to linear, delayed        */
S16 APlin=0;     /*  AP linear ampl. control                            */
S16 ABlin=0;     /*  AB linear ampl. control                            */
S16 r2pg=0;      /*  amp of parameter F2 for iir filter                 */
S16 r3pg=0;      /*  amp of parameter F3 for iir filter                 */

S16 a=0;         /*  Makes waveshape of glottal pulse when open         */
S16 b=0;         /*  Makes waveshape of glottal pulse when open         */

S16 k1=0;        /*  Equals nopen1 for glottal period duration.         */
S16 k2=0;        /*  Equals nopen2 for glottal period duration.         */

/**********************************************************************/
/*  COUNTERS                                                          */
/**********************************************************************/

S16 ns=0;  /*  Number of samples into current frame            */
/* eab was this U32 ns=0;*/  /*  Number of samples into current frame   */
S16 nper=0;      /*  Current loc in voicing period   40000 samp/s       */
S16 nsr4=0;      /*  Counter of 4 samples in glottal source loop        */

/**********************************************************************/
/*  COUNTER LIMITS                                                    */
/**********************************************************************/

S16 T0=0;        /*  Fundamental period in output samples times 4       */
S16 nopen=0;     /*  Number of samples in open phase of period          */
S16 nmod=0;      /*  Position in period to begin noise amp. modul       */

/**********************************************************************/
/*  ALL-PURPOSE TEMPORARY VARIABLES                                   */
/**********************************************************************/

S16 temp=0;
S16 f=0;         /*  Frequency argument to setb()                       */
S16 bw=0;        /*  Bandwidth argument to setc()                       */
S16 gain=0;      /*  Gain term used in seta() calculations              */

/**********************************************************************/
/*  OTHER VARIABLES                                                   */
/**********************************************************************/

S16 randomx=0;    /*  Varible used by random unmber generator           */
/*S32 ranmul;*/    /*  Magic constant to make next random number          */
//S16 ranmul;    /*  Magic constant to make next random number          */
/*S32 ranadd;*/    /*  Magic constant to make next random number          */
//S16  ranadd;    /*  Magic constant to make next random number          */

S16 out=0;       /*  Output signal from the vocal tract model           */

/* 23-Mar-95 CJL Add to prevent multiple includes.*/
#endif
