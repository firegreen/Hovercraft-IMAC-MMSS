#ifndef MENU_H
#define MENU_H

#include "Object2D/geometry.h"
#include "Object2D/Color.h"
#include "mode.h"
#include "Object2D/hovercraft.h"

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <SDL.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL/SDL.h>
#endif

typedef enum MAINMENUBTN {ONEPLAYERS,INSTRUCTIONS,CREDITS,EXIT_GAME,MAINMENUNBBUTTONS} MAINMENUBTN;
typedef enum LEVELBTN {LEVEL1,LEVEL2,RETOUR,LEVELNBBUTTONS} LEVELBTN;
typedef enum Mode {MODE_MAINMENU=0, MODE_ONEPLAYER,
                   MODE_INSTRUCTIONS, MODE_CREDITS, MODE_LEVEL, NBMODES} Mode;

typedef struct Button {
    char* label;
    Bounds6F bounds;
    Color4f fore;
    Color4f back;
    void (*clickHandle)(void);
    int hover;
    int press;
    int invisible;
    int inactiv;
} Button;

typedef struct Menu{
    Map map;
    Button* buttons;
    int nbButtons;
    GLuint titleTextureID;
    GLuint imageTextureID;
} Menu;


void initMenu(Menu* menu, const char* titlefilename, int nbButtons, const char* imagefilename);
void initMainMenu(Menu* menu);
void initCredits(Menu* menuC);
void initInstruction(Menu* menuI);
void initMultiPlayers(Menu* menuMP);
void initOnePlayer(Menu* menuOP);

void drawMenu(const Menu *m);
void updateMenu(Menu *m);
Button makeButton(char *label, Bounds6F bounds, Color4f fore, Color4f back, void (*clickHandle)(void));
void vBitmapOutput(int x, int y, char *string, void *font);
void handleEventMenu(Menu *m, const SDL_Event *event);

#endif
