#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern char cinput_array[];
extern int TextToSpeechStart(char *input);
extern int TextToSpeechInit();
extern int TextToSpeechReset();
extern int TextToSpeechChangeVoice(char *cvoice);
extern void TextToSpeechSetRate(int rate);
extern int TextToSpeechSetVoiceParam(char *cmd, int value);
extern short TextToSpeechGetSpdefValue(int index);
