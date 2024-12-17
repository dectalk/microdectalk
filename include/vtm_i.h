/*
 ***********************************************************************
 *
 *                           Coryright (c)
 *    © Digital Equipment Corporation 1995. All rights reserved.
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
int16_t iwave[MAXIMUM_FRAME_SIZE];
#else 
int16_t iwave[71];
#endif

/**********************************************************************/
/*  Temporary variables used by in-line functions and by the vocal    */
/*  tract model.                                                      */
/**********************************************************************/

int32_t temp0;
int32_t temp1;

/**********************************************************************/
/*  VARIABLES TO HOLD SPEAKER DEFINITION FROM HOST:                   */
/**********************************************************************/

//int16_t f4c=0;       /*  Frequency of fixed cascade 4th formant             */
//int16_t b4c=0;       /*  Bandwidth of fixed cascade 4th formant             */
//int16_t f5c=0;       /*  Frequency of fixed cascade 5th formant             */
//int16_t b5c=0;       /*  Bandwidth of fixed cascade 5th formant             */
int16_t b2p=0;       /*  Bandwidth of parallel 2th formant                  */
int16_t b3p=0;       /*  Bandwidth of parallel 3th formant                  */
//int16_t f4p=0;       /*  Frequency of fixed parallel 4th formant            */
int16_t b4p=0;       /*  Bandwidth of fixed parallel 4th formant            */
//int16_t f5p=0;       /*  Frequency of fixed parallel 5th formant            */
int16_t b5p=0;       /*  Bandwidth of fixed parallel 5th formant            */
int16_t f6p=0;       /*  Frequency of fixed parallel 6th formant            */
int16_t b6p=0;       /*  Bandwidth of fixed parallel 6th formant            */
//int16_t fnp=0;       /*  Frequency of fixed nasal formant                   */
//int16_t bnp=0;       /*  Bandwidth of fixed nasal formant                   */
int16_t flp=0;       /*  Frequency of fixed glottal decimating filter       */
int16_t blp=0;       /*  Bandwidth of fixed glottal decimating filter       */
int16_t rlpg=0;      /*  Gain of down-sampling low-pass resonator           */
#ifndef MINIMAL_SYNTH
int16_t t0jitr=0;    /*  Alternating jitter for voicing period              */
#endif
int16_t fnscal=0;    /*  Q12 scale factor for F1inHZ, F2inHZ, and F3inHZ    */
int16_t avg=0;       /*  Gain factor for voicing                            */
int16_t abrg=0;      /*  Gain factor for breathiness                        */
int16_t apg=0;       /*  Gain factor for aspiration                         */
int16_t afg=0;       /*  Gain factor for frication                          */
int16_t rnpg=0;      /*  Gain factor for nasal pole resonator               */
int16_t a5gain=0;    /*  Gain in dB for the 5th cascade resonator.          */
int16_t a4gain=0;    /*  Gain in dB for the 4th cascade resonator.          */
int16_t a3gain=0;    /*  Gain in dB for the 3rd cascade resonator.          */
int16_t a2gain=0;    /*  Gain in dB for the 2nd cascade resonator.          */
int16_t a1gain=0;    /*  Gain in dB for the 1st cascade resonator.          */

/**********************************************************************/
/*  CONSTANTS OF SPEAKER DEFINITION                                   */
/*  below are r6pa, r6pb, r6pc, rnpa, rnpb, rnpc, rlpa, rlpb, rlpc    */
/**********************************************************************/

/**********************************************************************/
/*  VARIABLES TO HOLD INPUT PARAMETERS FROM HOST:                     */
/**********************************************************************/

int16_t T0inS4=0;  /*  Voicing fund freq in Hz*10,      500 to 1650         */
int16_t F1inHZ=0;  /*  First formant freq in Hz,        200 to 1300         */
int16_t F2inHZ=0;  /*  Second formant freq in Hz,       550 to 3000         */
int16_t F3inHZ=0;  /*  Third formant freq in Hz,       1200 to 4800         */
int16_t FZinHZ=0;  /*  Nasal zero freq in Hz,           248 to  528         */
int16_t B1inHZ=0;  /*  First formant bw in Hz,           40 to 1000         */
int16_t B2inHZ=0;  /*  Second formant bw in Hz,          40 to 1000         */
int16_t B3inHZ=0;  /*  Third formant bw in Hz,           40 to 1000         */
int16_t AVinDB=0;  /*  Amp of voicing in dB,             0 to   70          */
int16_t APinDB=0;  /*  Amp of aspiration in dB,          0 to   70          */
int16_t A2inDB=0;  /*  Amp of F2 frication in dB,        0 to   80          */
int16_t A3inDB=0;  /*  Amp of F3 frication in dB,        0 to   80          */
int16_t A4inDB=0;  /*  Amp of F4 frication in dB,        0 to   80          */
int16_t A5inDB=0;  /*  Amp of F5 frication in dB,        0 to   80          */
int16_t A6inDB=0;  /*  Amp of F6                         0 to   80          */
int16_t ABinDB=0;  /*  Amp of bypass fric. in dB,        0 to   80          */
int16_t TILTDB=0;  /*  Voicing spectral tilt in dB,      0 to   24          */
int16_t Aturb=0;   /*  Amplitude of breathiness in dB,   0 to   24          */

/**********************************************************************/
/*  INTERNAL MEMORY FOR DIGITAL RESONATORS AND ANTIRESONATOR          */
/**********************************************************************/

int16_t r2pd1=0;    /*  Last output sample from parallel 2nd formant        */
int16_t r2pd2=0;    /*  Second-previous output sample                       */

int16_t r3pd1=0;    /*  Last output sample from parallel 3rd formant        */
int16_t r3pd2=0;    /*  Second-previous output sample                       */

int16_t r4pd1=0;    /*  Last output sample from parallel 4th formant        */
int16_t r4pd2=0;    /*  Second-previous output sample                       */

int16_t r5pd1=0;    /*  Last output sample from parallel 5th formant        */
int16_t r5pd2=0;    /*  Second-previous output sample                       */

int16_t r6pd1=0;    /*  Last output sample from parallel 6th formant        */
int16_t r6pd2=0;    /*  Second-previous output sample                       */

int16_t r1cd1=0;    /*  Last output sample from cascade 1st formant         */
int16_t r1cd2=0;    /*  Second-previous output sample                       */

int16_t r2cd1=0;    /*  Last output sample from cascade 2nd formant         */
int16_t r2cd2=0;    /*  Second-previous output sample                       */

int16_t r3cd1=0;    /*  Last output sample from cascade 3rd formant         */
int16_t r3cd2=0;    /*  Second-previous output sample                       */

int16_t r4cd1=0;    /*  Last output sample from cascade 4th formant         */
int16_t r4cd2=0;    /*  Second-previous output sample                       */

int16_t r5cd1=0;    /*  Last output sample from cascade 5th formant         */
int16_t r5cd2=0;    /*  Second-previous output sample                       */

int16_t rnpd1=0;    /*  Last output sample from cascade nasal pole          */
int16_t rnpd2=0;    /*  Second-previous output sample                       */

int16_t rnzd1=0;    /*  Last output sample from cascade nasal zero          */
int16_t rnzd2=0;    /*  Second-previous output sample                       */

int16_t rlpd1=0;    /*  Last output sample from low-pass filter             */
int16_t rlpd2=0;    /*  Second-previous output sample                       */

int16_t ablas1=0;   /*  Last output sample from nasal anti-resonator        */
int16_t ablas2=0;   /*  Second-previous output sample                       */

int16_t vlast=0;    /*  Last output sample of the tilt filter.              */

/**********************************************************************/
/*  MEMORY VARIABLES FOR ONE-POLE and ONE-ZERO FILTERING              */
/**********************************************************************/

int16_t nolast=0;    /*  Previous value of noisea                           */

/**********************************************************************/
/*  COEFFICIENTS FOR DIGITAL RESONATORS AND ANTIRESONATOR             */
/**********************************************************************/

int16_t r2pa=0;      /*  "a" coef for parallel 2nd formant                  */
int16_t r2pb=0;      /*  "b" coefficient                                    */
int16_t r2pc=0;      /*  "c" coefficient                                    */

int16_t r3pa=0;      /*  "a" coef for parallel 3rd formant                  */
int16_t r3pb=0;      /*  "b" coefficient                                    */
int16_t r3pc=0;      /*  "c" coefficient                                    */

int16_t r4pa=0;      /*  "a" coef for parallel 4th formant                  */
int16_t R4pb=0;      /*  "b" coefficient                                    */
int16_t r4pc=0;      /*  "c" coefficient                                    */

int16_t r5pa=0;      /*  "a" coef for parallel 5th formant                  */
int16_t R5pb=0;      /*  "b" coefficient                                    */
int16_t r5pc=0;      /*  "c" coefficient                                    */

int16_t r6pa=0;      /*  "a" coef for parallel 6th formant                  */
//int16_t r6pb=0;      /*  "b" coefficient                                    */
//int16_t r6pc=0;      /*  "c" coefficient                                    */

int16_t R1ca=0;      /*  "a" coefficient for cascade 1st formant            */
int16_t r1cb=0;      /*  "b" coefficient                                    */
int16_t r1cc=0;      /*  "c" coefficient                                    */

int16_t R2ca=0;      /*  "a" coefficient for cascade 2nd formant            */
int16_t r2cb=0;      /*  "b" coefficient                                    */
int16_t r2cc=0;      /*  "c" coefficient                                    */

int16_t R3ca=0;      /*  "a" coefficient for cascade 3rd formant            */
int16_t r3cb=0;      /*  "b" coefficient                                    */
int16_t r3cc=0;      /*  "c" coefficient                                    */

int16_t R4ca;      /*  "a" coefficient for cascade 4th formant            */
int16_t R4cb=0;      /*  "b" coefficient                                    */
int16_t R4cc=0;      /*  "c" coefficient                                    */

//int16_t R5ca=0;      /*  "a" coefficient for cascade 5th formant            */
int16_t R5cb=0;      /*  "b" coefficient                                    */
int16_t R5cc=0;      /*  "c" coefficient                                    */

int16_t rnpa=0;      /*  "a" coef for nasal pole                            */
int16_t rnpb=0;      /*  "b" coefficient                                    */
int16_t rnpc=0;      /*  "c" coefficient                                    */

int16_t rnza=0;      /*  "a" coefficient for nasal zero                     */
int16_t rnzb=0;      /*  "b" coefficient                                    */
int16_t rnzc=0;      /*  "c" coefficient                                    */

int16_t rlpa=0;      /*  "a" coefficient for low-pass filter                */
int16_t rlpb=0;      /*  "b" coefficient                                    */
int16_t rlpc=0;      /*  "c" coefficient                                    */

//int16_t noiseb=0;    /*  "b" coefficient                                    */
//int16_t noisec=0;    /*  "c" coefficient                                    */

int16_t decay=0;     /*  decay coefficient for tilt low-pass filter         */
int16_t one_minus_decay=0;  /*  tilt filter first order iir filter.         */

/**********************************************************************/
/*  OUTPUT SIGNAL FROM EACH ANTIRESONATOR                             */
/*  (Output of a resonator is r__d1 variable defined above)           */
/**********************************************************************/

int16_t rnzout=0;    /*  Output signal from nasal zero                      */
int16_t about=0;     /*  Output signal from bypass path                     */

/**********************************************************************/
/*  OTHER OUTPUT SIGNALS                                              */
/**********************************************************************/

int16_t noise=0;     /*  Output of random number generator                  */
int16_t voice0=0;    /*  Used for glottal pulse waveform generation         */
int16_t voice=0;     /*  Current sample of voicing waveform                 */

/**********************************************************************/
/*  FIXED AND VARIABLE GAIN FACTORS                                   */
/**********************************************************************/

int16_t avgain=0;    /*  Gain factor for voicing source                     */
int16_t aturb1=0;    /*  Gain factor for breathiness of voice               */
int16_t APgain=0;    /*  Gain factor for aspiration source                  */
int16_t AFgain=0;    /*  Gain factor for frication source                   */
int16_t r3cg=0;      /*  Gain factor for 3rd formant                        */
int16_t r2cg=0;      /*  Gain factor for 2nd formant                        */
int16_t r1cg=0;      /*  Gain factor for 1st formant                        */

int16_t avlin=0;     /*  AVinDB converted to linear ampl. control           */
int16_t avlind=0;    /*  AV after convert from dB to linear, delayed        */
int16_t APlin=0;     /*  AP linear ampl. control                            */
int16_t ABlin=0;     /*  AB linear ampl. control                            */
int16_t r2pg=0;      /*  amp of parameter F2 for iir filter                 */
int16_t r3pg=0;      /*  amp of parameter F3 for iir filter                 */

int16_t a=0;         /*  Makes waveshape of glottal pulse when open         */
int16_t b=0;         /*  Makes waveshape of glottal pulse when open         */

int16_t k1=0;        /*  Equals nopen1 for glottal period duration.         */
int16_t k2=0;        /*  Equals nopen2 for glottal period duration.         */

/**********************************************************************/
/*  COUNTERS                                                          */
/**********************************************************************/

int16_t ns=0;  /*  Number of samples into current frame            */
/* eab was this U32 ns=0;*/  /*  Number of samples into current frame   */
int16_t nper=0;      /*  Current loc in voicing period   40000 samp/s       */
int16_t nsr4=0;      /*  Counter of 4 samples in glottal source loop        */

/**********************************************************************/
/*  COUNTER LIMITS                                                    */
/**********************************************************************/

int16_t T0=0;        /*  Fundamental period in output samples times 4       */
int16_t nopen=0;     /*  Number of samples in open phase of period          */
int16_t nmod=0;      /*  Position in period to begin noise amp. modul       */

/**********************************************************************/
/*  ALL-PURPOSE TEMPORARY VARIABLES                                   */
/**********************************************************************/

int16_t temp=0;
int16_t f=0;         /*  Frequency argument to setb()                       */
int16_t bw=0;        /*  Bandwidth argument to setc()                       */
int16_t gain=0;      /*  Gain term used in seta() calculations              */

/**********************************************************************/
/*  OTHER VARIABLES                                                   */
/**********************************************************************/

int16_t randomx=0;    /*  Varible used by random unmber generator           */
/*int32_t ranmul;*/    /*  Magic constant to make next random number          */
//int16_t ranmul;    /*  Magic constant to make next random number          */
/*int32_t ranadd;*/    /*  Magic constant to make next random number          */
//int16_t  ranadd;    /*  Magic constant to make next random number          */

int16_t out=0;       /*  Output signal from the vocal tract model           */

/* 23-Mar-95 CJL Add to prevent multiple includes.*/
#endif
