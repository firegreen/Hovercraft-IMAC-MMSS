#include "game.h"
#include "SDL_tools.h"
#include "GUI/level.h"
#include "textures.h"
#include "audios.h"
#include "Object2D/hovercraft.h"
#include "GUI/menu.h"
#include <stdio.h>
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


void drawGame()
{
    int i=0;
    if(Game.transitionMode)
    {
        loadCustomViewport(window.width*Game.specialState*-Game.transitionMode,0,window.width,window.height);
        glClearColor(0,0,0,0);
        glClear(GL_COLOR_BUFFER_BIT);
        switch(Game.currentMode)
        {
        case MODE_MAINMENU :
        case MODE_CREDITS:
        case MODE_INSTRUCTIONS:
        case MODE_ONEPLAYER:
        case MODE_MULTIPLAYERS:
            if(Game.nextMode==MODE_LEVEL){
                if(Game.specialMode)
                    specialdrawMenu(&Game.currentModeStruct->menu,Game.specialState,
                                    window.width*Game.specialState*Game.transitionMode);
                else
                    drawMenu(&Game.currentModeStruct->menu,window.width*Game.specialState*Game.transitionMode);
            }
            else{
                drawTwoMenu(&Game.currentModeStruct->menu,&Game.modeStructNext->menu,
                            window.width*Game.specialState*-Game.transitionMode,
                            (Game.transitionMode<0?-window.width:window.width)
                            -window.width*Game.specialState*Game.transitionMode);
                return;
            }
            break;
        case MODE_LEVEL :
            for(i=0;i<Game.currentModeStruct->level.nbPlayers;i++)
            {
                Game.currentModeStruct->level.players[i].view.viewportleftTop.x
                        +=window.width*Game.specialState*-Game.transitionMode;
            }
            if(Game.specialMode)
                specialDrawLevel(&(Game.currentModeStruct->level),101);
            else
                drawLevel(&(Game.currentModeStruct->level));
            break;
            for(i=0;i<Game.currentModeStruct->level.nbPlayers;i++)
            {
                Game.currentModeStruct->level.players[i].view.viewportleftTop.x
                        -=window.width*Game.specialState*-Game.transitionMode;
            }
        default :
            break;
        }
        loadCustomViewport((Game.transitionMode<0?0:window.width)
                           -window.width*Game.specialState*Game.transitionMode,0,window.width,window.height);
        switch(Game.nextMode)
        {
        case MODE_MAINMENU :
        case MODE_CREDITS:
        case MODE_INSTRUCTIONS:
        case MODE_ONEPLAYER:
        case MODE_MULTIPLAYERS:
            if(Game.specialMode){
                specialdrawMenu(&Game.modeStructNext->menu,0,
                                (Game.transitionMode<0?0:window.width)
                                -window.width*Game.specialState*Game.transitionMode);
            }
            else{
                drawMenu(&Game.modeStructNext->menu,
                         (Game.transitionMode<0?0:window.width)
                         -window.width*Game.specialState*Game.transitionMode);
            }
            break;
        case MODE_LEVEL :
            for(i=0;i<Game.modeStructNext->level.nbPlayers;i++)
            {
                Game.modeStructNext->level.players[i].view.viewportleftTop.x
                        +=(Game.transitionMode<0?0:window.width)
                        -window.width*Game.specialState*Game.transitionMode;
            }
            if(Game.specialMode)
                specialDrawLevel(&(Game.modeStructNext->level),0);
            else
                drawLevel(&(Game.modeStructNext->level));
            for(i=0;i<Game.modeStructNext->level.nbPlayers;i++)
            {
                Game.modeStructNext->level.players[i].view.viewportleftTop.x
                        -=(Game.transitionMode<0?0:window.width)
                        -window.width*Game.specialState*Game.transitionMode;
            }
            break;
        default :
            break;
        }
    }
    else
    {
        reloadGLrepere();
        glClearColor(0,0,0,0);
        glClear(GL_COLOR_BUFFER_BIT);
        switch (Game.currentMode)
        {
        case MODE_MAINMENU :
        case MODE_CREDITS:
        case MODE_INSTRUCTIONS:
        case MODE_ONEPLAYER:
        case MODE_MULTIPLAYERS:
            if(Game.specialMode){
                specialdrawMenu(&Game.currentModeStruct->menu,Game.specialState,0);
            }
            else{
                drawMenu(&Game.currentModeStruct->menu,0);
            }
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
}

void update()
{
    if(Game.transitionMode)
    {
        Game.specialState += Game.transitionSpeed;
        if(Game.transitionSpeed>0.01)
            Game.transitionSpeed -= Game.transitionSpeed*0.1;
        Game.transitionMode = Game.specialState<1;
        if(!Game.transitionMode){
            if(Game.currentMode>= MODE_MAINMENU && Game.currentMode<=MODE_CREDITS &&
                  Game.nextMode>= MODE_MAINMENU && Game.nextMode<=MODE_CREDITS){
                Game.currentModeStruct->menu.buttons = Game.modeStructNext->menu.buttons;
                Game.currentModeStruct->menu.imageTextureID = Game.modeStructNext->menu.imageTextureID;
                Game.currentModeStruct->menu.titleTextureID = Game.modeStructNext->menu.titleTextureID;
                Game.currentModeStruct->menu.nbButtons = Game.modeStructNext->menu.nbButtons;
            }
            else{
                if(Game.currentMode>= MODE_MAINMENU && Game.currentMode<=MODE_CREDITS)
                {
                    freeMap2(&Game.currentModeStruct->menu.map);
                    free(Game.currentModeStruct->menu.buttons);
                }
                (*Game.currentModeStruct) = (*Game.modeStructNext);
            }
            Game.specialState=0;
            Game.currentMode= Game.nextMode;
        }
    }
    else
    {
        switch (Game.currentMode)
        {
        case MODE_LEVEL:
            if(Game.specialMode)
                Game.specialMode = specialUpdateLevel(&Game.currentModeStruct->level,&Game.specialState);
            else
                updateLevel(&Game.currentModeStruct->level);
            break;
        case MODE_MAINMENU :
        case MODE_CREDITS:
        case MODE_INSTRUCTIONS:
        case MODE_ONEPLAYER:
        case MODE_MULTIPLAYERS:
            if(Game.specialMode)
                Game.specialMode = specialUpdateMenu(&Game.currentModeStruct->menu,
                                                     &Game.specialState);
            else
                updateMenu(&Game.currentModeStruct->menu);
            break;
        default :
            break;
        }
    }
}
int handleEvent(const SDL_Event* event)
{
    if(event->type == SDL_QUIT) return 0;
    /*if(event->type == SDL_KEYUP)
    {
        if(event->key.keysym.sym == SDLK_F4)
            if(event->key.keysym.mod == KMOD_LALT || event->key.keysym.mod == KMOD_RALT)
            {
                return 0;
            }
    }*/
    windowEventHandler(event);
    if(Game.transitionMode) return 1;
    switch (Game.currentMode)
    {
    case MODE_LEVEL:
        handleEventLevel(&(Game.currentModeStruct->level),event);
        break;
    case MODE_MAINMENU :
    case MODE_CREDITS:
    case MODE_INSTRUCTIONS:
    case MODE_ONEPLAYER:
    case MODE_MULTIPLAYERS:
        handleEventMenu(&(Game.currentModeStruct->menu), event);
        break;
    default :
        break;
    }
    return 1;
}

void initGameAudio(){
    initAudios(AUDIO_S16SYS,22050,2,512);
}

void initializeGame()
{
    Game.windowWidth = 640;
    Game.windowHeight = 480;
    Game.fullscreen = 0;
    Game.currentMode = MODE_MAINMENU;
    Game.currentModeStruct = malloc(sizeof(ModeStruct));
    Game.modeStructNext = malloc(sizeof(ModeStruct));
    loadConfig();
    initialize_window(Game.windowWidth,Game.windowHeight,Game.fullscreen);
    initGameAudio();
    initTextures();
    initControls();
    int i = 0;
    int max =SDL_NumJoysticks()>NBJOYSTICK?NBJOYSTICK:SDL_NumJoysticks();
    for(; i<max; i++)
    {
        initJoystick(Game.joysticks+i,i);
    }
    SDL_PauseAudio(0);
    initMainMenu(&(Game.currentModeStruct->menu));
    //initLevel(&Game.currentModeStruct->level,2,1,3);
    Game.specialMode = 1;
    Game.specialState = 0;
    Game.transitionMode = 0;
}

void game()
{
    SDL_Event event;
    int loop = 1;
    while (loop)
    {
        unsigned int startTime = SDL_GetTicks();
        while(SDL_PollEvent(&event) && loop)
        {
            if(event.type==SDL_KEYDOWN)
                printf("%d %d\n",event.key.keysym.sym, event.key.state);
            loop = handleEvent(&event);
        }
        update();
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

void transitionLeftMode(Mode nextMode){
    Game.nextMode = nextMode;
    switch(nextMode){
    case MODE_LEVEL:
        Game.specialMode = 1;
        break;
    case MODE_MAINMENU:
        initMainMenu(&Game.modeStructNext->menu);
        break;
    case MODE_MULTIPLAYERS:
        initMultiPlayers(&Game.modeStructNext->menu);
        break;
    case MODE_ONEPLAYER:
        initOnePlayer(&Game.modeStructNext->menu);
        break;
    case MODE_INSTRUCTIONS:
        initInstruction(&Game.modeStructNext->menu);
        break;
    case MODE_CREDITS:
        initCredits(&Game.modeStructNext->menu);
        break;
    default:
        break;
    }
    Game.transitionMode = 1;
    Game.specialState = 0;
    Game.transitionSpeed = 0.1;
}
void transitionRightMode(Mode nextMode){
    Game.nextMode = nextMode;
    switch(nextMode){
    case MODE_LEVEL:
        Game.specialMode = 1;
        break;
    case MODE_MAINMENU:
        initMainMenu(&Game.modeStructNext->menu);
        break;
    case MODE_MULTIPLAYERS:
        initMultiPlayers(&Game.modeStructNext->menu);
        break;
    case MODE_ONEPLAYER:
        initOnePlayer(&Game.modeStructNext->menu);
        break;
    case MODE_INSTRUCTIONS:
        initInstruction(&Game.modeStructNext->menu);
        break;
    case MODE_CREDITS:
        initCredits(&Game.modeStructNext->menu);
        break;
    default:
        break;
    }
    Game.transitionMode = -1;
    Game.specialState = 0;
    Game.transitionSpeed = 0.1;
}
