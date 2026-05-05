/*
 ***********************************************************************
 *
 *                           Copyright ©
 *    Copyright © 2000, 2001 Force Computers, Inc., a Solectron company. All rights reserved.
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
 *    File Name:    ph_log.c
 *    Author:
 *    Creation Date:
 *
 *    Functionality:
 *  all ph logging functions have been moved to here lib and now log
 *  based on the loaded phonemic set ...
 *
 ***********************************************************************
 *    Revision History:
 *
 *  Rev Name    Date        Description
 *  --- -----   ----------- --------------------------------------------
 *  001 MGS     03/25/1996	Merged WIN 95 code to 42c
 *  002	MGS		06/06/1996	Changed file name from phlog.c ph_log.c
 *  003 GL		10/03/1996  Add many logging hook for WIN95
 *							add make sure LATIN mode get the correct logging for Spanish
 *  004 GL		03/21/1997  for BATS#305
 *							need to send the space for single phoneme character logging
 *							for WIN32_OLD
 *  005 GL		03/24/1997  for BATS#313
 *							the ascky logging should no have the spaces.
 *  006	GL		04/21/1997	BATS#357  Add the code for __osf__ build
 *  007 GL		11/06/1997	BATS#486 change display to 72 character from 35
 *  008	MFG		05/08/1998	added dbglog.txt logging for debug switch[:debug 2008]
 *  009 MFG		05/12/1998	cleanup of dbglog.txt logging
 *  010 MFG		05/19/1998	excluded dbglog logging when build 16-bit code (MSDOS)
 *  011 ETT		10/05/1998	Added Linux code.
 *  012	MGS		04/13/2000	Changes for integrated phoneme set
 *  013	CHJ		07/20/2000	Added French
 *  014 CAB		10/18/2000	Changed copyright info and formatted comments
 *  015	MGS		11/06/2000	Changes all foreigh langauge phonemes to print with lower case letters
 *  016	MGS		05/09/2001	Some VxWorks porting BATS#972
 *  017 CAB		05/14/2001	Updated copyright
 *  018 MFG		05/29/2001	Included dectalkf.h
 *  019	MGS		06/19/2001	Solaris Port BATS#972
 *  020	MGS		04/11/2002	ARM7 port
 */

#include "dectalkf.h"
#include "port.h"

#include "tts.h"

#include "defs.h"
#include "dectalk.h"
#include "kernel.h"
#include "cmd.h"
#include "esc.h"
#include "p_all_ph.h"
#include "pcport.h"
#include "186.h"

extern unsigned char* arpabet_arrays[];
extern unsigned int   arpabet_sizes[];
extern unsigned int   arpabet_lang_flags[];
extern unsigned int   arpabet_lang_fonts[];

extern unsigned char usa_arpa[];
extern unsigned char spanish_arpa[];
extern unsigned char la_arpa[];
extern unsigned char german_arpa[];
extern unsigned char uk_arpa[];
extern unsigned char french_arpa[];

void dologphoneme(LPTTS_HANDLE_T phTTS, short phone, short dur, short f0);

unsigned char _far* PrintLangBit(LPTTS_HANDLE_T phTTS, short);

/*
 * Phoneme names, for logging.
 */

/*
 * Table of "SP" names,
 */

char* logspnames[] =
    {
	"sex",
	"sm",
	"as",
	"ap",
	"pr",
	"br",
	"ri",
	"nf",
	"la",
	"hs",
	"f4",
	"b4",
	"f5",
	"b5",
	"p4",
	"p5",
	"gf",
	"gh",
	"gv",
	"gn",
	"g1",
	"g2",
	"g3",
	"g4",
	"lo", /* Was "g5".            */
	"ft",
	"bf",
	"lx", /* Was "ef", now gone.  */
	"qu",
	"hr",
	"sr"};

/*
 * This routine is called after a clause has been
 * read, and the LOG_HOSTPH flag is set. It runs down the
 * "symbols", "user_durs", and "user_f0" arrays, and writes out
 * the phonemes it finds. A carriage return is appended to the
 * clause; this can be used as a signal that the end of
 * the clause has been reached.
 */

void logclaus(LPTTS_HANDLE_T phTTS,
	      short*	     sym,
	      short	     nsym,
	      short*	     ud,
	      short*	     uf0) {
	register int		i, cnt;
	struct spc_packet _far* spc_pkt;
	PKSD_T			pKsd_t;

	pKsd_t = phTTS->pKernelShareData;

	cnt = 0;

	for(i = 1; i < nsym; ++i) {
		cnt++;

		if((spc_pkt = (struct spc_packet _far*)pKsd_t->spc_pkt_save) != NULL_SPC_PACKET) {
			while(spc_pkt != NULL_SPC_PACKET) {
				cnt += 6;
				if((*spc_pkt).data[0] == (U16)i) {
					if(pKsd_t->logflag & LOG_PHONEMES) {
						if(fprintf(phTTS->pLogFile, "[:i %d]", (*spc_pkt).data[2]) < 0) {
							// TextToSpeechErrorHandler (phTTS,
							//						  ERROR_WRITING_FILE,
							//						  0L);
						}
					}
				}
				spc_pkt = (struct spc_packet _far*)(*spc_pkt).link;
			}
		}

		dologphoneme(phTTS, sym[i], ud[i], uf0[i]);
		/* GL 11/06/1997 BATS#486 change display to 72 character from 35 */
		if(cnt >= 72) {
#ifdef PRINTFDEBUG_OLD
			printf("\n");
#endif
			if(pKsd_t->logflag & LOG_PHONEMES) {
				if(fprintf(phTTS->pLogFile, "\n") < 0) {
					// TextToSpeechErrorHandler (phTTS,
					//						  ERROR_WRITING_FILE,
					//						  0L);
				}
			}
			cnt = 0;
		}
	}

#ifdef PRINTFDEBUG_OLD
	printf("\n");
#endif
	if(pKsd_t->logflag & LOG_PHONEMES) {
		if(fprintf(phTTS->pLogFile, "\n") < 0) {
			// TextToSpeechErrorHandler (phTTS,
			//						  ERROR_WRITING_FILE,
			//						  0L);
		}
	}

	return;
}

/*
 * Log a single item. This is called to
 * log control phonemes that appear between breath
 * groups. Each one gets a log line by itself. I don't
 * log index marks, syncs, or breath breaks. Only speaking
 * phonemes (rate, pauses, etc.) are logged.
 */

void logitem(LPTTS_HANDLE_T phTTS, register short buf[]) {
	register char* cp;
	char	       oc;
	PKSD_T	       pKsd_t;

	pKsd_t = phTTS->pKernelShareData;
	/*
	 * LPTTS_HANDLE_T phTTS; phTTS = TextToSpeechGetHandle(); */

	WAIT_PRINT;

	switch(buf[0]) {
	case RATE:
#ifdef PRINTFDEBUG_OLD
		printf(":ra %d\n", buf[1]);
#endif
		if(pKsd_t->dbglog) /* mfg added for dbglog.txt logging support*/
			fprintf(pKsd_t->dbglog, ":ra %d\n", buf[1]);
		if(pKsd_t->logflag & LOG_PHONEMES) {
			if(fprintf(phTTS->pLogFile, ":ra %d\n", buf[1]) < 0) {
				// TextToSpeechErrorHandler (phTTS,
				//						  ERROR_WRITING_FILE,
				//						  0L);
			}
		}
		break;

	case CPAUSE:
#ifdef PRINTFDEBUG_OLD
		printf(":cp %d\n", buf[1]);
#endif
		if(pKsd_t->dbglog) /* mfg added for dbglog.txt logging support*/
			fprintf(pKsd_t->dbglog, ":cp %d\n", buf[1]);
		if(pKsd_t->logflag & LOG_PHONEMES) {
			if(fprintf(phTTS->pLogFile, ":cp %d\n", buf[1]) < 0) {
				// TextToSpeechErrorHandler (phTTS,
				//						  ERROR_WRITING_FILE,
				//						  0L);
			}
		}
		break;

	case PPAUSE:
#ifdef PRINTFDEBUG_OLD
		printf(":pp %d\n", buf[1]);
#endif
		if(pKsd_t->dbglog) /* mfg added for dbglog.txt logging support*/
			fprintf(pKsd_t->dbglog, ":pp %d\n", buf[1]);
		if(pKsd_t->logflag & LOG_PHONEMES) {
			if(fprintf(phTTS->pLogFile, ":pp %d\n", buf[1]) < 0) {
				// TextToSpeechErrorHandler (phTTS,
				//						  ERROR_WRITING_FILE,
				//						  0L);
			}
		}
		break;

	case NEW_SPEAKER:

		oc = "pbhfdkurwv"[buf[1]];

#ifdef PRINTFDEBUG_OLD
		printf(":n%c\n", oc);
#endif
		if(pKsd_t->dbglog) /* mfg added for dbglog.txt logging support*/
			fprintf(pKsd_t->dbglog, ":n%c\n", oc);
		if(pKsd_t->logflag & LOG_PHONEMES) {
			if(fprintf(phTTS->pLogFile, ":n%c\n", oc) < 0) {
				// TextToSpeechErrorHandler (phTTS,
				//						  ERROR_WRITING_FILE,
				//						  0L);
			}
		}
		break;

	case SPECIALWORD:
#ifdef PRINTFDEBUG_OLD
		printf(" sp ");
#endif
		if(pKsd_t->dbglog) /* mfg added for dbglog.txt logging support*/
			fprintf(pKsd_t->dbglog, " sp ");
		if(pKsd_t->logflag & LOG_PHONEMES) {
			if(fprintf(phTTS->pLogFile, " sp ") < 0) {
				// TextToSpeechErrorHandler (phTTS,
				//						  ERROR_WRITING_FILE,
				//						  0L);
			}
		}
		break;

	case NEW_PARAM:

		cp = logspnames[buf[1]];

#ifdef PRINTFDEBG
		printf(":dv %s %d\n", cp, buf[2]);
#endif
		if(pKsd_t->dbglog) /* mfg added for dbglog.txt logging support*/
			fprintf(pKsd_t->dbglog, ":dv %s %d\n", cp, buf[2]);
		if(pKsd_t->logflag & LOG_PHONEMES) {
			if(fprintf(phTTS->pLogFile, ":dv %s %d\n", cp, buf[2]) < 0) {
				// TextToSpeechErrorHandler (phTTS,
				//						  ERROR_WRITING_FILE,
				//						  0L);
			}
		}
		break;

	case SAVE:
#ifdef PRINTFDEBUG_OLD
		printf(":dv save\n");
#endif
		if(pKsd_t->dbglog) /* mfg added for dbglog.txt logging support*/
			fprintf(pKsd_t->dbglog, ":dv save\n");
		if(pKsd_t->logflag & LOG_PHONEMES) {
			if(fprintf(phTTS->pLogFile, ":dv save\n") < 0) {
				// TextToSpeechErrorHandler (phTTS,
				//						  ERROR_WRITING_FILE,
				//						  0L);
			}
		}
		break;
	}
}

void dologphoneme(LPTTS_HANDLE_T phTTS, short phone, short dur, short f0) {
	unsigned char _far* arpa;
	unsigned char _far* ascky;
	PKSD_T		    pKsd_t;
	short		    tmp;

	pKsd_t = phTTS->pKernelShareData;

	ascky = (char _far*)pKsd_t->ascky;
	arpa  = (unsigned char _far*)pKsd_t->arpabet;

	/* GL 10/03/1996       */ /*mfg_debug*/
				  /* make sure the logging for LATIN mode is correct. change E_TH(17) to E_S(15) */
	if((pKsd_t->lang_curr == LANG_latin_american) && (pKsd_t->modeflag & MODE_LATIN) && (phone == SPP_TH)) {
		phone = SPP_S;
	}
	tmp = phone >> 8;
	phone &= PVALUE;
	if(pKsd_t->phoneme_mode & PHONEME_ASCKY) {
		if(dur != 0 || f0 != 0) {
#ifdef PRINTFDEBUG_OLD
			if(ascky[phone] == '_')
				printf("%c<0,0>", ascky[phone]);
			else
				printf("%c<%d,%d>", ascky[phone], dur, f0);
#endif
			if(pKsd_t->dbglog) /* mfg added for dbglog.txt logging support*/
			{
				if(ascky[phone] == '_')
					fprintf(pKsd_t->dbglog, "%c<0,0>\n", ascky[phone]);
				else
					fprintf(pKsd_t->dbglog, "%c<%d,%d>\n", ascky[phone], dur, f0);
			}
			if((pKsd_t->logflag & LOG_OUTPHON) || (pKsd_t->logflag & LOG_PHONEMES)) {

				if(fprintf(phTTS->pLogFile,
					   "%c<%d,%d>", ascky[phone], dur, f0) < 0) {
					// TextToSpeechErrorHandler (phTTS,
					//						  ERROR_WRITING_FILE,
					//						  0L);
				}
			}
		} else {
			/* GL 03/24/1997 for BATS#313  should not send out the space for ASCKY logging */
#ifdef PRINTFDEBUG_OLD
			printf("%c", ascky[phone]);
#endif
			if(pKsd_t->dbglog) /* mfg added for dbglog.txt logging support*/
				fprintf(pKsd_t->dbglog, "%c", ascky[phone]);
			if((pKsd_t->logflag & LOG_OUTPHON) || (pKsd_t->logflag & LOG_PHONEMES)) {
				if(fprintf(phTTS->pLogFile, "%c", ascky[phone]) < 0) {
					// TextToSpeechErrorHandler (phTTS,
					//						  ERROR_WRITING_FILE,
					//						  0L);
				}
			}
		}

	} else {
		// print the langauge bits and set the arpabet table for the phonemes if applicable
		if(tmp) {
			arpa = PrintLangBit(phTTS, tmp);
		}
		if(arpa[phone * 2 + 1] == ' ') {
			if(dur != 0 || f0 != 0) {
#ifdef PRINTFDEBUG_OLD

				printf("%c<%d,%d>", arpa[phone * 2], dur, f0);

#endif

				if(pKsd_t->dbglog) /* mfg added for dbglog.txt logging support*/
				{
					if(arpa[phone * 2] == '_')
						fprintf(pKsd_t->dbglog, "%c<0,0>\n", arpa[phone * 2]);
					else
						fprintf(pKsd_t->dbglog, "%c<%d,%d>\n", arpa[phone * 2], dur, f0);
				}

				if((pKsd_t->logflag & LOG_OUTPHON) || (pKsd_t->logflag & LOG_PHONEMES)) {
					if(fprintf(phTTS->pLogFile, "%c<%d,%d>", arpa[phone * 2], dur, f0) < 0) {
						// TextToSpeechErrorHandler (phTTS,
						//						  ERROR_WRITING_FILE,
						//						  0L);
					}
				}
			} else {

#ifdef PRINTFDEBUG_OLD
				printf("%c ", arpa[phone * 2]);
#endif

				if(pKsd_t->dbglog) /* mfg added for dbglog.txt logging support*/
					fprintf(pKsd_t->dbglog, "%c ", arpa[phone * 2]);

				if((pKsd_t->logflag & LOG_OUTPHON) || (pKsd_t->logflag & LOG_PHONEMES)) {
					/* GL 03/21/1997 for BATS#305 need to send "%c " intead of "%c" only */
					if(fprintf(phTTS->pLogFile, "%c ", arpa[phone * 2]) < 0) {
						// TextToSpeechErrorHandler (phTTS,
						//						  ERROR_WRITING_FILE,
						//						  0L);
					}
				}
			}
		} else {
			if(dur != 0 || f0 != 0) {
#ifdef PRINTFDEBUG_OLD
				printf("%c%c<%d,%d>", arpa[phone * 2], arpa[phone * 2 + 1], dur, f0);
#endif
				if(pKsd_t->dbglog) /* mfg added for dbglog.txt logging support*/
					fprintf(pKsd_t->dbglog, "%c%c<%d,%d>\n", arpa[phone * 2], arpa[phone * 2 + 1], dur, f0);

				if((pKsd_t->logflag & LOG_OUTPHON) || (pKsd_t->logflag & LOG_PHONEMES)) {
					if(fprintf(phTTS->pLogFile, "%c%c<%d,%d>", arpa[phone * 2], arpa[phone * 2 + 1], dur, f0) < 0) {
						// TextToSpeechErrorHandler (phTTS,
						//						  ERROR_WRITING_FILE,
						//						  0L);
					}
				}
			} else {

#ifdef PRINTFDEBUG_OLD
				printf("%c%c", arpa[phone * 2], arpa[phone * 2 + 1]);
#endif

				if(pKsd_t->dbglog) /* mfg added for dbglog.txt logging support*/
					fprintf(pKsd_t->dbglog, "%c%c", arpa[phone * 2], arpa[phone * 2 + 1]);

				if((pKsd_t->logflag & LOG_OUTPHON) || (pKsd_t->logflag & LOG_PHONEMES)) {
					if(fprintf(phTTS->pLogFile, "%c%c", arpa[phone * 2], arpa[phone * 2 + 1]) < 0) {
						// TextToSpeechErrorHandler (phTTS,
						//						  ERROR_WRITING_FILE,
						//						  0L);
					}
				}
			}
		}
	}
}

unsigned char _far* PrintLangBit(LPTTS_HANDLE_T phTTS, short tmp) {
	PKSD_T pKsd_t;

	pKsd_t = phTTS->pKernelShareData;

	switch(tmp) {
	case PFUSA:
		printf("us_");
		if((pKsd_t->logflag & LOG_OUTPHON) || (pKsd_t->logflag & LOG_PHONEMES)) {
			if(fprintf(phTTS->pLogFile, "us_") < 0) {
				// TextToSpeechErrorHandler (phTTS,
				//	ERROR_WRITING_FILE,
				//	0L);
			}
		}
		return usa_arpa;
		break;

	case PFUK:
		printf("uk_");
		if((pKsd_t->logflag & LOG_OUTPHON) || (pKsd_t->logflag & LOG_PHONEMES)) {
			if(fprintf(phTTS->pLogFile, "uk_") < 0) {
				// TextToSpeechErrorHandler (phTTS,
				//	ERROR_WRITING_FILE,
				//	0L);
			}
		}
		return uk_arpa;
		break;

	case PFSP:
		printf("sp_");
		if((pKsd_t->logflag & LOG_OUTPHON) || (pKsd_t->logflag & LOG_PHONEMES)) {
			if(fprintf(phTTS->pLogFile, "sp_") < 0) {
				// TextToSpeechErrorHandler (phTTS,
				//	ERROR_WRITING_FILE,
				//	0L);
			}
		}
		return spanish_arpa;
		break;

	case PFLA:
		printf("la_");
		if((pKsd_t->logflag & LOG_OUTPHON) || (pKsd_t->logflag & LOG_PHONEMES)) {
			if(fprintf(phTTS->pLogFile, "la_") < 0) {
				// TextToSpeechErrorHandler (phTTS,
				//	ERROR_WRITING_FILE,
				//	0L);
			}
		}
		return la_arpa;
		break;

	case PFGR:
		printf("gr_");
		if((pKsd_t->logflag & LOG_OUTPHON) || (pKsd_t->logflag & LOG_PHONEMES)) {
			if(fprintf(phTTS->pLogFile, "gr_") < 0) {
				// TextToSpeechErrorHandler (phTTS,
				//	ERROR_WRITING_FILE,
				//	0L);
			}
		}
		return german_arpa;
		break;

	case PFFR:
		printf("fr_");
		if((pKsd_t->logflag & LOG_OUTPHON) || (pKsd_t->logflag & LOG_PHONEMES)) {
			if(fprintf(phTTS->pLogFile, "fr_") < 0) {
				// TextToSpeechErrorHandler (phTTS,
				//	ERROR_WRITING_FILE,
				//	0L);
			}
		}
		return french_arpa;
		break;

	default:
		return pKsd_t->arpabet;
		break;
	}
}

/************************************end of phlog.c***************************/
