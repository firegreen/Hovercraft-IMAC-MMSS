#ifndef MENU_H
#define MENU_H

#include "Object2D/geometry.h"
#include "Object2D/Color.h"
#include "mode.h"

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
typedef enum LEVELBTN {LEVEL1,LEVEL2,LEVEL3,RETOUR,LEVELNBBUTTONS} LEVELBTN;

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
    Button* buttons;
    GLuint titleTextureID;
} Menu;

typedef struct Instruction{
    Button* retour;
    GLuint titleTextureID;
} Instruction;

typedef struct Credits{
    Button* retour;
    GLuint titleTextureID;
} Credits;

typedef struct OnePlayer{
    Button* buttons;
    GLuint titleTextureID;
} OnePlayer;

typedef struct TwoPlayers{
    Button* buttons;
    GLuint titleTextureID;
} TwoPlayers;



void un_joueur();
void deux_joueurs();
void instructions();
void credits();
void quitter();
void goback();
void initMenu(Menu* menu, const char* titleFilename, int nbButtons);
void initMainMenu(Menu* menu);
void drawMenu(const Menu *m);
Button makeButton(char *label, Bounds6F bounds, Color4f fore, Color4f back, void (*clickHandle)(void));
void handleEventMenu(Menu *m, const SDL_Event *event);

void initOnePlayer(OnePlayer* op, const char* filename, int nbButtons);
void drawOnePlayer(const OnePlayer* tp);

void initTwoPlayers(TwoPlayers* tp, const char* filename, int nbButtons);
void drawTwoPlayers(const TwoPlayers* tp);

void initInstructions(Instruction* i, const char* filename);
void drawInstructions(const Instruction* i);

void initCredits(Credits* c, const char* filename);
void drawCredits(const Credits* c);

void handleEventGoBack(Button* retour, const SDL_Event* event);
void handleEventGoBackPlayer(Button* retour, const SDL_Event* event);

#endif
