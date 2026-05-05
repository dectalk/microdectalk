#ifndef _EPSONAPI_H
#define _EPSONAPI_H
// Sample rate selection
#ifdef WAVE_FORMAT_1M16
#undef WAVE_FORMAT_1M16
#endif
#ifdef WAVE_FORMAT_08M16
#undef WAVE_FORMAT_08M16
#endif

#define WAVE_FORMAT_1M16 0x0001	 // 11Khz Output (71 samles per buffer)
#define WAVE_FORMAT_08M16 0x0002 // 8Khz Output (51 samples per buffer)

#define ERR_NOERROR 0 // no errors
#define ERR_ERROR 1   // random error
#define ERR_RESET 2   // Start returned early due to reset
#define ERR_INDEX 3   // Data in callback is index

#define PTS_DONE 0
#define PTS_OUTPUT 5

#ifndef DT_EXTERN
#if defined(BLD_DECTALK_DLL) && defined(_WIN32)
#define DT_EXTERN extern __declspec(dllexport)
#elif defined(_WIN32)
#define DT_EXTERN extern __declspec(dllimport)
#else
#define DT_EXTERN extern
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

DT_EXTERN void TextToSpeechSafeInit(void); /* call this if you want to initialize safely (e.g. when using in multithread program) */

DT_EXTERN void* TextToSpeechAllocate(void);
DT_EXTERN void	TextToSpeechFree(void* tts);
DT_EXTERN int	TextToSpeechStartEx(void* tts, char* input, short* buffer_deprecated, int output_format);
DT_EXTERN int	TextToSpeechInitEx(void* tts, short* (*callback)(short*, long, int), short* (*callback_ex)(void*, short*, long, int), void* user_dict);
DT_EXTERN int	TextToSpeechResetEx(void* tts);
DT_EXTERN int	TextToSpeechSyncEx(void* tts);
DT_EXTERN int	TextToSpeechChangeVoiceEx(void* tts, const char* cvoice);
DT_EXTERN void	TextToSpeechSetRateEx(void* tts, int rate);
DT_EXTERN int	TextToSpeechSetVoiceParamEx(void* tts, const char* cmd, int value);
DT_EXTERN short TextToSpeechGetSpdefValueEx(void* tts, int index);

DT_EXTERN int	TextToSpeechStart(char* input, short* buffer_deprecated, int output_format);
DT_EXTERN int	TextToSpeechInit(short* (*callback)(short*, long, int), void* user_dict);
DT_EXTERN int	TextToSpeechReset(void);
DT_EXTERN int	TextToSpeechSync(void);
DT_EXTERN int	TextToSpeechChangeVoice(const char* cvoice);
DT_EXTERN void	TextToSpeechSetRate(int rate);
DT_EXTERN int	TextToSpeechSetVoiceParam(const char* cmd, int value);
DT_EXTERN short TextToSpeechGetSpdefValue(int index);

#ifdef __cplusplus
}
#endif

#endif //_EPSONAPI_H
