#ifndef _EPSONAPI_H
#define _EPSONAPI_H
//Sample rate selection
#ifdef WAVE_FORMAT_1M16
#undef WAVE_FORMAT_1M16
#endif
#ifdef WAVE_FORMAT_08M16
#undef WAVE_FORMAT_08M16
#endif

#define WAVE_FORMAT_1M16  0x0001	// 11Khz Output (71 samles per buffer)
#define WAVE_FORMAT_08M16 0x0002	// 8Khz Output (51 samples per buffer)

#define ERR_NOERROR		0	// no errors
#define ERR_ERROR		1	// random error
#define ERR_RESET		2	// Start returned early due to reset
#define ERR_INDEX		3	// Data in callback is index

#ifdef EPSON_ARM7
#define ERR_PHONEME_BUF_TOO_SHORT 4 //phoneme buffer is full and there is more to go in it
#endif

#define PTS_DONE		0
#define PTS_OUTPUT		5

extern int TextToSpeechStart(char *input,short *buffer,int output_format);
extern int TextToSpeechInit(short *(*callback)(short *,long, int),void *user_dict);
extern int TextToSpeechReset(void);
extern int TextToSpeechSync();

#endif //_EPSONAPI_H

