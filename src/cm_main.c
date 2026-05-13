/*
 ***********************************************************************
 *
 *                           Copyright ©
 *	  Copyright © 2002 Fonix Corporation. All rights reserved.
 *	  Copyright © 2000, 2001 Force Computers Inc., a Solectron company. All rights reserved.
 *    © Digital Equipment Corporation 1996, 1997. All rights reserved.
 *
 *    Restricted Rights: Use, duplication, or disclosure by the U.S.
 *    Government is subject to restrictions as set forth in subparagraph
 *    (c) (1) (ii) of DFARS 252.227-7013, or in FAR 52.227-19, or in FAR
 *    52.227-14 Alt. III, as applicable.
 *
 *    This software is proprietary to and embodies the confidential
 *    technology of Fonix Corporation and other parties.
 *    Possession, use, or copying of this software and media is authorized
 *    only pursuant to a valid written license from Fonix or an
 *    authorized sublicensor.
 *
 ***********************************************************************
 *    File Name:        cm_main.c
 *    Author:
 *    Creation Date:
 *
 *    Functionality:
 *    Main command driver entry point ... all data is initialized here then we
 *    spawn the command task ...
 *
 ***********************************************************************
 *    Revision History:
 *
 * Rev	Who		Date		Description
 * ---	-----	-----------	--------------------------------------------
 * 0001	tek		12/15/1995	merge dtex; this includes adding the
 *                          undocumented :vs command which only
 *                          worked before because there were no other
 *                          :v commands. Note that the dtex version
 *                          command isn't enabled for the dtpc.
 * 0002	sik		03/22/1996	Re-structured and merged in the win95 code.
 * 0003	SIK		07/08/1996	Cleaning up and maintenance
 * 0004	GL		04/21/1997	BATS#357  Add the code for __osf__ build
 * 0005	GL		04/21/1997	BATS#360  remove spaces before "#define" or "#if"
 * 0006	NS      05/02/1997	Removed form-feed character.  Needed for stripper.
 * 0007	ETT     10/05/1998	added linux code
 * 0008	mfg     10/29/1999	added the FreeCMDThreadMemory declaration for Windows CE
 * 0009	NAL     05/23/2000	Added function prototype (warning removal)
 * 0010	CAB		10/16/2000	Changed copyright info
 * 0011	MGS		05/09/2001	Some VxWorks porting BATS#972
 * 0012	CAB		05/14/2001	Updated copyright info
 * 0013	MFG		05/29/2001	Included dectalkf.h
 * 0014	MGS		06/19/2001	Solaris Port BATS#972
 * 0015	MGS		04/11/2002	ARM7 port
 * 0016	CAB		05/20/2002	Updated copyright info
 * 0017	CAB		07/30/2002	Condense repeated __osf__ code
 */

/*
 *  Main command driver entry point ... all data is initialized here then we
 *  spawn the command task ...
 */

#include "dectalkf.h"
#include "cm_def.h"

#include <stdlib.h>
extern void FreeCMDThreadMemory(PCMD_T);

/* ******************************************************************
 *      Function Name:
 *			#ifdef WIN32_OLD
 *				cmd_main()
 *			#ifdef MSDOS
 *				main()
 *			#ifdef ARM7
 *				cmd_main()
 *			#if defined (__osf__) || defined (__unix__) || defined VXWORKS || defined _SPARC_SOLARIS_
 *				OP_THREAD_ROUTINE()
 *
 *      Description:
 *
 *      Arguments:
 *
 *      Return Value:
 *
 *      Comments:
 *
 * *****************************************************************/
int cmd_main(LPTTS_HANDLE_T phTTS) {
	PCMD_T pCmd_t = 0;
	PKSD_T pKsd_t;

	pKsd_t = phTTS->pKernelShareData;

	if((pCmd_t = (PCMD_T)calloc(1, sizeof(CMD_T))) == NULL)
		printf("Error\n");
	//	        phTTS->uiThreadError = MMSYSERR_NOMEM;
	else
		/***************Thread specific structure initialization MVP ***************/
		if((pCmd_t->cm = (short int*)calloc(total_commands, sizeof(int))) == NULL)
			printf("Error\n");
//				phTTS->uiThreadError = MMSYSERR_NOMEM;
#ifdef ESCAPE_SEQ
		else if((pCmd_t->esc_seq = (INPUT_SEQ*)calloc(1, sizeof(INPUT_SEQ))) == NULL)
			printf("Error\n");
//					phTTS->uiThreadError = MMSYSERR_NOMEM;
#endif

	phTTS->pCMDThreadData = pCmd_t; /* Associate thread specific instance data with corresponding speech object */

	cm_util_initialize(phTTS); /* Initialization routine */

	cm_pars_loop(phTTS);
	// cmd_loop(phTTS, '\0');
	return MMSYSERR_NOERROR;
}
