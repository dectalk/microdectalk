#include <SDL.h>
#include <SDL_main.h>
#include <epsonapi.h>

static SDL_AudioDeviceID dev;

static short* dtc_callback(short* samples, long frames, int phoneme){
	SDL_QueueAudio(dev, samples, frames * 2);

	return samples;
}

int main(int argc, char** argv){
	SDL_AudioSpec spec;
	SDL_Event ev;

	memset(&spec, 0, sizeof(spec));
	spec.freq = 11025;
	spec.format = AUDIO_S16SYS;
	spec.channels = 1;
	spec.samples = 4096;

	SDL_Init(SDL_INIT_AUDIO);

	if((dev = SDL_OpenAudioDevice(NULL, 0, &spec, NULL, 0)) == 0) return 1;
	SDL_PauseAudioDevice(dev, 0);

	TextToSpeechInit(dtc_callback, NULL);
	TextToSpeechStart("Hello, I'm DECtalk.", NULL, WAVE_FORMAT_1M16);
	TextToSpeechSync();

	while(SDL_GetQueuedAudioSize(dev) > 0){
		while(SDL_PollEvent(&ev)){
			if(ev.type == SDL_QUIT) return 0;
		}
	}
}
