#ifndef __AUDIO
#define __AUDIO

#ifdef __APPLE__
#include <SDL.h>
#else
#include <SDL/SDL.h>
#endif

struct {
    SDL_AudioSpec initialConfig;
    SDL_AudioSpec currentConfig;
} audioSpec;

typedef struct {

    SDL_AudioSpec spec;
    SDL_AudioCVT  audioConverter;

    Uint8 * sounddata;
    Uint32 soundlength;
    Uint32 soundpos;

    unsigned char play;
    unsigned char repeat;
    float currentVolume,maxVolume;
    float fadeIn,fadeOut;
} Audio;

void initAudios(Uint16 format, int freq, Uint8 channels, Uint16 samples);
void makeAudio(int id, const char* filename, unsigned char repeat, float volume);
void playAudio(int id);
void stopAudio(int id);
void playAudioFadeIn(int id, float fadeValue);
void stopAudioFadeOut(int id, float fadeValue);

#endif
