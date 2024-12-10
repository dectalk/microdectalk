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
 *    File Name:	lsmain.c
 *    Author:
 *    Creation Date:
 *
 *    Functionality:
 * lts entry point stub ...
 *
 ***********************************************************************
 *    Revision History:
 *
 */

#ifdef	SIMULATOR
#define	__far
#define	_far
#define far
#include <stdio.h>
#endif


#include	"lsacna.h"
#include	"lsdef.h"
#include	"lsconst.h"
#ifdef ACNA
extern int namef;
#endif

extern int fc_index;

// extern functions
extern void init_lang(void);
extern int ltsout();
extern int lts(LETTER  *llp, LETTER  *rlp, int def_lang, int sel_lang);
extern int id_name(LETTER *llp, LETTER *rlp);
extern int default_lang(unsigned int lang_code, unsigned int ready_code);

#ifndef SINGLE_THREADED
P_PIPE	linp;
P_PIPE	kinp;
#endif

int lsmain()
{
	init_lang();
	default_lang(LANG_english,LANG_lts_ready);
	default_lang(LANG_english,LANG_both_ready);
	return (0);
}	

int do_lts(LETTER *lp2, LETTER *lp1)
{

	short	sel_lang;
	if(KS.pronflag & PRON_ACNA_NAME)
		{
		sel_lang = id_name(lp2,lp1);
		lts(lp2,lp1,NAME_ENGLISH,sel_lang);

		KS.pronflag &= (~PRON_ACNA_NAME);
		}
	else

		{
		lts(lp2,lp1,0,0);
		}
	ltsout();
	return (0);
}

int is_name(LETTER *lp2, LETTER *lp1) {
	if(KS.pronflag & PRON_ACNA_NAME)
		return(true);
	if((KS.modeflag & MODE_NAME) == 0)
		return(true);
	if((KS.modeflag & MODE_NAME) == 0)
		return(TRUE);
	if(fc_index == 0)
		return(FALSE);
	if((*lp1).l_ch < 64 || (*lp1).l_ch > 97 )/*first character upper??*/
		return(FALSE);
	for(lp1++;lp1<lp2;lp1++)
		if( (*lp1).l_ch  >122 ||  (*lp1).l_ch <97) /*the rest lower*/
			return(FALSE);

	return(FALSE);
}
