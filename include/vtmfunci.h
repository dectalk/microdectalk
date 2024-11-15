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
 *    File Name:	vtmfunci.h
 *    Author:
 *    Creation Date:
 *
 *    Functionality:
 *  This include declares functions used by the DECtalk vocal tract
 *  model vtm.c
 *
 ***********************************************************************
 *    Revision History:
 * 0001 23-Mar-95 CJL Change this file from vtmfunc.h to vtmfunci.h,
 *                      add #define to exclude multiple includes.
 */

#ifndef VTMFUNCI_H
#define VTMFUNCI_H

#ifndef USE_FILTER_FUNCTIONS

/**********************************************************************/
/*  Function two_zero_filter.                                         */
/**********************************************************************/

#define  two_zero_filter( tz_input, \
                          tz_output, \
                          tz_delay_1, \
                          tz_delay_2, \
                          tz_a, \
                          tz_b, \
                          tz_c ) \
  temp1 = tz_c * tz_delay_2; \
  temp1 += tz_b * tz_delay_1; \
  temp1 += tz_a * tz_input; \
  tz_delay_2 = tz_delay_1; \
  tz_delay_1 = tz_input; \
  tz_output = ( temp1 >> 12 );

/**********************************************************************/
/*  Function two_zero_filter_2.                                       */
/*  This is a three zero filter, but is implemented as a special case */
/*  because one of the multipliers is 4096 (4096 = 1.0 in Q12 format. */
/*  The input and output variable is "tz_input".                      */
/**********************************************************************/

#define  two_zero_filter_2( tz_input, \
                            tz_delay_1, \
                            tz_delay_2, \
                            tz_b, \
                            tz_c ) \
  temp0 = tz_c * tz_delay_2; \
  temp0 += tz_b * tz_delay_1; \
  tz_delay_2 = tz_delay_1; \
  tz_delay_1 = tz_input; \
  tz_input += ( temp0 >> 12 );

/**********************************************************************/
/*  Function two_pole_filter                                          */
/**********************************************************************/

#define  two_pole_filter( tp_input, \
                          tp_delay_1, \
                          tp_delay_2, \
                          tp_a, \
                          tp_b, \
                          tp_c ) \
  temp1 = tp_c * tp_delay_2; \
  tp_delay_2 = tp_delay_1; \
  temp0 = tp_b * tp_delay_1; \
  temp1 += temp0; \
  temp0 = tp_a * tp_input; \
  temp1 += temp0; \
  tp_delay_1 = ( temp1 >> 12 );

/* 23-MAR-95 CJL Add to exclude multiple includes.*/
#else
void two_pole_filter( short tp_input, short *tp_delay_1, short *tp_delay_2, short tp_a, short tp_b, short tp_c );
void two_zero_filter( short tz_input, short *tz_output, short *tz_delay_1, short *tz_delay_2, short tz_a, short tz_b, short tz_c );
void two_zero_filter_2( short *tz_input, short *tz_delay_1, short *tz_delay_2, short tz_b, short tz_c );
#endif
#endif
