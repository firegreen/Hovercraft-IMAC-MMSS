#include "audios.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define BUFSIZE 512
#define readLine(strin, bufsize, fp) strin[0] = 0; fgets(strin,bufsize,fp); if(!strin[0]){ return;}
#define ERROR fprintf(stderr,"Error while reading audios files"); return;

Audio* audios = NULL;
Audio** currentAudios = NULL;
int nbAudios;
int nbCurrentAudio;

Sint16 sample_mix(Sint16 src1, Sint16 src2)
{
    Sint32 mix = src1 + src2;
    Sint32 max = (1<<16)-1;

    if (mix > max) mix = max;
    if (mix < - max) mix = -max;

    return (Sint16)mix;
}

Sint16 sample_volume(Sint16 sample, float volume){
    return (sample * volume) / 255.;
}

void mixaudio(void * userData, Uint8 * stream, int len)
{
    int i = 0,j;
    Uint32 nbSamples = len / sizeof(Sint16);

    for(;i<nbCurrentAudio;i++){
        Audio* a = currentAudios[i];
        if(a->play){
            if(a->fadeOut)
            {
                a->currentVolume -= a->fadeOut;
                if(a->currentVolume<=0){
                    a->currentVolume = 0;
                    a->fadeOut = 0;
                    a->play = 0;
                }
            }
            else if(a->fadeIn)
            {
                a->currentVolume += a->fadeIn;
                if(a->currentVolume>=a->maxVolume){
                    a->currentVolume = a->maxVolume;
                    a->fadeIn = 0;
                }
            }
            for (j = 0; j < nbSamples; j++) {
                Sint16 mix = ((Sint16 *)stream)[j];
                Sint16 sample =   a->soundpos < a->soundlength ?
                            (*((Sint16 *)(a->sounddata + a->soundpos))) :
                            0;
                sample = sample_volume(sample,a->currentVolume);
                sample = sample_mix(sample, mix);
                ((Sint16 *)stream)[j] = sample;

                a->soundpos += sizeof(Sint16);
            }
            if(a->soundpos>a->soundlength){
                if(!a->repeat){
                    a->play = 0;
                }
                a->soundpos = 0;
            }
        }
    }
}

void initAudios(Uint16 format, int freq, Uint8 channels, Uint16 samples){
    char audioFile[BUFSIZE/8];
    char strin[BUFSIZE];
    FILE* fp;
    char strincmp[BUFSIZE/8];

    audioSpec.initialConfig.format =format;// AUDIO_S16SYS;
    audioSpec.initialConfig.freq = freq;//22050;
    audioSpec.initialConfig.channels = channels;//1;
    audioSpec.initialConfig.samples = samples;//512;
    audioSpec.initialConfig.userdata = NULL;
    audioSpec.initialConfig.callback = &mixaudio;
    if (SDL_OpenAudio(&(audioSpec.initialConfig), &(audioSpec.currentConfig)) != 0) {
        printf("Erreur lors de l'ouverture du périphérique audio: %s\n", SDL_GetError());
    }

    fp = fopen ("audios", "r");
    if(fp == NULL)
    {
        ERROR
    }
    readLine(strin,BUFSIZE,fp);
    if(sscanf(strin, "%d",&nbAudios)<1){
        return;
    }
    if(nbAudios<=0) return;
    audios = malloc(nbAudios * sizeof(Audio));
    currentAudios = malloc(nbAudios * sizeof(Audio*));
    nbCurrentAudio=0;
    int i;
    float volume;
    for(i=0;i<nbAudios;i++){
        readLine(strin,BUFSIZE,fp);
        memset(audioFile,0,BUFSIZE/8);
        memset(strincmp,0,BUFSIZE/8);
        volume=100;
        strcpy(strincmp,"repeat");
        if(sscanf(strin, "\"%[^\t\n\"]\" %s %f",audioFile, strincmp,&volume)>1){
            makeAudio(i, audioFile,!strcmp(strincmp,"repeat"),volume);
        }
    }
}

void makeAudio(int i, const char* filename, unsigned char repeat, float volume){
    Audio* a = audios + i;
    if (SDL_LoadWAV(filename, &(a->spec), &(a->sounddata), &(a->soundlength)) == NULL) {
        printf("Erreur lors du chargement du fichier son: %s\n", SDL_GetError());
        return;
    }
    if (SDL_BuildAudioCVT(&(a->audioConverter), a->spec.format,
                          a->spec.channels, a->spec.freq,
                          audioSpec.currentConfig.format, audioSpec.currentConfig.channels,
                          audioSpec.currentConfig.freq) < 0) {
        printf("Impossible de construire le convertisseur audio!\n");
        return;
    }
    a->audioConverter.buf = malloc(a->soundlength * a->audioConverter.len_mult);
    a->audioConverter.len = a->soundlength;
    memcpy(a->audioConverter.buf, a->sounddata, a->soundlength);
    if (SDL_ConvertAudio(&(a->audioConverter)) != 0) {
        printf("Erreur lors de la conversion du fichier audio: %s\n", SDL_GetError());
        return;
    }
    SDL_FreeWAV(a->sounddata);
    a->sounddata = malloc(a->audioConverter.len_cvt);
    memcpy(a->sounddata, a->audioConverter.buf, a->audioConverter.len_cvt);
    free(a->audioConverter.buf);

    a->soundlength = a->audioConverter.len_cvt;
    a->soundpos = 0;
    a->repeat = repeat;
    a->currentVolume = 0;
    a->maxVolume = volume;
    a->play = a->fadeIn = a->fadeOut = 0;
}

void playAudio(int id){
    id--;
    if(id<0 || id>=nbAudios) return;
    if(!audios[id].play){
        audios[id].play = 1;
        audios[id].currentVolume = audios[id].maxVolume;
        currentAudios[nbCurrentAudio] = audios + id;
        nbCurrentAudio ++;
    }
}

void stopAudio(int id){
    id--;
    if(id<0 || id>=nbAudios) return;
    if(audios[id].play){
        audios[id].play = 0;
        nbCurrentAudio--;
        int i=0;
        for(;currentAudios[i]!=audios+id;i++);
        for(;i<nbCurrentAudio;i++)
            currentAudios[i] = currentAudios[i+1];
    }
}

void playAudioFadeIn(int id, float fadeValue){
    id--;
    if(id<0 || id>=nbAudios) return;
    if(!audios[id].play){
        audios[id].play = 1;
        audios[id].fadeIn = fadeValue;
        audios[id].fadeOut = 0;
        currentAudios[nbCurrentAudio] = audios + id;
        nbCurrentAudio ++;
    }
}

void stopAudioFadeOut(int id, float fadeValue){
    id--;
    if(id<0 || id>=nbAudios) return;
    if(audios[id].play){
        audios[id].play = 0;
        audios[id].fadeIn = 0;
        audios[id].fadeOut = fadeValue;
        nbCurrentAudio--;
        int i=0;
        for(;currentAudios[i]!=audios+id;i++);
        for(;i<nbCurrentAudio;i++)
            currentAudios[i] = currentAudios[i+1];
    }
}

