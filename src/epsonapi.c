/* **********************************************************************************
 *                			Copyright                         
 *
 *    Copyright © 2002 Fonix Coporation. All rights reserved.
 *
 *    This is an unpublished work, and is confidential and proprietary: 
 *    technology and information of Fonix corporation.  No part of this
 *    code may be reproduced, used or disclosed without written consent of 
 *    Fonix corporation in each and every instance.
 * ***********************************************************************************
 *  Rev Name    Date        Description
 *  --- -----   ----------- --------------------------------------------
 *	001	CAB		05/15/2002	Added copyright info
 *	002	MGS		04/14/2003	Fix for Epson Bug 5_3_001
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "port.h"
#include "tts.h"
#include "kernel.h"
#include "cm_def.h"
//#include "ph_def.h"
#include "ph_defs.h"
#include "ph_data.h"

extern int cmd_loop(LPTTS_HANDLE_T phTTS,char input);
extern int send_pars_loop(LPTTS_HANDLE_T phTTS);
		
#include "epsonapi.h"

extern int vtm_main(LPTTS_HANDLE_T phTTS);
extern int ph_main(LPTTS_HANDLE_T phTTS);
extern int lts_main(LPTTS_HANDLE_T phTTS);
extern int cmd_main(LPTTS_HANDLE_T phTTS);
extern void SetSampleRate( LPTTS_HANDLE_T phTTS, unsigned int uiSampRate );
//#ifndef EPSON_ARM7
extern short cur_packet_number;
extern short max_packet_number;
//#endif

//#ifndef EPSON_ARM7
TTS_HANDLE_T hTTS;
LPTTS_HANDLE_T phTTS;
KSD_T Ksd_t;
//#endif

typedef unsigned char U8;

char *g_user_dict;
short *(*g_callback)(short *,long, int); // audio, length, phoneme

#define get_long_int(ptr) ((U32)\
                       ((((U8 *)(ptr))[3] << 24)  | \
                        (((U8 *)(ptr))[2] << 16)  | \
                        (((U8 *)(ptr))[1] << 8)  | \
                        (((U8 *)(ptr))[0])))

#if defined ARM7 && !defined WIN32_OLD_TEST
__align(8) 
#endif
extern const unsigned char main_dict[];

#define PC_SAMPLE_RATE 11025
#define MULAW_SAMPLE_RATE 8000

/* ******************************************************************
 *      Function Name: TextToSpeechInit()
 *
 *      Description: 
 *
 *      Arguments:	short *(*callback)(short*,long)
 *					void *user_dict
 *
 *      Return Value: int
 *
 *      Comments:
 *
 * *****************************************************************/

#ifndef NO_FILESYSTEM
extern void init_charset();
#endif

int TextToSpeechInit(short *(*callback)(short *, long, int), void *user_dict) {
	int return_code;
	int i;

#ifndef NO_FILESYSTEM
        init_charset();
#endif

	PKSD_T pKsd_t;
	memset(&hTTS,0,sizeof(TTS_HANDLE_T));
	memset(&Ksd_t,0,sizeof(KSD_T));
	
	phTTS=&hTTS;
	phTTS->EmbCallbackRoutine=callback;
	phTTS->pKernelShareData=&Ksd_t;
	pKsd_t=&Ksd_t;

	g_callback=callback;
	g_user_dict=(char *)user_dict;
	
	for (i=0; i < MAX_languages; ++i)
	{
		pKsd_t->adic_index[i] = 0;
		pKsd_t->udic_index[i] = 0;
		pKsd_t->fdic_index[i] = 0;
		pKsd_t->adic_data[i] = 0;
		pKsd_t->udic_data[i] = 0;
		pKsd_t->fdic_data[i] = 0;
		pKsd_t->adic_bytes[i] = 0;
		pKsd_t->udic_bytes[i] = 0;
		pKsd_t->fdic_bytes[i] = 0;
		pKsd_t->adic_entries[i] = 0;
		pKsd_t->udic_entries[i] = 0;
		pKsd_t->fdic_entries[i] = 0;
		pKsd_t->fdic_fc_entry[i]=0;
		pKsd_t->fdic_fc_entries[i]=0;
	}
	
	/* user dictionary load */
	if (user_dict)
	{
		pKsd_t->udic_entries[0]=get_long_int((char *)user_dict);
		if (pKsd_t->udic_entries[0])
		{
			pKsd_t->udic_bytes[0]=get_long_int((char *)user_dict+4);
			pKsd_t->udic_index[0]=((volatile S32 *)user_dict)+8;
			pKsd_t->udic_data[0]=((char *)user_dict)+(pKsd_t->udic_entries[0]*4)+8;
		}
	}
	
	/* main dictioanry load */
/*
	if (main_dict)
	{
            for (int i = 0; i < MAX_languages; i++) {
		pKsd_t->fdic_entries[0]=get_long_int(main_dict);
		if (pKsd_t->fdic_entries[0])
		{
			pKsd_t->fdic_bytes[i]=get_long_int(main_dict+4);
			pKsd_t->fdic_fc_entries[i]=get_long_int(main_dict+8);
			pKsd_t->fdic_fc_entry[i]=(volatile S32 *) (main_dict+12);
			pKsd_t->fdic_index[i]=(volatile S32 *)(main_dict+12+((pKsd_t->fdic_fc_entries[i])*4));
			pKsd_t->fdic_data[i]=(main_dict+((pKsd_t->fdic_entries[i]+3)*4) + ((pKsd_t->fdic_fc_entries[i])*4));
		}
            }
	}
*/
		
	pKsd_t->cmd_flush = FALSE;
	pKsd_t->spc_flush = FALSE;
	pKsd_t->halting = FALSE;
	pKsd_t->logflag  = 0;
	/* GL 11/15/1996 set defaulted Spanish to Latin */
	pKsd_t->modeflag = MODE_CITATION;
	pKsd_t->sayflag = SAY_CLAUSE;
	pKsd_t->pronflag = 0;
	pKsd_t->wbreak = FALSE;
	pKsd_t->text_flush = FALSE;
	pKsd_t->async_change = FALSE;
	pKsd_t->SamplePeriod = 9.07029478458E-5 ;  /* Initialize sample period */
	pKsd_t->spc_pkt_save = NULL_SPC_PACKET;
	pKsd_t->loaded_languages = NULL;           //MVP:05/10/96 Fixed an exception(When DECtalk runs on a machine without audio card)
	pKsd_t->volume=100; //set volume to 100;
	pKsd_t->vol_att=100; //set volume to 100;

	pKsd_t->phTTS = phTTS;

	phTTS->uiCurrentMsgNumber = 1;
	phTTS->uiLastTextMsgNumber = 0;
	phTTS->uiFlushMsgNumber = 0;
	
	cur_packet_number=0;
	max_packet_number=0;
	
	return_code=vtm_main(phTTS);
	if (return_code)
		return(ERR_ERROR);
	return_code=ph_main(phTTS);
	if (return_code)
		return(ERR_ERROR);
	// load_user_dictionary here...
	return_code=lts_main(phTTS);
	if (return_code)
		return(ERR_ERROR);
	return_code=cmd_main(phTTS);
	if (return_code)
		return(ERR_ERROR);
		
	SetSampleRate( phTTS, PC_SAMPLE_RATE );

	return(ERR_NOERROR);
}

/* ******************************************************************
 *      Function Name: TextToSpeechReset()
 *
 *      Description: 
 *
 *      Arguments:	void
 *
 *      Return Value: int
 *
 *      Comments:
 *
 * *****************************************************************/
int TextToSpeechReset(void)
{
	phTTS->pKernelShareData->halting=1;
	return(ERR_NOERROR);
}

/* ******************************************************************
 *      Function Name: TextToSpeechStart()
 *
 *      Description: 
 *
 *      Arguments:	char *input
 *					short *buffer
 *					int output_format
 *
 *      Return Value: int
 *
 *      Comments:
 *
 * *****************************************************************/
#ifndef NO_FILESYSTEM
extern char *convert_string_for_dapi(char *in, size_t inlen);
#endif

int TextToSpeechStart(char *input, short *buffer_deprecated, int output_format)
{
	int i;
	int oldrate=0;
	int oldspeaker=0;

        // convert string with iconv (or other implementation i guess)
#ifndef NO_FILESYSTEM
	input = convert_string_for_dapi(input, strlen(input));
#endif

	if (phTTS->pKernelShareData->halting)
	{
		oldrate=phTTS->pKernelShareData->uiSampleRate;
		oldspeaker=phTTS->pKernelShareData->last_voice;
		TextToSpeechInit(g_callback,g_user_dict);
		if (oldrate!=phTTS->pKernelShareData->uiSampleRate &&
		    output_format==0)
		{
			SetSampleRate( phTTS, MULAW_SAMPLE_RATE );
		}
		phTTS->pKernelShareData->last_voice=oldspeaker;
	}

	((PDPH_T)(phTTS->pPHThreadData))->loadspdef=1;

	//phTTS->output_buffer=buffer;
        
        //memset(phTTS->pCMDThreadData, 1, sizeof(phTTS->pCMDThreadData));
	phTTS->pKernelShareData->halting=0;
	//cm_cmd_reset_comm(phTTS->pCMDThreadData, STATE_NORMAL);
	
	switch(output_format)
	{
		case WAVE_FORMAT_1M16:
			//memset(buffer,0,71*2);
			if (phTTS->pKernelShareData->uiSampleRate==MULAW_SAMPLE_RATE)
			{
				SetSampleRate( phTTS, PC_SAMPLE_RATE );
			}
			break;
		case WAVE_FORMAT_08M16:
			//memset(buffer,0,51*2);
			if (phTTS->pKernelShareData->uiSampleRate==PC_SAMPLE_RATE)
			{
				SetSampleRate( phTTS, MULAW_SAMPLE_RATE );
			}
			break;
	}

	i=0;
	while (input[i])
	{
#ifdef DEBUG
                printf("Processing Char: %c\n", input[i]);
#endif
		cmd_loop(phTTS,input[i]);
		if (phTTS->pKernelShareData->halting)
		{
			TextToSpeechInit(g_callback,g_user_dict);
			return ERR_RESET;
		}
		i++;
	}
	//cmd_loop(phTTS,0x0B); // force it
	if (phTTS->pKernelShareData->halting)
	{
		cmd_loop(phTTS,0x0B); // force it when halting
		TextToSpeechInit(g_callback,g_user_dict);
		return ERR_RESET;
	}
	return ERR_NOERROR;
}


int TextToSpeechSync() {
    cmd_loop(phTTS,0x0B); // sync command
}
