#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "kernel.h"
#include "epsonapi.h"

extern void lsmain(void);
extern void usa_main(void);
extern void kltask_init(void);
extern void InitializeVTM(void);
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

int TextToSpeechReset() {
	KS.halting=1;
	return 0;
}
