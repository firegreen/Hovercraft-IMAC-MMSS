#include "game.h"
#include "SDL_tools.h"
#include "GUI/level.h"
#include "image_src.h"
#include <stdlib.h>
#include "Object2D/hovercraft.h"

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
        drawLevel(Game.currentLevel);
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
        updateLevel(Game.currentLevel);
        break;
    default :
        break;
    }

}
int handleEvent(const SDL_Event* event){
    if(windowEventHandler(event)) return 1;
    if(handleJoystickEvent(Game.joysticks,event)) return 1;
    switch(event->type)
    {
    case SDL_QUIT:
        return 0;
    case SDL_MOUSEMOTION:
        break;
    case SDL_MOUSEBUTTONUP:
        break;
    case SDL_KEYDOWN:
        if( event->button.button == SDL_BUTTON_WHEELDOWN)
        {
            Game.currentLevel->players[0].view.leftTop.x-=10;
            Game.currentLevel->players[0].view.rightBottom.x+=10;
            Game.currentLevel->players[0].view.leftTop.y+=10;
            Game.currentLevel->players[0].view.rightBottom.y-=10;
        }
        else if(event->button.button == SDL_BUTTON_WHEELUP){
            Game.currentLevel->players[0].view.leftTop.x+=10;
            Game.currentLevel->players[0].view.rightBottom.x-=10;
            Game.currentLevel->players[0].view.leftTop.y-=10;
            Game.currentLevel->players[0].view.rightBottom.y+=10;
        }
        switch(event->key.keysym.sym ){
        case SDLK_UP:
            Game.currentLevel->players[0].linearBoost = 1;
            playAudioFadeIn(Game.audioIDs[ACCAUDIO],0.8);
            break;
        case SDLK_DOWN:
            Game.currentLevel->players[0].linearBoost = -1./2.;
            break;
        case SDLK_LEFT:
            Game.currentLevel->players[0].rotationBoost =1;
            break;
        case SDLK_RIGHT:
            Game.currentLevel->players[0].rotationBoost =-1;
            break;
        case SDLK_p:
            if(event->key.keysym.mod == KMOD_LCTRL || event->key.keysym.mod == KMOD_RCTRL)
            {
                if(Game.currentLevel->players[0].view.rightBottom.x-
                        Game.currentLevel->players[0].view.leftTop.x > 40){
                    Game.currentLevel->players[0].view.leftTop.x+=10;
                    Game.currentLevel->players[0].view.rightBottom.x-=10;
                    Game.currentLevel->players[0].view.leftTop.y-=10;
                    Game.currentLevel->players[0].view.rightBottom.y+=10;
                    updateView(Game.currentLevel->players);
                }
            }
            break;
        case SDLK_m:
            if(event->key.keysym.mod == KMOD_LCTRL || event->key.keysym.mod == KMOD_RCTRL)
            {
                if(Game.currentLevel->players[0].view.rightBottom.x-
                        Game.currentLevel->players[0].view.leftTop.x < Game.currentLevel->map.width-20){
                    Game.currentLevel->players[0].view.leftTop.x-=10;
                    Game.currentLevel->players[0].view.rightBottom.x+=10;
                    Game.currentLevel->players[0].view.leftTop.y+=10;
                    Game.currentLevel->players[0].view.rightBottom.y-=10;
                    updateView(Game.currentLevel->players);
                }
            }
            break;
        default:
            break;
        }
        break;
    case SDL_KEYUP:
        switch( event->key.keysym.sym ){
        case SDLK_UP:
            stopAudioFadeOut(Game.audioIDs[ACCAUDIO],1.2);
            Game.currentLevel->players[0].linearBoost = 0;
            break;
        case SDLK_DOWN:
            Game.currentLevel->players[0].linearBoost = 0;
            break;
        case SDLK_LEFT:
            Game.currentLevel->players[0].rotationSpeed = 0;
            Game.currentLevel->players[0].rotationBoost = 0;
            break;
        case SDLK_RIGHT:
            Game.currentLevel->players[0].rotationSpeed = 0;
            Game.currentLevel->players[0].rotationBoost = 0;
            break;
        default:
            break;
        }
        break;
    case SDL_MOUSEBUTTONDOWN:
        break;
    default:
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

    Game.windowWidth = 640;
    Game.windowHeight = 480;
    Game.fullscreen = 0;
    Game.currentMode = MODE_LEVEL;
    Game.currentLevel = malloc(sizeof(Level));
    Game.currentLevel->players = malloc(1*sizeof(Hovercraft));
    Game.currentLevel->nbPlayers=1;
    initHovercraft(Game.currentLevel->players);
    Object* o = makeObject(1,1,1,1);
    o->effectDelays[0]=1;
    Effect e;
    e.acceleration.acceleration_value = 0.1;
    o->effectsAtCollision[0]=e;
    o->effectsTypesAtCollision[0]=ACCELERATION;
    makeCircle(o->shapes,3,makePoint(0,0));
    o->x = -10; o->y = 10;

    Object* o2 = makeObject(1,1,1,1);
    o2->effectDelays[0]=1;
    o2->effectsAtCollision[0]=e;
    o2->effectsTypesAtCollision[0]=ACCELERATION;
    makeCircle(o2->shapes,3,makePoint(0,0));
    o2->x = 0; o2->y = -10;

    int i = 0;
    int max =SDL_NumJoysticks()>NBJOYSTICK?NBJOYSTICK:SDL_NumJoysticks();
    for(;i<max;i++){
        initialiserJoystick(Game.joysticks+i,i);
    }
    loadConfig();
    initialize_window(Game.windowWidth,Game.windowHeight,Game.fullscreen);
    initGameAudio();
    playAudioFadeIn(Game.audioIDs[MAINAUDIO1],0.1);
    SDL_PauseAudio(0);
    initMap(&(Game.currentLevel->map),500,750);
    addObjectToMap(&(Game.currentLevel->map),o,makePoint(o->x,o->y));
    addObjectToMap(&(Game.currentLevel->map),o2,makePoint(o2->x,o2->y));

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
