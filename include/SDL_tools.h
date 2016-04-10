#ifndef SDL_TOOLS_H
#define SDL_TOOLS_H

#ifdef __APPLE__
#include <SDL.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <SDL/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#define BITS_PER_PIXEL 32
#define MILLISECOND_PER_FRAME 1000 / 60
#define GLREPERE 100
#define NBAUDIOMAX 10
#define GL_BRG 0x80E
#define GL_BRGA 0x801

struct {
  float width;
  float height;
  float orthoGLX;
  float orthoGLY;
} window;

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

void resize_handler(float width, float height);
void initialize_window(float width, float height, unsigned char fullscreen);
void initialize_audio(Uint16 format, int freq, Uint8 channels, Uint16 samples);
int makeAudio(const char* filename, unsigned char repeat, float volume);
void playAudio(int id);
void stopAudio(int id);
void playAudioFadeIn(int id, float fadeValue);
void stopAudioFadeOut(int id, float fadeValue);
int windowEventHandler(const SDL_Event* event);


#endif
