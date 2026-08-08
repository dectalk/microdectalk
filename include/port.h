/************************************************************************
 *
 *                           Copyright ©
 *	  Copyright © 2000-2001 Force Computers Inc., a Solectron company. All rights reserved.
 *    © Digital Equipment Corporation 1995. All rights reserved.
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
 *    File Name:        port.h
 *    Author:
 *    Creation Date:
 *
 *    Functionality:
 *    DECtalk porting specific header file - FOR WINDOWS 95 Code
 *
 ***********************************************************************
 *    Revision History:
 *
 *  Rev Name    Date        Description
 *  --- -----   ----------- --------------------------------------------
 *  001	GL		04/21/1997	BATS#357  Add the code for __osf__ build
 *  002 MGS		09/08/1997	Made change for osf build
 *				11/13/1997	bats423: correct LP_AUDIO_HANDLE_T (aug97)
 *  003 GL 		05/04/1998	Add dectalkf.h here to make sure every file catch
 *                          the setup.
 *  004 ETT		10/05/1998  Added Linux code.
 *  005	MGS		07/14/2000	Sapi 5 additions
 *  006	CAB		01/19/2001	Fixed copyright
 *  007	MGS		02/28/2001	Merged in base changes needed for ACCESS32 merge
 *  008	MGS		05/09/2001	Some VxWorks porting BATS#972
 *  009	MGS		05/18/2001	More VxWorks porting
 *  010	MGS		06/19/2001	Solaris Port BATS#972
 *  011	MGS		04/11/2002	ARM7 port
 *************************************************************************/

#ifndef _PORT_H
#define _PORT_H

#include "dectalkf.h"
#include <stdio.h>

/*
 * fake out some MSDOS stuff
 */
#define __far
#define _far
#define far
#define huge
#define _huge
#define volatile

#include <stdint.h>

typedef uint16_t DT_PIPE_T;
typedef int32_t	 S32;
typedef uint32_t U32;
typedef int16_t	 S16;
typedef uint16_t U16;
typedef int8_t	 S8;
typedef uint8_t	 U8;
typedef uint64_t QWORD;
typedef uint32_t DWORD;
typedef uint16_t USHORT;
typedef void*	 PVOID;
typedef uint32_t UINT;
#define IsBadWritePtr(ptr, size) ((ptr == NULL) ? 1 : 0)
// #define _stricmp strcasecmp
extern int portstricmp(const char* a, const char* b);

#define PRINTFDEBUG_OLD
#if defined(__LP64__) || defined(_WIN64)
typedef uint64_t PTRINT;
#else
typedef uint32_t PTRINT;
#endif

#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#ifndef __BIG_ENDIAN__
#define __BIG_ENDIAN__
#endif
#endif

/* SWAP_dd_eeeeeee */
/* dd is 16 or 32 bits */
/* eeeeee is
   BIG for big-endian numbers
   LITTLE for little-endian numbers */
/* ex on intel SWAP_32_LITTLE(x) does nothing
   but on sparc SWAP_32_LITTLE(x) swaps the bytes */

#ifndef __BIG_ENDIAN__
#define SWAP_16_BIG(x) (((((U16)(x)) & 0xff00) >> 8) | \
			((((U16)(x)) & 0x00ff) << 8))
#define SWAP_16_LITTLE(x) (x)
#define SWAP_32_BIG(x) (((((U32)(x)) & 0xff000000) >> 24) | \
			((((U32)(x)) & 0x00ff0000) >> 8) | \
			((((U32)(x)) & 0x0000ff00) << 8) | \
			((((U32)(x)) & 0x000000ff) << 24))
#define SWAP_32_LITTLE(x) (x)
#else
#define SWAP_16_LITTLE(x) (((((U16)(x)) & 0xff00) >> 8) | \
			   ((((U16)(x)) & 0x00ff) << 8))
#define SWAP_16_BIG(x) (x)
#define SWAP_32_LITTLE(x) (((((U32)(x)) & 0xff000000) >> 24) | \
			   ((((U32)(x)) & 0x00ff0000) >> 8) | \
			   ((((U32)(x)) & 0x0000ff00) << 8) | \
			   ((((U32)(x)) & 0x000000ff) << 24))
#define SWAP_32_BIG(x) (x)
#endif

#endif /* _PORT_H */
