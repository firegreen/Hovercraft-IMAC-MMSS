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
    if(Game.transitionMode)
    {
        if(Game.specialState<1)
        {
            glPushMatrix();
            glTranslatef(-window.orthoGLX + window.orthoGLX*2*Game.specialState,0,0);
            switch(Game.nextMode)
            {
            case MODE_MAINMENU :
                drawMenu(&Game.ModeStructNext->menu);
                break;
            case MODE_LEVEL :
                if(Game.specialMode)
                    specialDrawLevel(&(Game.ModeStructNext->level),Game.specialState);
                else
                    drawLevel(&(Game.ModeStructNext->level));
                break;
            case MODE_ONEPLAYER :
                drawOnePlayer(&(Game.ModeStructNext->oneplayer));
                break;
            case MODE_TWOPLAYERS :
                drawTwoPlayers(&(Game.ModeStructNext->twoplayers));
                break;
            case MODE_INSTRUCTIONS :
                drawInstructions(&(Game.ModeStructNext->instruction));
                break;
            case MODE_CREDITS :
                drawCredits(&(Game.ModeStructNext->credit));
                break;
            default :
                break;
                glPopMatrix();
            }

            glPushMatrix();
            glTranslatef(window.orthoGLX*2*Game.specialState,0,0);
            switch(Game.currentMode)
            {
            case MODE_MAINMENU :
                drawMenu(&Game.currentModeStruct->menu);
                break;
            case MODE_LEVEL :
                if(Game.specialMode)
                    specialDrawLevel(&(Game.currentModeStruct->level),Game.specialState);
                else
                    drawLevel(&(Game.currentModeStruct->level));
                break;
            case MODE_ONEPLAYER :
                initOnePlayer(&(Game.currentModeStruct->oneplayer),"images/instructions.png",4);
                drawOnePlayer(&(Game.currentModeStruct->oneplayer));
                break;
            case MODE_TWOPLAYERS :
                initTwoPlayers(&(Game.currentModeStruct->twoplayers),"images/instructions.png",4);
                drawTwoPlayers(&(Game.currentModeStruct->twoplayers));
                break;
            case MODE_INSTRUCTIONS :
                initInstructions(&(Game.currentModeStruct->instruction),"images/instructions.png");
                drawInstructions(&(Game.currentModeStruct->instruction));
                break;
            case MODE_CREDITS :
                initCredits(&(Game.currentModeStruct->credit),"images/credits.png");
                drawCredits(&(Game.currentModeStruct->credit));
                break;
            default :
                glPopMatrix();
            }
        }
    }
    else
    {
        switch (Game.currentMode)
        {
        case MODE_MAINMENU :
            drawMenu(&Game.currentModeStruct->menu);
            break;
        case MODE_LEVEL :
            if(Game.specialMode)
                specialDrawLevel(&(Game.currentModeStruct->level),Game.specialState);
            else
                drawLevel(&(Game.currentModeStruct->level));
            break;
        case MODE_ONEPLAYER :
            initOnePlayer(&(Game.currentModeStruct->oneplayer),"images/instructions.png", 4);
            drawOnePlayer(&(Game.currentModeStruct->oneplayer));
            break;
        case MODE_TWOPLAYERS :
            initTwoPlayers(&(Game.currentModeStruct->twoplayers),"images/instructions.png", 4);
            drawTwoPlayers(&(Game.currentModeStruct->twoplayers));
            break;
        case MODE_INSTRUCTIONS :
            drawInstructions(&(Game.currentModeStruct->instruction));
            break;
        case MODE_CREDITS :
            initCredits(&(Game.currentModeStruct->credit),"images/credits.png");
            drawCredits(&(Game.currentModeStruct->credit));
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
        Game.specialState += 0.05;
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
        case MODE_MAINMENU:
            //updateMenu(Game.currentModeStruct->menu);
            break;
        case MODE_INSTRUCTIONS:

            break;
        default :
            break;
        }
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
    if(windowEventHandler(event)) return 1;
    switch (Game.currentMode)
    {
    case MODE_LEVEL:
        handleEventLevel(&(Game.currentModeStruct->level),event);
        break;
    case MODE_MAINMENU:
         handleEventMenu(&(Game.currentModeStruct->menu), event);
        break;
    case MODE_INSTRUCTIONS:
         handleEventGoBack(Game.currentModeStruct->instruction.retour, event);
         break;
    case MODE_CREDITS:
         handleEventGoBack(Game.currentModeStruct->credit.retour, event);
         break;
    case MODE_ONEPLAYER:
        handleEventGoBackPlayer(Game.currentModeStruct->oneplayer.buttons, event);
        break;
    case MODE_TWOPLAYERS:
        handleEventGoBackPlayer(Game.currentModeStruct->twoplayers.buttons, event);
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
    char fakeParam[] = "fake";
    char *fakeargv[] = { fakeParam, NULL };
    int fakeargc = 1;
    glutInit( &fakeargc, fakeargv);
    Game.windowWidth = 640;
    Game.windowHeight = 480;
    Game.fullscreen = 0;
    Game.currentMode = MODE_MAINMENU;
    Game.currentModeStruct = malloc(sizeof(ModeStruct));
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
    playAudioFadeIn(3,0.1);
    SDL_PauseAudio(0);
    initMainMenu(&(Game.currentModeStruct->menu));
    //initLevel(&Game.currentModeStruct->level,2,2,0);
    Game.specialMode = 0;
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
