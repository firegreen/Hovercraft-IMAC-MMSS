#include "game.h"
#include "SDL_tools.h"
#include "GUI/level.h"
#include "textures.h"
#include "audios.h"
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


void drawGame()
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
        drawMenu(&Game.currentModeStruct->menu);
        break;
    case MODE_LEVEL :
        drawLevel(&(Game.currentModeStruct->level));
        break;
    default :
        break;
    }
}

void update()
{
    switch (Game.currentMode)
    {
    case MODE_LEVEL:
        updateLevel(&Game.currentModeStruct->level);
        break;
    case MODE_MAINMENU :
    case MODE_CREDITS:
    case MODE_INSTRUCTIONS:
    case MODE_ONEPLAYER:
        updateMenu(&Game.currentModeStruct->menu);
        break;
    default :
        break;
    }
}
int handleEvent(const SDL_Event* event)
{
    if(event->type == SDL_QUIT) return 0;
    if(event->type == SDL_KEYUP)
    {
        if(event->key.keysym.sym == SDLK_F4)
            if(event->key.keysym.mod == KMOD_LALT || event->key.keysym.mod == KMOD_RALT)
            {
                return 0;
            }
    }
    windowEventHandler(event);
    switch (Game.currentMode)
    {
    case MODE_LEVEL:
        handleEventLevel(&(Game.currentModeStruct->level),event);
        break;
    case MODE_MAINMENU :
    case MODE_CREDITS:
    case MODE_INSTRUCTIONS:
    case MODE_ONEPLAYER:
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
    initialize_window(Game.windowWidth,Game.windowHeight,Game.fullscreen);
    initGameAudio();
    initTextures();
    int i = 0;
    int max =SDL_NumJoysticks()>NBJOYSTICK?NBJOYSTICK:SDL_NumJoysticks();
    for(; i<max; i++)
    {
        initJoystick(Game.joysticks+i,i);
    }
    SDL_PauseAudio(0);
    initMainMenu(&(Game.currentModeStruct->menu));
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

void transition(Mode nextMode){
    Game.currentMode = nextMode;
    switch(nextMode){
    case MODE_LEVEL:
        break;
    case MODE_MAINMENU:
        initMainMenu(&Game.currentModeStruct->menu);
        break;
    case MODE_ONEPLAYER:
        initOnePlayer(&Game.currentModeStruct->menu);
        break;
    case MODE_INSTRUCTIONS:
        initInstruction(&Game.currentModeStruct->menu);
        break;
    case MODE_CREDITS:
        initCredits(&Game.currentModeStruct->menu);
        break;
    default:
        break;
    }
}
