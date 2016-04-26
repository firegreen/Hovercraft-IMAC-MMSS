#include "game.h"
#include "SDL_tools.h"
#include "GUI/level.h"
#include "image_src.h"
#include "Object2D/hovercraft.h"
#include "GUI/menu.h"
#include <stdlib.h>

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#include <SDL.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <SDL/SDL.h>
#endif


void drawGame(){
    switch (Game.currentMode) {
    case MODE_MAINMENU :
        drawMenu();
        break;
    case MODE_LEVEL :
        if(Game.specialMode)
            specialDrawLevel(&(Game.currentModeStruct->level),Game.specialState);
        else
            drawLevel(&(Game.currentModeStruct->level));
        break;
    default :
        break;
    }
}

void update(){
    int i,j;
    for(i=0;i<SDL_NumJoysticks();i++){
        for(j=0;j<SDL_JoystickNumBalls(Game.joysticks[i].joystick);j++){
            Game.joysticks[i].trackballs->x = 0;
            Game.joysticks[i].trackballs->y = 0;
        }
    }
    switch (Game.currentMode) {
    case MODE_LEVEL:
        if(Game.specialMode)
            Game.specialMode = specialUpdateLevel(&Game.currentModeStruct->level,&Game.specialState);
        else
            updateLevel(&Game.currentModeStruct->level);
        break;
    case MODE_MAINMENU:
        //updateMenu(Game.currentModeStruct->menu);
    default :
        break;
    }

}
int handleEvent(const SDL_Event* event){
    if(event->type == SDL_QUIT) return 0;
    if(event->type == SDL_KEYUP){
        if(event->key.keysym.sym == SDLK_F4)
            if(event->key.keysym.mod == KMOD_LALT || event->key.keysym.mod == KMOD_RALT)
            {
                return 0;
            }
    }
    if(windowEventHandler(event)) return 1;
    if(handleJoystickEvent(Game.joysticks,event)) return 1;
    switch (Game.currentMode) {
    case MODE_LEVEL:
        handleEventLevel(&(Game.currentModeStruct->level),event);
        break;
    case MODE_MAINMENU:
        //handleEventMenu(Game.currentModeStruct->menu,event);
    default :
        break;
    }

    return 1;
}

void initGameAudio(){
    initialize_audio(AUDIO_S16SYS,22050,2,512);
    Game.audioIDs[MAINAUDIO1] = makeAudio("sea_theme.wav",1,10);
    Game.audioIDs[MAINAUDIO2] = makeAudio("stardust_theme.wav",0,150);
    Game.audioIDs[ACCAUDIO] = makeAudio("hovercraft.wav",1,50);
}

void initGameTextures(){
    glGenTextures(NBTEXTURES, Game.textureIDs);
    makeTexture(Game.textureIDs[ONETEXTURE],"images/un.png",GL_RGBA);
    makeTexture(Game.textureIDs[TWOTEXTURE],"images/deux.png",GL_RGBA);
    makeTexture(Game.textureIDs[THREETEXTURE],"images/trois.png",GL_RGBA);
}

void initializeGame(){

    Game.windowWidth = 640;
    Game.windowHeight = 480;
    Game.fullscreen = 0;
    Game.currentMode = MODE_LEVEL;
    Game.currentModeStruct = malloc(sizeof(ModeStruct));
    int i = 0;
    int max =SDL_NumJoysticks()>NBJOYSTICK?NBJOYSTICK:SDL_NumJoysticks();
    for(;i<max;i++){
        initJoystick(Game.joysticks+i,i);
    }

    loadConfig();
    initialize_window(Game.windowWidth,Game.windowHeight,Game.fullscreen);
    initGameAudio();
    initGameTextures();
    playAudioFadeIn(Game.audioIDs[MAINAUDIO2],0.1);
    SDL_PauseAudio(0);
    initLevel(&Game.currentModeStruct->level,1,1);
    Game.specialState = 0;
    Game.specialMode = 1;
}



void game(){
    SDL_Event event;
    int loop = 1;
    while (loop)
    {
        unsigned int startTime = SDL_GetTicks();
        while(SDL_PollEvent(&event) && loop)
        {
            loop = handleEvent(&event);
        }
        update();
        glMatrixMode(GL_MODELVIEW);
        glClearColor(0,0,0,0);
        glClear(GL_COLOR_BUFFER_BIT);
        drawGame();
        SDL_GL_SwapBuffers();
        unsigned int delay = SDL_GetTicks() - startTime;
        if(delay < MILLISECOND_PER_FRAME)
        {
            SDL_Delay(MILLISECOND_PER_FRAME - delay);
        }
    }
}
void loadConfig(){

}
void saveConfig(){

}
