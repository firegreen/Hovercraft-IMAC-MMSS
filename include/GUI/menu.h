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

typedef enum MAINMENUBTN {ONEPLAYERS,TWOPLAYERS,INSTRUCTIONS,CREDITS,EXIT_GAME,MAINMENUNBBUTTONS} MAINMENUBTN;
typedef enum LEVELBTN {LEVEL1,LEVEL2,RETOUR,LEVELNBBUTTONS} LEVELBTN;
typedef enum JOUEURS {JOUEURSNB2_1,JOUEURSNB2_2,JOUEURSNB3_1,JOUEURSNB3_2,
                      JOUEURSNB4_1,JOUEURSNB4_2,RETOURMULTI, MULTINBBUTTONS} MULTIBTN;
typedef enum Mode {MODE_MAINMENU=0, MODE_ONEPLAYER, MODE_MULTIPLAYERS,
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
    int indice;
    int nbBots;
    BotHovercraft* bots;
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

void drawMenu(const Menu *m, int viewportX);
void specialdrawMenu(const Menu *m, float state, int viewportX);
int specialUpdateMenu(const Menu *m, float *state);
void drawTwoMenu(const Menu *m, const Menu *m2, int viewportX1, int viewportX2);
void updateMenu(Menu *m);
Button makeButton(char *label, Bounds6F bounds, Color4f fore, Color4f back, void (*clickHandle)(void));
void vBitmapOutput(int x, int y, char *string, void *font);
void handleEventMenu(Menu *m, const SDL_Event *event);

#endif
