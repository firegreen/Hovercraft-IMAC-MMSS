#include "Object2D/geometry.h"
#include "Object2D/Color.h"

#ifndef MENU_H
#define MENU_H

#include "Object2D/Color.h"
#include "Object2D/geometry.h"
typedef enum MAINMENUBTN {ONEPLAYERS,TWOPLAYERS,INSTRUCTIONS,CREDITS,EXIT_GAME,MAINMENUNBBUTTONS} MAINMENUBTN;


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
    char* title;
} Menu;

Menu* initMenu(const char* title, int nbButtons);
Menu* initMainMenu();
void drawMenu(const Menu *m);
Button makeButton(char *label, Bounds6F bounds, Color4f fore, Color4f back, void (*clickHandle)(void));

#endif
