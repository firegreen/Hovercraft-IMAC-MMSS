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
const Color4f DIALOGBACK = {0.5,0.7,0.55,1};
const Color4f IMAGIMPBACK = {0.05,0.01,0.07,1};
const Color4f DIALOGFORE = {0.02,0.1,0.05,1};
const Color4f IMAGIMPFORE = {0.3,0.6,0.4,1};
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
    menu->title = malloc(sizeof(char)*strlen(title));
    strcpy(menu->title,title);
    menu->buttons = malloc(sizeof(Button)*nbButtons);
}

Button makeButton(char *label, Bounds4P bounds, Color4f fore, Color4f back, void (*clickHandle)(void)){
    Button b;
    b.label = vBitmapOutput(0,50,label,GLUT_BITMAP_HELVETICA_18);
    b.bounds = bounds;
    b.fore = fore;
    b.back = back;
    b.clickHandle = clickHandle;
    b.press = 0;
    b.hover = 0;
    b.inactiv = 0;
    return b;
}

void drawMenu(Button* button){
  Menu.buttons[ONEPLAYERS]= makeButton("1 Joueur",makeBounds4P(-40.0,10.0,40.0,-2.0),IMAGIMPFORE,IMAGIMPBACK,quit);
  Menu.buttons[TWOPLAYERS]= makeButton("2 Joueurs",makeBounds4P(-40.0,-4.0,40.0,-16.0),IMAGIMPFORE,IMAGIMPBACK,quit);
  Menu.buttons[INSTRUCTIONS]= makeButton("Instructions",makeBounds4P(-40.0,-18.0,40.0,-30.0),IMAGIMPFORE,IMAGIMPBACK,quit);
  Menu.buttons[CREDITS]= makeButton("Crédits",makeBounds4P(-40.0,-32.0,40.0,-44.0),IMAGIMPFORE,IMAGIMPBACK,quit);
  Menu.buttons[EXIT_GAME]= makeButton("Quitter",makeBounds4P(-40.0,-46.0,40.0,-58.0),IMAGIMPFORE,IMAGIMPBACK,quit);
}

void privateDrawButton(const Button* b,const Color4f* fore, const Color4f* back){
    glColor4f(back->r,back->g,back->b,back->a);
    glBegin(GL_QUADS);
		glVertex2f(b->bounds.x,b->bounds.y);
		glVertex2f(b->bounds.x2,b->bounds.y);
		glVertex2f(b->bounds.x2,b->bounds.y2);
		glVertex2f(b->bounds.x,b->bounds.y2);
	glEnd();
    glColor4f(fore->r,fore->g,fore->b,fore->a);
    float width = (float)glutBitmapLength(GLUT_BITMAP_8_BY_13,(b->label))/(float)screen.width;
    float height = (float)glutBitmapWidth(GLUT_BITMAP_8_BY_13,'_') * 8./(13.*(float)screen.height);
    vBitmapOutput(b->bounds.x + b->bounds.width/2. - width/2., b->bounds.y + b->bounds.height/2. - height/2.,b->label);
}


Color makeColor(float r, float g, float b, float a){
    Color c; c.r=r;c.g=g;c.b=b;c.a=a;
    return c;
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
            Color4f fore = makeColor(b->fore.r+0.2,b->fore.g+0.4,b->fore.b+0.4,b->fore.a);
            privateDrawButton(b,&fore,&b->back);
        }
        else{
            privateDrawButton(b,&b->fore,&b->back);
        }
    }
}
