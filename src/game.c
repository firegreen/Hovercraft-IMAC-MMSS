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
        drawMenu(Game.currentModeStruct->menu);
        break;
    case MODE_LEVEL :
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
        updateLevel(&(Game.currentModeStruct->level));
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

void initializeGame(){
    char fakeParam[] = "fake";
    char *fakeargv[] = { fakeParam, NULL };
    int fakeargc = 1;
    glutInit( &fakeargc, fakeargv);
    Game.windowWidth = 640;
    Game.windowHeight = 480;
    Game.fullscreen = 0;
    Game.currentMode = MODE_MAINMENU;
    Game.currentModeStruct = malloc(sizeof(ModeStruct));
    Game.currentModeStruct->menu=initMainMenu();/*
    Game.currentModeStruct->level.players = malloc(1*sizeof(Hovercraft));
    Game.currentModeStruct->level.nbPlayers=1;
    initHovercraft(Game.currentModeStruct->level.players);*/
    Object* o = malloc(sizeof(Object));
    makeObject(o,1,1,1,3,1,0,0);
    o->effectDelays[0]=1;
    Effect e;
    e.rebound.resistance = 40;
    e.rebound.rebound_value = 1;
    o->effectsAtCollision[0]=e;
    o->effectsTypesAtCollision[0]=POINTSPLUS;
    makeCircle(o->shapes,3,makePoint(0,0));
    o->x = -10; o->y = 10;

    Object* o2 = malloc(sizeof(Object));
    makeObject(o2,1,1,1,3,1,0,0);
    o2->effectDelays[0]=1;
    o2->effectsAtCollision[0]=e;
    o2->effectsTypesAtCollision[0]=REBOUND;
    makeCircle(o2->shapes,3,makePoint(0,0));
    o2->x = 0; o2->y = -10;

    int i = 0;
    int max =SDL_NumJoysticks()>NBJOYSTICK?NBJOYSTICK:SDL_NumJoysticks();
    for(;i<max;i++){
        initJoystick(Game.joysticks+i,i);
    }
    loadConfig();
    initialize_window(Game.windowWidth,Game.windowHeight,Game.fullscreen);
    initGameAudio();
    playAudioFadeIn(Game.audioIDs[MAINAUDIO1],0.1);
    SDL_PauseAudio(0);/*
    initMap(&(Game.currentModeStruct->level.map),500,750,0.02);
    addObjectToMap(&(Game.currentModeStruct->level.map),o,makePoint(o->x,o->y));
    addObjectToMap(&(Game.currentModeStruct->level.map),o2,makePoint(o2->x,o2->y));*/

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
        glClearColor(1,1,1,1);
        glClear(GL_COLOR_BUFFER_BIT);
        glLoadIdentity();
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
