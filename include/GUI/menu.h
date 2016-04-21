#include "Object2D/geometry.h"
#include "Object2D/Color.h"

#ifndef MENU_H
#define MENU_H

typedef enum nbButtons {ONEPLAYERS,TWOPLAYERS,INSTRUCTIONS,CREDITS,EXIT_GAME} nbButtons;

typedef struct Button {
    char* label;
    Bounds4P bounds;
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
void drawMenu();

#endif
