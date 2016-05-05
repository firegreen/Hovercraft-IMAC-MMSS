#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#include <SDL.h>
#include <SDL_image.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#endif

#include "SDL_tools.h"

Audio* audios = NULL;
int nbAudios;

void resize_handler(float width, float height)
{
    window.width = width;
    window.height = height;
    window.orthoGLX = GLREPERE;
    window.orthoGLY = GLREPERE * height/width;
    reloadGLrepere();
}

void reloadGLrepere(){
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-window.orthoGLX, window.orthoGLX, -window.orthoGLY, window.orthoGLY);
}


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

    for(;i<nbAudios;i++){
        Audio* a = audios + i;
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
                /* Ecriture du nouveau son mixé */
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

void initialize_audio(Uint16 format, int freq, Uint8 channels, Uint16 samples){
    audios = malloc(sizeof(Audio)*NBAUDIOMAX);
    nbAudios = 0;
    audioSpec.initialConfig.format =format;// AUDIO_S16SYS;
    audioSpec.initialConfig.freq = freq;//22050;
    audioSpec.initialConfig.channels = channels;//1;
    audioSpec.initialConfig.samples = samples;//512;
    audioSpec.initialConfig.userdata = NULL;
    audioSpec.initialConfig.callback = &mixaudio;
    if (SDL_OpenAudio(&(audioSpec.initialConfig), &(audioSpec.currentConfig)) != 0) {
        printf("Erreur lors de l'ouverture du périphérique audio: %s\n", SDL_GetError());
    }
}

void initialize_window(float width, float height, unsigned char fullscreen)
{
    /* Initialize the SDL library (starts the event loop) */
    if ( SDL_Init(SDL_INIT_VIDEO  | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK) < 0 )
    {
        fprintf(stderr,"Couldn't initialize SDL: %s\n", SDL_GetError());
        exit(1);
    }

    /* Clean up on exit, exit on window close and interrupt */
    atexit(SDL_Quit);
    unsigned long option = SDL_GL_DOUBLEBUFFER | SDL_OPENGL | SDL_RESIZABLE
            | (SDL_FULLSCREEN * fullscreen);
    /* The mouse isn't much use unless we have a display for reference */
    if (SDL_SetVideoMode(width, height, BITS_PER_PIXEL, option) == NULL )
    {
        fprintf(stderr, "Couldn't set %fx%fx%d video mode: %s\n",width,height,BITS_PER_PIXEL,SDL_GetError());
        exit(1);
    }
    SDL_JoystickEventState(SDL_ENABLE);
    glDisable(GL_DEPTH_TEST);
    resize_handler(width,height);
}

int makeAudio(const char* filename, unsigned char repeat, float volume){
    Audio* a = audios + nbAudios;
    if (SDL_LoadWAV(filename, &(a->spec), &(a->sounddata), &(a->soundlength)) == NULL) {
        printf("Erreur lors du chargement du fichier son: %s\n", SDL_GetError());
        return -1;
    }
    if (SDL_BuildAudioCVT(&(a->audioConverter), a->spec.format,
                          a->spec.channels, a->spec.freq,
                          audioSpec.currentConfig.format, audioSpec.currentConfig.channels,
                          audioSpec.currentConfig.freq) < 0) {
        printf("Impossible de construire le convertisseur audio!\n");
        return -1;
    }
    a->audioConverter.buf = malloc(a->soundlength * a->audioConverter.len_mult);
    a->audioConverter.len = a->soundlength;
    memcpy(a->audioConverter.buf, a->sounddata, a->soundlength);
    if (SDL_ConvertAudio(&(a->audioConverter)) != 0) {
        printf("Erreur lors de la conversion du fichier audio: %s\n", SDL_GetError());
        return -1;
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
    nbAudios++;
    return nbAudios-1;
}

void playAudio(int id){
    audios[id].play = 1;
    audios[id].currentVolume = audios[id].maxVolume;
}

void stopAudio(int id){
    audios[id].play = 0;
}

void playAudioFadeIn(int id, float fadeValue){
    audios[id].play = 1;
    audios[id].fadeOut = 0;
    audios[id].fadeIn = fadeValue;
}

void stopAudioFadeOut(int id, float fadeValue){
    audios[id].fadeIn = 0;
    audios[id].fadeOut = fadeValue;
}

int windowEventHandler(const SDL_Event* event)
{
    switch(event->type)
    {
    case SDL_VIDEORESIZE:
        resize_handler(event->resize.w,event->resize.h);
        return 1;
    default:
        break;
    }
    return 0;
}

void makeTexture(GLuint id, const char *filename, GLint textureFormat){
    SDL_Surface* image = IMG_Load(filename);
    if(image == NULL) {
        fprintf(stderr, "Impossible de charger l'image %s\n", filename);
    }
    else{
        glBindTexture(GL_TEXTURE_2D, id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        GLenum format;
        switch(image->format->BytesPerPixel) {
        case 1:
            format = GL_BLUE;
            break;
        case 3:
            format = GL_RGB;
            break;
        case 4:
            format = GL_RGBA;
            break;
        default:
            format = GL_RGBA;
            fprintf(stderr, "Format des pixels de l'image %d non pris en charge - byte per pixel :%d\n",
                    id, image->format->BytesPerPixel);
        }
        glTexImage2D(GL_TEXTURE_2D, 0, textureFormat, image->w, image->h,
                     0, format, GL_UNSIGNED_BYTE, image->pixels);
        SDL_FreeSurface(image);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

void drawTextureQuad(GLuint id, const Color4f* c, const Bounds2P* b){
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, id);
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBegin(GL_QUADS);

    glColor4f(c->r, c->g, c->b,c->a);

    glTexCoord2f(0, 0);
    glVertex2f(b->leftTop.x, b->leftTop.y);

    glTexCoord2f(0, 1);
    glVertex2f(b->leftTop.x, b->rightBottom.y);

    glTexCoord2f(1, 1);
    glVertex2f(b->rightBottom.x, b->rightBottom.y);

    glTexCoord2f(1, 0);
    glVertex2f(b->rightBottom.x, b->leftTop.y);

    glEnd();
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
}
