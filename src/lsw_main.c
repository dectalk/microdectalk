/*
 ***********************************************************************
 *
 *                           Copyright ©
 *	  Copyright © 2002 Fonix Corporation. All rights reserved.
 *	  Copyright © 2000-2001 Force Computers, a Solectron Company. All rights reserved.
 *    © SMART Modular Technologies 1999. All rights reserved.
 *    © Digital Equipment Corporation 1996-1998. All rights reserved.
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
 *    File Name:    lsw_main.c
 *    Author:       Matthew Schnee
 *    Creation Date:02/29/1996
 *
 *    Functionality:
 *    lts entry point stub, for window only
 *
 ***********************************************************************
 *    Revision History:
 *
 *  Rev Who     Date        	Description
 *  --- -----   ----------- 	---------------------------------------
 *  001 KM   	09/23/1984   	Modified to look for default user.dic
 *                              in the HOME directory for OSF/1.
 *  002 MGS     03/10/1996      Renamed file from lsm_acna.c
 *  003 JDB     05/30/1996      Add language dependent conditionals
 *  004 GL      11/26/1996      remove dummy msdos and dtex switch
 *  005 GL		12/11/1996		remove the WIN32_OLD language pipe hack
 *  006 KSB     02/10/1997		Fixed bugs for DTDemo fetch, and added debug code
 *  007 NCS     02/21/1997      Merged Tom's multilanguage code in.
 *  008 NCS     02/26/1997      Changed LibMain to LTSLibMain.
 *	009	NCS		04/17/1997		Moved Dictionary Reg entry path to coop.h
 *  010	GL		04/21/1997	    BATS#357  Add the code for __osf__ build
 *  011 NCS     08/06/1997      Moved Demo dict name entries to coop.h
 *  012 GL		09/25/1997      use the array for dictionary entry structure
 *                              add abbreviation dictionary support
 *                              also add UK_english support
 *  013 GL		10/23/1997      use dtalk_<language>.dic to replace dectalk.dic
 *  xxx	tek		25feb98			repair DEMO build to allow spaces in dictionary
 *								path (bats 607) and to look in additional places.
 *  014 JAW     03/04/1998      ifdef'd out code from 02/25/1998 so it won't be
 *                              compiled UNDER_CE.  When UNDER_CE is defined,
 *                              code from previous version of this file will be
 *                              used.
 *  015 CJL     03/18/1998      Removed specific path for dectalkf.h.
 *	016	tek		14may98			bats672 log failures on main dict registry keys
 *  017	tek		27may98			bats689: threadsafe instance counting
 *  018	mfg		22jun98			Added support for LANG_latin_american
 *	xxx	tek		21aug98			un-static GetDictionaryNames so it can be
 *								used by other modules.
 *  019.5 ETT	10/05/1998      Added Linux code.
 *  020	GL		11/12/1998		BATS#800  need to initialize some Spanish variables
 *  021	GL		11/20/1998		BATS#828 use LTS_DEBUG_OLD to replace _DEBUG_OLD
 *  022	MFG		01/06/1999		Added MainDIc and UserDic support for Windows CE
 *  023	MGS		08/22/1999		Change #ifdef LTS_DEBUG_OLD to #if LTS_DEBUG_OLD because of VMS debugging code
 *								that was turned on by accident
 *  024	MGS		02/09/2000		Made the dictionary information come from a config
 *								file for __unix__ (and __osf__ in the future)
 *  025 NAL		06/12/2000		Added prototype (warning removal)
 *  026 MFG		07/31/2000		Implemented dictionary memory mapping for Windows CE/NT/95
 *  027 MFG		07/30/2000		fixed multi language dictionary memory mapping error
 *  028	MGS		10/05/2000		Redhat 6.2 and linux warning removal
 *  029	MFG		10/10/2000		Shut off dictionary mapping for WCE_EMULATION it is not Supported
 *  030 CAB		10/18/2000		Added copyright info
 *  032	MGS		01/11/2001		Added Foreigh langauge dictioanry
 *  033	MGS		02/08/2001		Fixed Linux foreign language dictionary issues
 *  034	MGS		02/19/2001		Added code to fix the foreign language dict for WIN32_OLD
 *  035 CAB		02/23/2001		Updated copyright info
 *  036	MGS		03/02/2001		Added code for multiple instances work with dictionary mapping
 *  037	MGS		05/09/2001		Some VxWorks porting BATS#972
 *  038	MGS		05/18/2001		More VxWorks porting, fixed linux demo dictionaries
 *  039	MGS		06/19/2001		Solaris Port BATS#972
 *  040	MGS		02/26/2002		ACNA updates
 *  041	MGS		04/03/2002		Single threaded lts
 *  042	MGS		04/11/2002		ARM7 port\
 *	043	cab		04/17/2001		Fixed BATS#987
 *	044	CAB		07/08/2002		Added dtdic.log to Windows CE
 *  045	MGS		07/16/2002		Added Fonix dictionary startup
 *  046 MFG		07/17/2002		Merged in CUP28PROJECT changes
 *  047 MFG		07/19/2002		Added \windows\user.dic default for CUP28PROJECT
 *	048	CAB		08/12/2002		Fixed TTSStartupExFonix() for Windows CE
 *								Changed GetDictionaryNames() and load_dictionary() to TCHAR
 *	049	MFG		10/10/2002		Fixed the User dictionary stuff for WinCE
 *	050	RDK		01/22/2003		Change to use FORCE_WINDICTDIRS for forced windows dictionaries
 *  051	MFG		02/30/2003		Fixed the wide- string convertion for the WinCe dictionary
 ***************************************************************************/

// Only for the DECTALK_INSTALL_PREFIX, which is usually "/opt/dectalk"
#include "config.h"

#include "dectalkf.h"
#include "ls_def.h"

#define LSWMAIN_DIC
#include "coop.h"
#undef LSWMAIN_DIC

#include <stdlib.h>

#ifdef __linux
#include <linux/limits.h>
#endif
#if defined(__APPLE__)
#include <limits.h>
#include <stdint.h>
#include <sys/types.h>
#include <mach-o/dyld.h>
#endif
#include <limits.h>
#ifdef SEPARATE_PROCESSES
struct share_data* kernel_share;
#endif

#if defined(_WIN32)
#ifndef PATH_MAX
#define PATH_MAX 260
#endif
#else
#include <unistd.h>
#endif

#ifndef PATH_MAX
#define PATH_MAX 260
#endif

static size_t
dt_dirname_r(const char* path, char* buf, size_t buflen) {
	const char* endp;
	size_t	    len;

	/*
	 * If `path' is a null pointer or points to an empty string,
	 * return a pointer to the string ".".
	 */
	if(path == NULL || *path == '\0') {
		path = ".";
		len  = 1;
		goto out;
	}

	/* Strip trailing slashes, if any. */
	endp = path + strlen(path) - 1;
	while(endp != path && *endp == '/')
		endp--;

	/* Find the start of the dir */
	while(endp > path && *endp != '/')
		endp--;

	if(endp == path) {
		path = *endp == '/' ? "/" : ".";
		len  = 1;
		goto out;
	}

	do
		endp--;
	while(endp > path && *endp == '/');

	len = endp - path + 1;
out:
	if(buf != NULL && buflen != 0) {
		buflen = buflen - 1;
		if(buflen > len) buflen = len;
		if(buf != path)
			memcpy(buf, path, buflen);
		buf[buflen] = '\0';
	}
	return len;
}

char* dt_dirname(char* path) {
	static char result[PATH_MAX];
	(void)dt_dirname_r(path, result, sizeof(result));
	return result;
}

int  linux_get_dict_names(char* main_dict_name, char* user_dict_name, char* foreign_dict_name);
void default_lang(PKSD_T, unsigned int, unsigned int); // NAL warning removal

extern MMRESULT load_dictionary(void**, void**, unsigned int*, unsigned int*,
				char*, int, DT_HANDLE*, DT_HANDLE*,
				LPVOID*, MEMMAP_T);

/* PROTOTYPES */
/* MGS 11/19/1997 commented out duplicate prototype */
// extern lsa_util_init_lang();
// extern void default_lang ();
extern void default_lang(PKSD_T pKsd_t, unsigned int lang_code, unsigned int ready_code);
// extern ls_task_main ();

/*int lts_main(LPTTS_HANDLE_T phTTS )*/
int lts_main(LPTTS_HANDLE_T phTTS) {
	/*
	 * Added a variable to get current instance kernel share data and
	 * initialize from phTTS structure  :MI : MVP
	 */
	PKSD_T pKsd_t = phTTS->pKernelShareData;
	PLTS_T pLts_t = NULL;

#ifdef SEPARATE_PROCESSES
	{
		kernel_share = (struct share_data*)malloc(sizeof(struct share_data));
	}
#endif

	int nDicLoad;
	int fDicLoad;

	char main_dict_name[1000];
	char foreign_dict_name[1000];
	char user_dict_name[1000];

	linux_get_dict_names(main_dict_name, user_dict_name, foreign_dict_name);
	if(phTTS->dictionary_file_name[0]) {
		strcpy(main_dict_name, phTTS->dictionary_file_name);
	}
	/* Initialize thread error field to no error */
	// phTTS->uiThreadError = MMSYSERR_NOERROR;

	if((pLts_t = (PLTS_T)calloc(1, sizeof(LTS_T))) == NULL) {
		return (MMSYSERR_NOMEM);
	} else {
		/* MVP :Associate LTS thread specific data handle to the
		      current speech object */
		phTTS->pLTSThreadData = pLts_t;

		/* GL 09/25/1997 use the array for dictionary entry structure */
		/*               also add UK_english support */
		/*               comment out the abbr dictionary section for now */

		nDicLoad = load_dictionary((void**)&(pKsd_t->fdic_index[DICT_LANG]),
					   (void**)&(pKsd_t->fdic_data[DICT_LANG]),
					   (unsigned int*)&(pKsd_t->fdic_entries[DICT_LANG]),
					   (unsigned int*)&(pKsd_t->fdic_bytes[DICT_LANG]),
					   main_dict_name,
					   TRUE,
					   (DT_HANDLE*)&(pKsd_t->fdicMapObject[DICT_LANG]),
					   (DT_HANDLE*)&(pKsd_t->fdicFileHandle[DICT_LANG]),
					   (LPVOID*)&(pKsd_t->fdicMapStartAddr[DICT_LANG]),
					   MEMMAP_ON);

		fDicLoad = load_dictionary((void**)&(pKsd_t->foreigndic_index[DICT_LANG]),
					   (void**)&(pKsd_t->foreigndic_data[DICT_LANG]),
					   (unsigned int*)&(pKsd_t->foreigndic_entries[DICT_LANG]),
					   (unsigned int*)&(pKsd_t->foreigndic_bytes[DICT_LANG]),
					   foreign_dict_name,
					   FALSE,
					   (DT_HANDLE*)&(pKsd_t->foreigndicMapObject[DICT_LANG]),
					   (DT_HANDLE*)&(pKsd_t->foreigndicFileHandle[DICT_LANG]),
					   (LPVOID*)&(pKsd_t->foreigndicMapStartAddr[DICT_LANG]),
					   MEMMAP_ON);

		// nAdicLoad = load_dictionary( &(pKsd_t->adic[LANG_english]),
		//		   &(pKsd_t->adic_entries[LANG_english]),
		//		   "abbr_us.dic",
		//		   TRUE );

		if(nDicLoad == MMSYSERR_INVALPARAM || nDicLoad == MMSYSERR_NOMEM ||
		   nDicLoad == MMSYSERR_ERROR) {
			fprintf(stderr, "DECtalk cannot run without the dictionary file %s\n",
				main_dict_name);
			// return (nDicLoad);
		}

		/* GL 09/25/1997 support Abbr dictionary load error checking */
		/*               comment out for now */
		//    if( nAdicLoad == MMSYSERR_INVALPARAM || nAdicLoad == MMSYSERR_NOMEM ||
		//	nAdicLoad == MMSYSERR_ERROR)
		//    {
		//      fprintf(stderr,"DECtalk cannot run without the abbr. dictionary file %s\n",
		//				  "abbr.dic");
		//      phTTS->uiThreadError = nAdicLoad;
		//    }

		/*
		 * Look for an ini file in the users login directory
		 */
		{

			/*
			 * Make sure we have a valid HOME environment set.
			 */
			if(user_dict_name[0]) {
				/* GL 09/25/1997 use the array for dictionary entry structure */
				/*               also add UK_english support */
				load_dictionary((void**)&(pKsd_t->udic_index[DICT_LANG]),
						(void**)&(pKsd_t->udic_data[DICT_LANG]),
						(unsigned int*)&(pKsd_t->udic_entries[DICT_LANG]),
						(unsigned int*)&(pKsd_t->udic_bytes[DICT_LANG]),
						user_dict_name,
						FALSE,
						NULL,
						NULL,
						NULL,
						MEMMAP_OFF);
			}
		}
	}

	ls_util_lts_init(pLts_t, pKsd_t);

/* JDB: language dependent... */
#ifdef ENGLISH_US
#ifdef ACNA
	lsa_util_init_lang(pLts_t);
#endif
	default_lang(pKsd_t, LANG_english, LANG_lts_ready);
#endif

/* GL 09/25/1997 add UK_english support */
#ifdef ENGLISH_UK
#ifdef ACNA
	lsa_util_init_lang(pLts_t);
#endif
	default_lang(pKsd_t, LANG_british, LANG_lts_ready);
#endif

#ifdef SPANISH_SP
	default_lang(pKsd_t, LANG_spanish, LANG_lts_ready);
#endif

#ifdef SPANISH_LA
	default_lang(pKsd_t, LANG_latin_american, LANG_lts_ready);
#endif

#ifdef SPANISH
	/* GL 11/12/1998, BATS#800 need to initialize these variables for Spanish */
	pLts_t->ord	   = 0;
	pLts_t->dic_offset = 0;
	pLts_t->flag	   = 0;
#endif

#ifdef GERMAN
	default_lang(pKsd_t, LANG_german, LANG_lts_ready);
#endif

#ifdef FRENCH
	default_lang(pKsd_t, LANG_french, LANG_lts_ready);
#endif

	ls_task_main(phTTS);
	/* Free here thread specific data structure MVP */
	return MMSYSERR_NOERROR; // NAL warning removal
}

/*extern int fc_index; */ /*MVP MI */

#ifdef ENGLISH_US
#define LINUX_DICT_TAG "US_dict:"
#define LINUX_FDICT_TAG "US_fdict:"
#define LINUX_UDICT_TAG "US_udict:"
#define DEF_LINUX_MAIN_DICT "dtalk_us.dic"
#define DEF_LINUX_FOREIGN_DICT "dtalk_fl_us.dic"
#define DEMO_DICT_NAME "dtalk_us.dic"
#define DEMO_FDICT_NAME "dtalk_fl_us.dic"
#endif
#ifdef ENGLISH_UK
#define LINUX_DICT_TAG "UK_dict:"
#define LINUX_FDICT_TAG "UK_fdict:"
#define LINUX_UDICT_TAG "UK_udict:"
#define DEF_LINUX_MAIN_DICT "dtalk_uk.dic"
#define DEF_LINUX_FOREIGN_DICT "dtalk_fl_uk.dic"
#define DEMO_DICT_NAME "dtalk_uk.dic"
#define DEMO_FDICT_NAME "dtalk_fl_uk.dic"
#endif
#ifdef GERMAN
#define LINUX_DICT_TAG "GR_dict:"
#define LINUX_FDICT_TAG "GR_fdict:"
#define LINUX_UDICT_TAG "GR_udict:"
#define DEF_LINUX_MAIN_DICT "dtalk_gr.dic"
#define DEF_LINUX_FOREIGN_DICT "dtalk_fl_gr.dic"
#define DEMO_DICT_NAME "dtalk_gr.dic"
#define DEMO_FDICT_NAME "dtalk_fl_gr.dic"
#endif
#ifdef SPANISH_SP
#define LINUX_DICT_TAG "SP_dict:"
#define LINUX_FDICT_TAG "SP_fdict:"
#define LINUX_UDICT_TAG "SP_udict:"
#define DEF_LINUX_MAIN_DICT "dtalk_sp.dic"
#define DEF_LINUX_FOREIGN_DICT "dtalk_fl_sp.dic"
#define DEMO_DICT_NAME "dtalk_sp.dic"
#define DEMO_FDICT_NAME "dtalk_fl_sp.dic"
#endif
#ifdef SPANISH_LA
#define LINUX_DICT_TAG "LA_dict:"
#define LINUX_FDICT_TAG "LA_fdict:"
#define LINUX_UDICT_TAG "LA_udict:"
#define DEF_LINUX_MAIN_DICT "dtalk_la.dic"
#define DEF_LINUX_FOREIGN_DICT "dtalk_fl_la.dic"
#define DEMO_DICT_NAME "dtalk_la.dic"
#define DEMO_FDICT_NAME "dtalk_fl_la.dic"
#endif
#ifdef FRENCH
#define LINUX_DICT_TAG "FR_dict:"
#define LINUX_FDICT_TAG "FR_fdict:"
#define LINUX_UDICT_TAG "FR_udict:"
#define DEF_LINUX_MAIN_DICT "dtalk_fr.dic"
#define DEF_LINUX_FOREIGN_DICT "dtalk_fl_fr.dic"
#define DEMO_DICT_NAME "dtalk_fr.dic"
#define DEMO_FDICT_NAME "dtalk_fl_fr.dic"
#endif

int linux_get_dict_names(char* main_dict_name, char* user_dict_name, char* foreign_dict_name) {
	FILE* config_file = NULL;
	char  line[1000];
	char* home_dir;
	char  temp_dict_name[1000];
	int   ret_value = 0;
	int   parent	= 0;
	int   exe_path	= 0;

	main_dict_name[0]    = '\0';
	foreign_dict_name[0] = '\0';
	user_dict_name[0]    = '\0';

#ifndef NO_FILESYSTEM

	if(config_file == NULL) {
		char p[PATH_MAX] = {0};
#if defined(_WIN32)
		ssize_t count = 3;
		p[0]	      = '.';
		p[1]	      = '/';
		p[2]	      = 'h';
		p[3]	      = 0;
#elif defined(__APPLE__)
		char	 ep[PATH_MAX] = {};
		uint32_t size	      = sizeof(ep);
		ssize_t	 count	      = -1;
		if(_NSGetExecutablePath(ep, &size) == 0) {
			if(realpath(ep, p) != NULL) {
				count = strlen(p);
			}
		}
#elif defined(__unix__)
		ssize_t count = readlink("/proc/self/exe", p, PATH_MAX);
#endif
		if(count != -1) {
			char* cfg;
			cfg = dt_dirname(p);
			strcat(cfg, "/");
			strcat(cfg, "DECtalk.conf");
			config_file = fopen(cfg, "r");
		}
		if(config_file != NULL) {
			exe_path = 1;
		}
	}

	if(config_file == NULL) {
		char p[PATH_MAX] = {0};
#if defined(_WIN32)
		ssize_t count = 3;
		p[0]	      = '.';
		p[1]	      = '/';
		p[2]	      = 'h';
		p[3]	      = 0;
#elif defined(__APPLE__)
		char	 ep[PATH_MAX] = {};
		uint32_t size	      = sizeof(ep);
		ssize_t	 count	      = -1;
		if(_NSGetExecutablePath(ep, &size) == 0) {
			if(realpath(ep, p) != NULL) {
				count = strlen(p);
			}
		}
#elif defined(__unix__)
		ssize_t count = readlink("/proc/self/exe", p, PATH_MAX);
#endif
		if(count != -1) {
			char* cfg;
			cfg = dt_dirname(p);
			strcat(cfg, "/../");
			strcat(cfg, "DECtalk.conf");
			config_file = fopen(cfg, "r");
		}
		if(config_file != NULL) {
			parent	 = 1;
			exe_path = 1;
		}
	}

	if(config_file == NULL) {
		config_file = fopen("DECtalk.conf", "r");
	}

	if(config_file == NULL) {
		config_file = fopen(DECTALK_INSTALL_PREFIX "/DECtalk.conf", "r");
	}

// #ifdef DEMO
#if 0
        strcpy(main_dict_name,DEMO_DICT_NAME);
        strcpy(foreign_dict_name,DEMO_FDICT_NAME);
        ret_value++;
#else
	if(config_file != NULL) {
		while(fgets(line, 999, config_file) != NULL) {
			if(strncmp(line, LINUX_DICT_TAG, 8) == 0) {
				line[strlen(line) - 1] = '\0';
				strcpy(main_dict_name, line + 8);
				if(exe_path && (main_dict_name[0] != '/')) {
					char p[PATH_MAX] = {0};
#if defined(_WIN32)
					ssize_t count = 3;
					p[0]	      = '.';
					p[1]	      = '/';
					p[2]	      = 'h';
					p[3]	      = 0;
#elif defined(__APPLE__)
					char	 ep[PATH_MAX] = {};
					uint32_t size	      = sizeof(ep);
					ssize_t	 count	      = -1;
					if(_NSGetExecutablePath(ep, &size) == 0) {
						if(realpath(ep, p) != NULL) {
							count = strlen(p);
						}
					}
#elif defined(__unix__)
					ssize_t count = readlink("/proc/self/exe", p, PATH_MAX);
#endif
					if(count != -1) {
						char* dict;
						dict = dt_dirname(p);
						strcat(dict, "/");
						if(parent)
							strcat(dict, "../");
						strcat(dict, main_dict_name);
						strcpy(main_dict_name, dict);
					}
				}
				break;
			}
		}
	}
	if(main_dict_name[0] == '\0') {
		fprintf(stderr, "libtts.so: Using default dictionary name\n");
		strcpy(main_dict_name, DEF_LINUX_MAIN_DICT);
		if(exe_path && (main_dict_name[0] != '/')) {
			char p[PATH_MAX] = {0};
#if defined(_WIN32)
			ssize_t count = 3;
			p[0]	      = '.';
			p[1]	      = '/';
			p[2]	      = 'h';
			p[3]	      = 0;
#elif defined(__APPLE__)
			char	 ep[PATH_MAX] = {};
			uint32_t size	      = sizeof(ep);
			ssize_t	 count	      = -1;
			if(_NSGetExecutablePath(ep, &size) == 0) {
				if(realpath(ep, p) != NULL) {
					count = strlen(p);
				}
			}
#elif defined(__unix__)
			ssize_t count = readlink("/proc/self/exe", p, PATH_MAX);
#endif
			if(count != -1) {
				char* dict;
				dict = dt_dirname(p);
				strcat(dict, "/");
				if(parent)
					strcat(dict, "../");
				strcat(dict, DEF_LINUX_MAIN_DICT);
				strcpy(main_dict_name, dict);
			}
		}
	} else {
		ret_value++;
	}
	if(config_file != NULL) {
		fseek(config_file, 0, SEEK_SET);
		while(fgets(line, 999, config_file) != NULL) {
			if(strncmp(line, LINUX_FDICT_TAG, 9) == 0) {
				line[strlen(line) - 1] = '\0';
				strcpy(foreign_dict_name, line + 9);
				if(exe_path && (foreign_dict_name[0] != '/')) {
					char p[PATH_MAX] = {0};
#if defined(_WIN32)
					ssize_t count = 2;
					p[0]	      = '.';
					p[1]	      = '/';
					p[2]	      = 0;
#elif defined(__APPLE__)
					char	 ep[PATH_MAX] = {};
					uint32_t size	      = sizeof(ep);
					ssize_t	 count	      = -1;
					if(_NSGetExecutablePath(ep, &size) == 0) {
						if(realpath(ep, p) != NULL) {
							count = strlen(p);
						}
					}
#elif defined(__unix__)
					ssize_t count = readlink("/proc/self/exe", p, PATH_MAX);
#endif
					if(count != -1) {
						char* dict;
						dict = dt_dirname(p);
						strcat(dict, "/");
						if(parent)
							strcat(dict, "../");
						strcat(dict, foreign_dict_name);
						strcpy(foreign_dict_name, dict);
					}
				}
				break;
			}
		}
	}
	if(foreign_dict_name[0] == '\0') {
		// fprintf(stderr,"libtts.so: Using default foreign dictionary name\n");
		strcpy(foreign_dict_name, DEF_LINUX_FOREIGN_DICT);
		if(exe_path && (foreign_dict_name[0] != '/')) {
			char p[PATH_MAX] = {0};
#if defined(_WIN32)
			ssize_t count = 3;
			p[0]	      = '.';
			p[1]	      = '/';
			p[2]	      = 'h';
			p[3]	      = 0;
#elif defined(__APPLE__)
			char	 ep[PATH_MAX] = {};
			uint32_t size	      = sizeof(ep);
			ssize_t	 count	      = -1;
			if(_NSGetExecutablePath(ep, &size) == 0) {
				if(realpath(ep, p) != NULL) {
					count = strlen(p);
				}
			}
#elif defined(__unix__)
			ssize_t count = readlink("/proc/self/exe", p, PATH_MAX);
#endif
			if(count != -1) {
				char* dict;
				dict = dt_dirname(p);
				strcat(dict, "/");
				if(parent)
					strcat(dict, "../");
				strcat(dict, DEF_LINUX_FOREIGN_DICT);
				strcpy(foreign_dict_name, dict);
			}
		}
	} else {
		ret_value++;
	}
#endif
	home_dir = getenv("HOME");
	if(home_dir != NULL) {
		temp_dict_name[0] = '\0';
		if(config_file != NULL) {
			fseek(config_file, 0, SEEK_SET);
			while(fgets(line, 999, config_file) != NULL) {
				if(strncmp(line, LINUX_UDICT_TAG, 9) == 0) {
					line[strlen(line) - 1] = '\0';
					strcpy(temp_dict_name, line + 9);
					break;
				}
			}
		}
		if(temp_dict_name[0]) {
			sprintf(user_dict_name, "%s/%s", home_dir, temp_dict_name);
			ret_value++;
		} else {
			sprintf(user_dict_name, "%s/udic.dic", home_dir);
		}
	}
	if(config_file != NULL) {
		fclose(config_file);
	}
#endif
	return (ret_value);
}
