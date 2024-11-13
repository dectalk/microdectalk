#ifndef NO_CMD
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
 *    File Name:	cmd_stre.c
 *    Author:
 *    Creation Date:
 *
 *    Functionality:
 *  pcdt typing table character substitution ...
 *
 ***********************************************************************
 *    Revision History:
 *
 */

#include	"cmd_def.h"
#include	"eng_phon.h"

int cmd_stress()
{

	if(defaults[0] == true)
		KS.pitch_delta = 0;
	else
		KS.pitch_delta = (int)params[0];
	return(CMD_success);
}

#endif
