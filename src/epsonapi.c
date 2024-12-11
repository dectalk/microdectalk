#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#include "wav.c"
#include "kernel.h"
#include "epsonapi.h"
#include "cmd.h"

extern int lsmain();
extern void usa_main();
extern int kltask_init();
extern void InitializeVTM();
extern void lts_loop(unsigned short *input);

extern	int string_match(unsigned char **sa ,unsigned char *s);
extern struct share_data *kernel_share;

extern int TextToSpeechStart(char *input);
extern int TextToSpeechInit();
extern int TextToSpeechQuit();
extern int TextToSpeechChangeVoice(char *cvoice);

//extern functions
extern int cmdmain();
extern void vtm_main();
extern int dtpc_cmd(unsigned char inchar);

short TextToSpeechGetSpdefValue(int index) {
    extern short curspdef[];
    return curspdef[index];
}

extern const unsigned char *define_options[];
int TextToSpeechSetVoiceParam(char *cmd, int value) {
    unsigned short pipe_value[3];
    pipe_value[1] = string_match((unsigned char **)define_options,cmd);
    pipe_value[0] = (2<<PSNEXTRA)+NEW_PARAM;
    pipe_value[1] -= 1;
    pipe_value[2] = value;
    lts_loop(pipe_value);
}

void TextToSpeechSetRate(int rate) {
    unsigned short  pipe_value[2];
    pipe_value[0] = (1<<PSNEXTRA) + RATE;
    pipe_value[1] = rate;
    lts_loop(pipe_value);
}

int TextToSpeechStart(char *input) {
    int i=0;
    KS.halting=0;	//had to reset the halting flag - since if you halt you want to halt only the one TTS you are on

    while (input[i]) {
        dtpc_cmd(input[i]);
        i++;
    }
    dtpc_cmd(0x0b);  // force it out.

    return 0;
}

// dictionary externs
extern unsigned char *mdict;
extern unsigned char *udict;
extern const unsigned char main_dict[];

// unused for now
int TextToSpeechLoadUserDictionary(const unsigned char *user_dict) {
    if (user_dict) {
        udict = user_dict;
    }
}

int TextToSpeechInit() {
    memset(kernel_share,0,sizeof(struct share_data));

    if (main_dict) { //load main dictionary
        mdict = main_dict;
    }

    vtm_main();
    usa_main();
    InitializeVTM();
    kltask_init();
    lsmain();
    cmdmain();

    KS.halting = 0;

    return 0;
}

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

    kltask_init();

    KS.halting=0;

    return 0;
}

int TextToSpeechReset() {
    KS.halting=1;
    return 0;
}
