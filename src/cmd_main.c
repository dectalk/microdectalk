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
 *    File Name:	cmd_main.c
 *    Author:
 *    Creation Date:
 *
 *    Functionality:
 *  Main command driver entry point ... all data is initialized here then we
 *  spawn the command task ...
 *
 ***********************************************************************
 *    Revision History:
 * 15dec95	...tek		merge dtex; this includes adding the 
 *				undocumented :vs command which only 
 *				worked before because there were no other
 *				:v commands. Note that the dtex version 
 *				command isn't enabled for the dtpc.
 *
 *
 */


#ifdef	WIN32
#include	"windows.h"
#include	"winbase.h"
#endif

#ifdef SIMULATOR
#include <stdio.h>
#endif

#include        "cmd_def.h"

extern int insertflag;

int     cmd_rate();                                     /* set the speaking rate */
int     cmd_name();                                     /* change the voice */
//int     cmd_latin();                            /*theta to s */
int     cmd_comma();                            /* comma pause time */
int     cmd_period();                           /* period pause time */
//int     cmd_volume();                           /* volume setting */
//int     cmd_mark();                                     /* set index marks in text */
int     cmd_error();                            /* set error mode */
int     cmd_phoneme();                          /* set phonemic mode */
//int     cmd_log();                                      /* set event logging */
int     cmd_mode();                                     /* set text interpretations */
int     cmd_punct();                            /* set punctuation interpretations */
//int     cmd_pause();                            /* pause speech output */
//int     cmd_resume();                           /* resume speech output */
int     cmd_sync();                                     /* sync cmd/lts/ph */
int     cmd_flush();                            /* flush all bufered text and commands */
int     cmd_enable();                           /* selective enable of the flush */

int     cmd_dial();                                     /* generate dial tones */
int     cmd_tone();                                     /* generate user tone */

int     cmd_define();                           /* define custom voices */
int     cmd_say();                                      /* how to break up text */
int     cmd_timeout();                          /* when to flush text */
int     cmd_pronounce();                        /* how to say certain things */
//int     cmd_digitized();                        /* switch to digitized mode */
int     cmd_language();                 /* switch to an alternate language */
//int     cmd_remove();                           /* remove a language from the board */
int     cmd_stress();                           /* set typing table stress */
//int     cmd_break();                            /* word bound pauses */
//int     cmd_cpu_rate();                 /* change default clock rates */
//int     cmd_code_page();                        /* change default code page table */
//int     cmd_plang();                            /* show and speak the phonemic alphabet */
int     cmd_setv();                             /* send a stored set of changes */
int   cmd_loadv();            /* load changes to setv */
//int	cmd_vs();		/* special entry for :vs */
#ifdef DTEX
int	cmd_power();				/* battery and power related stuff */
int	cmd_version();				/* say/return version.*/
int	cmd_tsr();			/* [:tsr modes.. */
#endif /*DTEX*/

extern int vtm_loop(unsigned short *);

int cmd_tone() {
    printf("Tone Command\n");

    unsigned short pipe[6];
    //if ( cm_cmd_sync(phTTS) == CMD_flushing )
    //    return( CMD_flushing );
    pipe[0] = SPC_type_tone;
    pipe[1] = params[1];
    pipe[2] = params[0];

    pipe[3] = 32767; //TONE_AMPLITUDE;
    pipe[4] = 1000;
    pipe[5] = 0;

    vtm_loop(pipe);

    return( CMD_success );
}

#define  DTMF_PAUSE_TIME_IN_MSEC            100
#define  DTMF_DIGIT_TIME_IN_MSEC            100
#define  DTMF_HIGH_TONE_AMPLITUDE         20090
#define  DTMF_LOW_TONE_AMPLITUDE          12676
#define  DTMF_INTER_DIGITAL_TIME_IN_MSEC    100

short   tlitone0[] = 
{                       /* DTMF frequency 2             */
        1336,   /* 0 */
        1209,   /* 1 */
        1336,   /* 2 */
        1477,   /* 3 */
        1209,   /* 4 */
        1336,   /* 5 */
        1477,   /* 6 */
        1209,   /* 7 */
        1336,   /* 8 */
        1477,   /* 9 */
        1209,   /* * */
        1477,   /* # */
        1633,   /* A */
        1633,   /* B */
        1633,   /* C */
        1633    /* D */
};

short   tlitone1[] = 
{                       /* DTMF frequency 1             */
        941,    /* 0 */
        697,    /* 1 */
        697,    /* 2 */
        697,    /* 3 */
        770,    /* 4 */
        770,    /* 5 */
        770,    /* 6 */
        852,    /* 7 */
        852,    /* 8 */
        852,    /* 9 */
        941,    /* * */
        941,    /* A */
        770,    /* B */
        852,    /* C */
        941             /* D */
};

int cmd_dial() {
  int iIndex;
  unsigned char *pChar;
  unsigned char szSingleDigit[2];
  unsigned short pipe[6];

  //if ( cm_cmd_sync(phTTS) == CMD_flushing )
  //      return( CMD_flushing );
  pChar = pString[0];
  while ( *pChar != '\0' ) {
        szSingleDigit[0] = *pChar;
        szSingleDigit[1] = '\0';
        switch ( szSingleDigit[0] ) {
                case '0':
                        iIndex = 0;
                        break;
                case '1':
                        iIndex = 1;
                        break;
                case '2':
                        iIndex = 2;
                        break;
                case '3':
                        iIndex = 3;
                        break;
                case '4':
                        iIndex = 4;
                        break;
                case '5':
                        iIndex = 5;
                        break;
                case '6':
                        iIndex = 6;
                        break;
                case '7':
                        iIndex = 7;
                        break;
                case '8':
                        iIndex = 8;
                        break;
                case '9':
                        iIndex = 9;
                        break;
                case '*':
                        iIndex = 10;
                        break;
                case '#':
                        iIndex = 11;
                        break;
                case 'a':
                case 'A':
                        iIndex = 12;
                        break;
                case 'b':
                case 'B':
                        iIndex = 13;
                case 'c':
                case 'C':
                        iIndex = 14;
                        break;
                case 'd':
                case 'D':
                        iIndex = 15;
                        break;
                default:
                        if  (( szSingleDigit[0] == '-' ) || ( szSingleDigit[0] == ',' ) || ( szSingleDigit[0] == ' ' )) {
                                iIndex = 16;
                        } else {
                                return( CMD_bad_string );
                        }

                        break;
        } /* switch ( szSingleDigit[0] ) */
        if ( iIndex == 16 ) {
          pipe[0] = SPC_type_tone;
          pipe[1] = DTMF_PAUSE_TIME_IN_MSEC;
          pipe[2] = 1000;
          pipe[3] = 0;
          pipe[4] = 1000;
          pipe[5] = 0;
          vtm_loop(pipe);
        } else {
          pipe[0] = SPC_type_tone;
          pipe[1] = DTMF_DIGIT_TIME_IN_MSEC;
          pipe[2] = tlitone0[iIndex];
          pipe[3] = DTMF_HIGH_TONE_AMPLITUDE;
          pipe[4] = tlitone1[iIndex];
          pipe[5] = DTMF_LOW_TONE_AMPLITUDE;
          vtm_loop(pipe);

          pipe[0] = SPC_type_tone;
          pipe[1] = DTMF_INTER_DIGITAL_TIME_IN_MSEC;
          pipe[2] = 1000;
          pipe[3] = 0;
          pipe[4] = 1000;
          pipe[5] = 0;

          vtm_loop(pipe);
        }
        pChar++;
  }

  return( CMD_success );
}


struct  icomm setv[10] =
{
	"\0",
	"\0",
	"\0",
	"\0",
	"\0",
	"\0",
	"\0",
	"\0",
	"\0",
	"\0"
};


struct  dtpc_command command_table[] = {
	{"rate","d",1,DCS_RATE,cmd_rate},
	{"latin","d",1,DCS_LATIN,cmd_name},
	{"name","a",1,DCS_NAME,cmd_name},
		{"np",0,0,DCS_NAME_PAUL,cmd_name},
		{"nb",0,0,DCS_NAME_BETTY,cmd_name},
		{"nh",0,0,DCS_NAME_HARRY,cmd_name},
		{"nf",0,0,DCS_NAME_FRANK,cmd_name},
		{"nd",0,0,DCS_NAME_DENNIS,cmd_name},
		{"nk",0,0,DCS_NAME_THE_KID,cmd_name},
		{"nu",0,0,DCS_NAME_URSULA,cmd_name},
		{"nr",0,0,DCS_NAME_RITA,cmd_name},
		{"nw",0,0,DCS_NAME_WILLY,cmd_name},
		{"nv",0,0,DCS_NAME_VAL,cmd_name},
	{"comma","d",1,DCS_COMMA,cmd_comma},
	{"cp","d",1,DCS_COMMA,cmd_comma},
	{"period","d",1,DCS_PERIOD,cmd_period},
	{"pp","d",1,DCS_PERIOD,cmd_period},
//	{"volume","ad",2,DCS_VOLUME_SET,cmd_volume},
//	{"vs","d",1,DCS_VOLUME_SET,cmd_vs},
	{"error","a",1,DCS_ERROR,cmd_error},
	{"phoneme","aaa",3,DCS_PHONEME,cmd_phoneme},
//	{"log","aa",2,DCS_LOG,cmd_log},
	{"mode","aa",2,DCS_MODE,cmd_mode},
	{"say","a",1,DCS_SAY,cmd_say},
	{"punctuation","a",1,DCS_PUNCT,cmd_punct},
//	{"pause","d",1,DCS_PAUSE,cmd_pause},
//	{"resume","",0,DCS_RESUME,cmd_resume},
	{"sync","",0,DCS_SYNC,cmd_sync},
	{"flush","ad",2,DCS_FLUSH,cmd_flush},
	{"enable","",1,DCS_ENABLE,cmd_enable},
//#ifndef	SIMULATOR
	{"dial","a",1,DCS_DIAL,cmd_dial},
	{"tone","dd",1,DCS_TONE,cmd_tone},
//#endif
//	{"timeout","d",1,DCS_TIMEOUT,cmd_timeout},
	{"pronounce","aa",2,DCS_PRONOUNCE,cmd_pronounce},
//	{"digitized",0,0,DCS_DIGITIZED,cmd_digitized},
//	{"language","a",1,DCS_LANGUAGE,cmd_language},
	{"pitch","d",1,DCS_STRESS,cmd_stress},
	{"define_voice","ad*",2,DCS_DEFINE,cmd_define},
	{"dv","ad*",2,DCS_DEFINE,cmd_define},
//	{"clk_rate","d",1,DCS_CPU_RATE,cmd_cpu_rate},
//	{"code_page","d",1,DCS_CODE_PAGE,cmd_code_page}//,
//	{"plang","d",1,0,cmd_plang},
//	{"break","a",1,DCS_BREAK,cmd_break}//,
//	{"setv","d",1,0,cmd_setv},
//	{"loadv","d",1,0,cmd_loadv}
};

#define         TOTAL_COMMANDS (sizeof(command_table)/sizeof(struct dtpc_command))

unsigned short params[NPARAM];                                    /* array of params built */
unsigned char*  pString[NPARAM];						/* array of pointers to string */
unsigned int defaults[NPARAM];                          /* use default value */
 int param_index;                                               /* current param being worked on */
int format_index;                                               /* current command format specifier */
int     p_count;                                                                                /* chars in this param */
int     p_flag;                                                                         /* negation flag */
int     q_flag;                                                                         /* quote flag */
#ifdef PROTO_BUILD
int     cm[20];                                         /* command match parse array */
#else
int     cm[TOTAL_COMMANDS];                                             /* command match parse array */
#endif
int     total_matches;                                                          /* currently matching buffers */
int     cmd_index;                                                                      /* array index of active command */
unsigned int last_char;                                                 /* last accepted character */
unsigned char string_buff[STRING_MAX];          /* string param array */
int     next_char;                                                                      /* next free character in string buffer */
int     parse_state;                                                            /* current parser state */
int     error_mode;                                                                     /* current error mode */
int     punct_mode;                                                                     /* current punctuation mode */
unsigned int    last_punct;                                             /* last non-strip punctuation */
int     esc_command;                                                            /* command execution via escape code */
//INPUT_SEQ       command_seq,*esc_seq;                   /* input escape sequences */
int     total_commands;                         
int     timeout;                        

#ifdef	SIMULATOR
int cmdmain()
#else
int main()
#endif
{

//#ifdef	SIMULATOR
//	printf ("cmdmain: enter\n");
//#endif

	total_commands = TOTAL_COMMANDS;
//	esc_seq = &command_seq;
//	SP.type = 0;
	reset_command(STATE_NORMAL);
	KS.phoneme_mode = PHONEME_OFF | PHONEME_SPEAK;
	KS.modeflag=MODE_NAME;
	error_mode = ERROR_ignore;
	punct_mode = PUNCT_some;
	last_punct = 0;
	timeout = 0;
//#ifndef	SIMULATOR
//	tone_wait = 0;
//#endif
	KS.pitch_delta = 35;
	insertflag=0;

//#ifndef SINGLE_THREADED
//	create_process(dtpc_cmd,2,data_seg,stack_start,0); /* tek 10/31 */
//#endif

//#ifdef	SIMULATOR
//	printf ("cmdmain: exit\n");
//#endif
	return(1);
}
#endif
