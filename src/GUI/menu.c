#include "GUI/menu.h"
#include "game.h"

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const Color4f BLACK = {0,0,0,1};
const Color4f BTNPBACK = {0.05,0.01,0.07,1};
const Color4f BTNFORE = {1,0.6,0.4,1};
const Color4f TRANSLUCIDE = {0.,0.,0.,0.};
const Color4f WHITE = {1,1,1,1};
const Color4f GREY = {0.5,0.5,0.5,1};

void vBitmapOutput(int x, int y, char *string, void *font){
    int len,i;
    glRasterPos2f(x,y);
    len = (int) strlen(string);
    for (i = 0; i < len; i++) glutBitmapCharacter(font,string[i]);
}

Menu* initMenu(const char* title, int nbButtons){
    Menu* menu = malloc(sizeof(Menu));
    menu->title = malloc(sizeof(char)*strlen(title)+1);
    strcpy(menu->title,title);
    menu->buttons = malloc(sizeof(Button)*nbButtons);
    return menu;
}

Menu* initMainMenu(){
    Menu* menu = initMenu("Menu Principal",MAINMENUNBBUTTONS);
    menu->buttons[ONEPLAYERS]= makeButton("1 Joueur",makeBounds6F(-40.0,10.0,80.0,13.0),BTNFORE,BTNPBACK,NULL);
    menu->buttons[TWOPLAYERS]= makeButton("2 Joueurs",makeBounds6F(-40.0,-4.0,80.0,13.0),BTNFORE,BTNPBACK,NULL);
    menu->buttons[INSTRUCTIONS]= makeButton("Instructions",makeBounds6F(-40.0,-18.0,80.0,13.0),BTNFORE,BTNPBACK,NULL);
    menu->buttons[CREDITS]= makeButton("Credits",makeBounds6F(-40.0,-32.0,80.0,13.0),BTNFORE,BTNPBACK,NULL);
    menu->buttons[EXIT_GAME]= makeButton("Quitter",makeBounds6F(-40.0,-46.0,80.0,13.0),BTNFORE,BTNPBACK,NULL);
    return menu;
}

Button makeButton(char *label, Bounds6F bounds, Color4f fore, Color4f back, void (*clickHandle)(void)){
    Button b;
    b.bounds = bounds;
    b.fore = fore;
    b.back = back;
    b.label = label;
    b.clickHandle = clickHandle;
    b.press = 0;
    b.hover = 0;
    b.inactiv = 0;
    return b;
}

void privateDrawButton(const Button* b,const Color4f* fore, const Color4f* back){
    glColor4f(back->r,back->g,back->b,back->a);
    glBegin(GL_QUADS);
    glVertex2f(b->bounds.x1,b->bounds.y1);
    glVertex2f(b->bounds.x2,b->bounds.y1);
    glVertex2f(b->bounds.x2,b->bounds.y2);
    glVertex2f(b->bounds.x1,b->bounds.y2);
    glEnd();
    glColor4f(fore->r,fore->g,fore->b,fore->a);
    glBegin(GL_LINE_LOOP);
    glVertex2f(b->bounds.x1,b->bounds.y1);
    glVertex2f(b->bounds.x2,b->bounds.y1);
    glVertex2f(b->bounds.x2,b->bounds.y2);
    glVertex2f(b->bounds.x1,b->bounds.y2);
    glEnd();
    vBitmapOutput(  -6 - strlen(b->label)/2,
                    b->bounds.y2 + b->bounds.height - 21,
                    b->label,
                    GLUT_BITMAP_8_BY_13);
}

void drawButton(Button* b){
    if(b->inactiv){
        Color4f fore = BLACK;
        Color4f back = GREY;
        privateDrawButton(b,&back,&fore);
    }
    else{
        if(b->press){
            privateDrawButton(b,&b->back,&b->fore);
        }
        else if(b->hover)
        {
            Color4f fore = makeColor4f(b->fore.r+0.2,b->fore.g+0.4,b->fore.b+0.4,b->fore.a);
            privateDrawButton(b,&fore,&b->back);
        }
        else{
            privateDrawButton(b,&b->fore,&b->back);
        }
    }
}

void drawMenu(const Menu* m){
    int i;
    glColor4f(1,1,1,1);
    glViewport(0,0,window.width,window.height);
    glBegin(GL_QUADS);
        glVertex2f(-window.width,-window.height);
        glVertex2f(window.width,-window.height);
        glVertex2f(window.width,window.height);
        glVertex2f(-window.width,window.height);
    glEnd();
    for(i=0;i<MAINMENUNBBUTTONS;i++){
        drawButton(&(m->buttons[i]));
    };
    glColor4f(0,0,1,1);


    vBitmapOutput(-20,50,m->title, GLUT_BITMAP_9_BY_15);



}
