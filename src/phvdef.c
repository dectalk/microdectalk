/*
 ***********************************************************************
 *
 *                           Coryright (c)
 *    � Digital Equipment Corporation 1995. All rights reserved.
 *      Copyright       1984                    by Dennis H. Klatt
 *      Copyright       1984                    Digital Equipment Corp.
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
 *    File Name:	phvdef.c
 *    Author:
 *    Creation Date:
 *
 *    Functionality:
 *      Speaker defs for:
 *              1. PAUL
 *              2. BETTY
 *              3. HARRY
 *              4. FRANK
 *              5. DENNIS
 *              6. KIT
 *              7. URSULA
 *              8. RITA
 *              9. WENDY
 *             10. VAL
 *
 *      Cur speaker def does not really change until loadspdef set to TRUE
 *
 *      Thus one can load a new speaker into curspdef[] at any time
 *      without ever really changing the output voice until PHCLAUSE()
 *      is called with a new input phonemic clause.
 *
 ***********************************************************************
 *    Revision History:
 * 0001  6-Dec-84 DK    Adopted from DECtalk klvdef.c
 * 0002 27-Dec-84 DGC   Added the "readonly" things for the 80186.
 * 0003 14-Jan-85 DK    Fine tune voices to new tilt(f0) function
 * 0004 21-Jan-85 DK    Increase richness of Paul (less base, more like old Paul)
 * 0005 22-Apr-85 DK    Fix Betty to better match Diane Br.
 * 0006 30-Apr-85 DK    Overload calibration for 3.0 for all voices
 * 0007  3-Jun-85 DK    Increase SR for more lively voices
 * 0008 10-Jun-85 DK    Paul: ap down, Betty: ap up, Dennis: less breathy
 * 0009 20-Jun-85 DK    Betty: ap back down a bit
 * 0010 24-Jul-85 DK    Boost LO so don't have to shift R1c output
 * 0011 29-Jul-85 DK    Adjust spdef levels to avoid overloads during aspiration
 * 0012 26-Aug-85 DK    Ursula still overloads, back off gains
 * 0013 06-Sep-85 DGC   Harry head size to 115, from 120.
 * 0014 07-jan-91 EAB   re-evaluated and adjusted gains for all voices for 
 *                      DECtalk-PC release
 * 0015 03-may-93 eab   Changed betty's values to match 88 code except for new
 * 0016 19-Oct-93 WIH   Added definitions for 8 kHz. sample rate.
 * 0017 22-Mar-95 EAB   Merged VTM definitions from Alpha group.
 * 0018 16-JAN-96 SIK Removed white space inbetween # and include in places
 * 					  where #include is commented out. 
 * 0019 17-JUL-96 KB    The breathiness parameters for Wendy, Kit, Denise and Frank
 *						had been changed (decremented).  When this release shipped
 *						many customers complained.  The parameters were changed back
 *						so they are as breathy as they have always been.
 *
 */

#ifdef __osf__
/*  eab 3/22/95 removed because we don't have this osf file set yet*/
/* 0018 16-JAN-96 SIK Removed white space inbetween # and include in places
 * 					  where #include is commented out. */
/* #include "dtmmedefs.h"*/
#endif
#include "phdefs.h"
/*  eab 3/22/95 removed because we don't have this osf file set yet*/
/*#include "ttsapi.h"*/


#define i386

/**********************************************************************/
/**********************************************************************/
/* SPEAKER DEFINITIONS FOR 8 kHz.                                     */
/**********************************************************************/
/**********************************************************************/

/*
 * Actually means that we are running an integer vocal tract model.
 * In the MSVC++ environment, we define it on the compiler option
 * line as /D i386
 */
	
/* Perfect Paul */
 short paul_8[SPDEF] = {
	MALE,   /* SEX = m */
	   3,   /* SM (smoothness in %, actually spectral tilt offset) */
	  100,  /* AS (assertiveness, degree of final f0 fall in % (Was 100) */
	 122,   /* AP (Average pitch in Hz) */
	 105,   /* PR (pitch range in percent of Paul's range) */
	   0,   /* BR (breathiness in dB) */
	  70,   /* RI (was 45,Richness in %, actually nopen is 100-RI % of T0) */
	   0,   /* NF (additional fixed number of samples in nopen) */
	   0,   /* LA (laryngealization, in percent) */
	 100,   /* HS (head size, in percent relative to normal for SEX) */
	3300,   /* F4 (was 3350, frequency in Hz of cascade 4th formant = F4*100/HS) */
	 260,   /* B4 (was 230, bandwidth in Hz of cascade 4th formant) */
	3650,   /* F5 (was 3900, frequency in Hz of cascade 5th formant = F5*100/HS) */
	 330,   /* B5 (was 180, bandwidth in Hz of cascaded 5th formant) */
	3350,   /* F7 (frequency in Hz of parallel 4th formant = F7) */
	3850,   /* F8 (frequency in Hz of parallel 5th formant = F8) */
	  67, /* GF (gain of frication source in dB) */
	  65,   /* GH (gain of aspiration source in dB) */
	  65, /* GV (gain of voicing source in dB ) */
	  70, /* GN (gain of input to cascade nasal pole pair in dB) */
	  66,   /* G1 (gain of input to cascade 5th formant in dB) */
	  59+10,   /* G2 (gain of input to cascade 4th formant in dB) */
	  54-2, /* G3 (gain of input to cascade 3rd formant in dB) */
	  65,   /* G4 (gain of input to cascade 2nd formant in dB) */
	  70+6,   /* LO (Loudness, gain input to cascade 1st formant in dB) */
	  75,   /* FT (f0-dependent spectral tilt in % of max)frm 75 to 90 for 10to8     */
	  12,   /* BF (baseline f0 fall in Hz) */
	   0,   /* LX (lax folds adjacent to voiceless sound -> breathiness) */
	  40,   /* QU (quickness of larynx gestures in % of max quickness) */
	  22,   /* HR (hat-pattern fundamental frequency rise in Hz, was 18) */
	  32,   /* SR (height of max stress-rise impulse of f0 in Hz, was 32) */
/*          PAUL */
};

/**********************************************************************/
/**********************************************************************/
/* SPEAKER DEFINITIONS FOR 10 khz and 11 kHz.                         */
/**********************************************************************/
/**********************************************************************/

/* Perfect Paul */
const short paul[SPDEF] = {
	MALE,   /* SEX = m */
	   3,   /* SM (smoothness in %, actually spectral tilt offset) */
	 100,   /* AS (assertiveness, degree of final f0 fall in % */
	 122,   /* AP (Average pitch in Hz) */
	 100,   /* PR (pitch range in percent of Paul's range) */
	   0,   /* BR (breathiness in dB) */
	  70,   /* RI (was 45,Richness in %, actually nopen is 100-RI % of T0) */
	   0,   /* NF (additional fixed number of samples in nopen) */
	   0,   /* LA (laryngealization, in percent) */
	 100,   /* HS (head size, in percent relative to normal for SEX) */
	3300,   /* F4 (was 3350, frequency in Hz of cascade 4th formant = F4*100/HS) */
	 260,   /* B4 (was 230, bandwidth in Hz of cascade 4th formant) */
	3650,   /* F5 (was 3900, frequency in Hz of cascade 5th formant = F5*100/HS) */
	 330,   /* B5 (was 180, bandwidth in Hz of cascaded 5th formant) */
	3350,   /* F7 (frequency in Hz of parallel 4th formant = F7) */
	3850,   /* F8 (frequency in Hz of parallel 5th formant = F8) */
	  70,   /* GF (gain of frication source in dB) */
	  70,   /* GH (gain of aspiration source in dB) */
	  65,   /* GV (gain of voicing source in dB ) */
	  74,   /* GN (gain of input to cascade nasal pole pair in dB) */
	  68,   /* G1 (gain of input to cascade 5th formant in dB) */
	  60,   /* G2 (gain of input to cascade 4th formant in dB) */
	  48,   /* G3 (gain of input to cascade 3rd formant in dB) */
	  64,   /* G4 (gain of input to cascade 2nd formant in dB) */
	  86,   /* LO (Loudness, gain input to cascade 1st formant in dB) */
	  75,   /* FT (f0-dependent spectral tilt in % of max) */
	  18,   /* BF (baseline f0 fall in Hz) */
	   0,   /* LX (lax folds adjacent to voiceless sound -> breathiness) */
	  40,   /* QU (quickness of larynx gestures in % of max quickness) */
	  18,   /* HR (hat-pattern fundamental frequency rise in Hz) */
	  32,   /* SR (height of max stress-rise impulse of f0 in Hz, was 32) */
/*          PAUL  */
};

extern short    var_val[SPDEF];

/*
 * This table, indexed by the
 * speaker definition code (the "SP_" symbols),
 * is used to range check user specified voice
 * definition ":dv" commands.
 */
 LIMIT  limitTable[] = {
	0,      1,                              /* SEX                  */
	0,      100,                            /* SM                   */
	0,      100,                            /* AS                   */
	50,     350,                            /* AP                   */
	0,      250,                            /* PR                   */
	0,      72,                             /* BR                   */
	0,      100,                            /* RI                   */
	0,      100,                            /* NF                   */
	0,      100,                            /* LA                   */
	65,     145,                            /* HS                   */
	2000,   4650,                           /* F4                   */
	100,    ZAPB,                           /* B4                   */
	2500,   4950,                           /* F5                   */
	100,    ZAPB,                           /* B5                   */
	2500,   4950,                           /* P4                   */
	2500,   4950,                           /* P5                   */
	0,      86,                             /* GF                   */
	0,      86,                             /* GH                   */
	0,      86,                             /* GV                   */
	0,      86,                             /* GN                   */
	0,      86,                             /* G1                   */
	0,      86,                             /* G2                   */
	0,      86,                             /* G3                   */
	0,      86,                             /* G4                   */
	0,      86,                             /* LO                   */
	0,      100,                            /* FT                   */
	0,      40,                             /* BF                   */
	0,      100,                            /* LX                   */
	0,      100,                            /* QU                   */
	2,      100,                            /* HR                   */
	1,      100                             /* SR                   */
};