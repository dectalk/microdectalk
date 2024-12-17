#include <stdint.h>

extern	int16_t d2pole_cf45(int16_t *bcoef,int16_t *ccoef,int16_t frequency,int16_t bandwidth,int16_t gain);
extern	int16_t d2pole_cf123(int16_t *bcoef,int16_t *ccoef,int16_t frequency,int16_t bandwidth,int16_t gain);
extern	int16_t d2pole_pf(int16_t *bcoef,int16_t *ccoef,int16_t frequency,int16_t bandwidth,int16_t gain);
extern	void speech_waveform_generator(void);
extern	void read_speaker_definition(void);
extern	void InitializeVTM(void);
extern	void SetSampleRate(uint16_t  uiSampRate);
extern	void doit(void);
