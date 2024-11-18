#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#include "wav.c"
#include "kernel.h"
#include "epsonapi.h"
#include "cmd.h"

extern void lsmain(void);
extern void usa_main(void);
extern void kltask_init(void);
extern void InitializeVTM(void);
extern void lts_loop(unsigned short *input);

extern	int string_match(unsigned char **sa ,unsigned char *s);
extern struct share_data *kernel_share;

extern int TextToSpeechStart(char *input);
extern int TextToSpeechInit();
extern int TextToSpeechQuit();

extern int TextToSpeechChangeVoice(char *cvoice);

extern char cinput_array[];	//limited TTS length
int *samplesEachPlay;

typedef unsigned char U8;
typedef unsigned int U32;
#define get_long_int(ptr) ((U32)\
                       ((((U8 *)(ptr))[3] << 24)  | \
                        (((U8 *)(ptr))[2] << 16)  | \
                        (((U8 *)(ptr))[1] << 8)  | \
                        (((U8 *)(ptr))[0])))

extern const unsigned char main_dict[];

//extern functions
extern int cmdmain();
extern	int dtpc_cmd(unsigned char inchar);

int TextToSpeechStart(char *input) {
        //init_wav("output.wav");

	int i=0;
	KS.halting=0;	//had to reset the halting flag - since if you halt you want to halt only the one TTS you are on
	while (input[i]) {
		dtpc_cmd(input[i]);
		i++;
	}
	dtpc_cmd(0x0b);  // force it out.
	return 0;
}


int TextToSpeechInit() {
	memset(kernel_share,0,sizeof(struct share_data));

	if (main_dict) { //load main dictionary
		//KS.fdic = ((int)(main_dict))+8;
		KS.fdic_entries= get_long_int(main_dict);
	}

	usa_main();
	InitializeVTM();
	kltask_init();
	lsmain();
	cmdmain();

	KS.halting = 0;

	return 0;
}





extern const unsigned char *define_options[];
int TextToSpeechChangeVoice(char *cvoice) {
    extern short last_voice;

    // normal code
    short new_voice;
    if (*cvoice != 0) {
        if (strcmp(cvoice,"np")==0) {
            new_voice= 0; /*paul*/
        } else if (strcmp(cvoice,"nb")==0) {
            new_voice= 1; /*betty*/
        } else if (strcmp(cvoice,"nh")==0) {
            new_voice= 2; /*harry*/
        } else if (strcmp(cvoice,"nf")==0) {
            new_voice= 3; /*frank*/
        } else if (strcmp(cvoice,"nd")==0) {
            new_voice= 4; /*dennis*/
        } else if (strcmp(cvoice,"nk")==0) {
            new_voice= 5; /*kit*/
        } else if (strcmp(cvoice,"nu")==0) {
            new_voice= 6; /*ursula*/
        } else if (strcmp(cvoice,"nr")==0) {
            new_voice= 7; /*rita*/
        } else if (strcmp(cvoice,"nw")==0) {
            new_voice= 8; /*wendy*/
        } else {
            new_voice=99; /*no match*/
        }
    } else {
        return 0;
    }
    if (new_voice != 99 ) {
        last_voice = new_voice;
    }

    // temp, remove later
    const char *cmd = "sx";
    short val = 1;
    unsigned short pipe_value[3];
    pipe_value[1] = string_match((unsigned char **)define_options,cmd);
    pipe_value[0] = (2<<PSNEXTRA)+NEW_PARAM;
    pipe_value[1] -= 1;
    pipe_value[2] = val;
    lts_loop(pipe_value);

    kltask_init();
    KS.halting=0;

    return 0;
}

int TextToSpeechReset() {
        //close_wav();
	KS.halting=1;
	return 0;
}
