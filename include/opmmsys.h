/*
 ***********************************************************************
 *
 *                           Copyright (c)
 *   Copyright (c) 2001 Force Computers Inc. A Solectron company. All rights reserved.
 *    Digital Equipment Corporation 1996, 1997. All rights reserved.
 *
 *    Restricted Rights: Use, duplication, or disclosure by the U.S.
 *    Government is subject to restrictions as set forth in subparagraph
 *    (c) (1) (ii) of DFARS 252.227-7013, or in FAR 52.227-19, or in FAR
 *    52.227-14 Alt. III, as applicable.
 *
 *    This software is proprietary to and embodies the confidential
 *    technology of Force Computers Incorporated and other parties.
 *    Possession, use, or copying of this software and media is authorized
 *    only pursuant to a valid written license from Force or an
 *    authorized sublicensor.
 *
 ***********************************************************************
 *    File Name:        opmmsys.h
 *    Author:			Bill Hallahan
 *    Creation Date:	12/26/95
 *
 *    Functionality:
 *
 ***********************************************************************
 *    Revision History:
 *
 * Rev	Who		Date		Description
 * ---  ------	-----------	--------------------------------------------
 *
 * 001	?		04/22/1996	?
 * 002	TQL		05/21/1997	BATS#357  Add the code for __osf__ build
 * 003  ETT		10/05/1998  Added Linux code.
 * 004	MGS		05/09/2001	Some VxWorks porting BATS#972
 * 005	CAB		05/14/2001	Added force copyright
 * 006	MGS		06/19/2001		Solaris Port BATS#972
 *
 */

#ifndef _OPMMSYS_H_
#define _OPMMSYS_H_

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************/
/*  Multimedia include files and type definitions for Digital UNIX.   */
/**********************************************************************/

#if (__WORDSIZE == 64)
#define OS_SIXTY_FOUR_BIT
#endif

#ifdef _LP64
#ifndef OS_SIXTY_FOUR_BIT
#define OS_SIXTY_FOUR_BIT
#endif
#endif

#ifdef _WIN64
#ifndef OS_SIXTY_FOUR_BIT
#define OS_SIXTY_FOUR_BIT
#endif
#endif

/**********************************************************************/
/*  Define the audio buffer type. This is currently the same for all  */
/*  Operating Systems and Platforms we support.                       */
/**********************************************************************/

typedef unsigned char AUDIO_T;
typedef AUDIO_T*      LPAUDIO_T;

#ifdef __cplusplus
} /* End extern "C" */
#endif

#endif
