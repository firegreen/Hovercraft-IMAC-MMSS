#include "GUI/menu.h"
#ifdef __APPLE__
#include <GLUT/glut.h>
#include <SDL.h>
#else
#include <GL/glut.h>
#include <SDL/SDL.h>
#endif
#include "SDL_tools.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "textures.h"
#include "game.h"

const Color4f BLACK = {0,0,0,1};
const Color4f BTNPBACK = {0.5,0.1,0.7,1};
const Color4f BTNFORE = {1,0.6,0.8,1};
const Color4f TRANSLUCIDE = {0.,0.,0.,0.};
const Color4f WHITE = {1,1,1,1};
const Color4f GREY = {0.5,0.5,0.5,1};

void vBitmapOutput(int x, int y, char *string, void *font){
    int len,i;
    glRasterPos2f(x,y);
    len = (int) strlen(string);
    for (i = 0; i < len; i++) glutBitmapCharacter(font,string[i]);
}

void un_joueur(){
    free(Game.currentModeStruct->oneplayer.buttons);
    initOnePlayer(&(Game.currentModeStruct->oneplayer),"images/instructions.png",4);
    Game.currentMode = MODE_ONEPLAYER;
}
void deux_joueurs(){
    free(Game.currentModeStruct->twoplayers.buttons);
    initTwoPlayers(&(Game.currentModeStruct->twoplayers),"images/instructions.png",4);
    Game.currentMode = MODE_TWOPLAYERS;
}
void instructions(){
    free(Game.currentModeStruct->instruction.retour);
    initInstructions(&(Game.currentModeStruct->instruction),"images/instructions.png");
    Game.currentMode = MODE_INSTRUCTIONS;
}
void credits(){
    free(Game.currentModeStruct->credit.retour);
    initCredits(&(Game.currentModeStruct->credit),"images/instructions.png");
    Game.currentMode = MODE_CREDITS;
    }
void goback(){
    initMainMenu(&Game.currentModeStruct->menu);
    Game.currentMode = MODE_MAINMENU;
}
void quitter(){ printf("yolo");
exit(0);}

/**********************************

        BOUTONS

**********************************/

Button makeButton(char *label, Bounds6F bounds, Color4f fore, Color4f back, void (*clickHandle)(void)){
    Button b;
    b.label = malloc(sizeof(char)*strlen(label)+1);
    b.label[strlen(label)]=0;
    strcpy(b.label,label);
    b.bounds = bounds;
    b.fore = fore;
    b.back = back;
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

    float width = glutBitmapLength(GLUT_BITMAP_HELVETICA_18,(b->label))*(window.orthoGLX*2)/window.width;
    vBitmapOutput(b->bounds.x1 + b->bounds.width/2. - width/2.,
               b->bounds.y2 - b->bounds.height/2. ,b->label,GLUT_BITMAP_HELVETICA_18);


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



/***********************************

        MENU PRINCIPAL

***********************************/
void initMenu(Menu* m, const char* filename, int nbButtons){
    glGenTextures(1,&m->titleTextureID);
    int width,height;
    makeTexture(m->titleTextureID,filename,&width,&height);
    m->buttons = malloc(sizeof(Button)*nbButtons);
}

void initMainMenu(Menu* m){
    initMenu(m,"images/titre.png",MAINMENUNBBUTTONS);
    m->buttons[ONEPLAYERS]= makeButton("1 Joueur",makeBounds6F(-40.0,0.0,80.0,13.0),BTNFORE,BTNPBACK,un_joueur);
    m->buttons[TWOPLAYERS]= makeButton("2 Joueurs",makeBounds6F(-40.0,-14.0,80.0,13.0),BTNFORE,BTNPBACK,deux_joueurs);
    m->buttons[INSTRUCTIONS]= makeButton("Instructions",makeBounds6F(-40.0,-28.0,80.0,13.0),BTNFORE,BTNPBACK,instructions);
    m->buttons[CREDITS]= makeButton("Credits",makeBounds6F(-40.0,-42.0,80.0,13.0),BTNFORE,BTNPBACK,credits);
    m->buttons[EXIT_GAME]= makeButton("Quitter",makeBounds6F(-40.0,-56.0,80.0,13.0),BTNFORE,BTNPBACK,quitter);
}


void drawMenu(const Menu* m){
    int i;
    glColor4f(0,1,1,1);
    glViewport(0,0,window.width,window.height);
    glBegin(GL_QUADS);
        glVertex2f(-window.width,-window.height);
        glVertex2f(window.width,-window.height);
        glVertex2f(window.width,window.height);
        glVertex2f(-window.width,window.height);
    glEnd();
    for(i=0;i<MAINMENUNBBUTTONS;i++){
        drawButton(&(m->buttons[i]));
    }
    Bounds2P b = makeBounds2P(-50,50,60,20);
    drawTextureQuad(m->titleTextureID,&WHITE,&b);
}

void handleEventMenu(Menu *m, const SDL_Event *event){
    if(event->type == SDL_MOUSEBUTTONDOWN){
        int i;
        float xGL = (event->button.x)*2*(window.orthoGLX)/window.width-window.orthoGLX;
        float yGL = -((event->button.y)*2*(window.orthoGLY)/window.height)+window.orthoGLY;
        for (i=0; i<MAINMENUNBBUTTONS; i++){
        if (m->buttons[i].bounds.x1<xGL && xGL<= m->buttons[i].bounds.x2 &&
            m->buttons[i].bounds.y1<yGL && yGL<= m->buttons[i].bounds.y2)
            {
                (*m->buttons[i].clickHandle)();
            }
        }
    }
}
/****************************************

        1 JOUEUR

****************************************/

void initOnePlayer(OnePlayer* op, const char* filename, int nbButtons){
    glGenTextures(1,&op->titleTextureID);
    int width,height;
    makeTexture(op->titleTextureID,filename,&width,&height);
    op->buttons = malloc(sizeof(Button)*nbButtons);
    op->buttons[LEVEL1]= makeButton("Niveau 1",makeBounds6F(-40.0,0.0,80.0,13.0),BTNFORE,BTNPBACK,NULL);
    op->buttons[LEVEL2]= makeButton("Niveau 2",makeBounds6F(-40.0,-14.0,80.0,13.0),BTNFORE,BTNPBACK,NULL);
    op->buttons[LEVEL3]= makeButton("Niveau 3",makeBounds6F(-40.0,-28.0,80.0,13.0),BTNFORE,BTNPBACK,NULL);
    op->buttons[RETOUR]= makeButton("Retour",makeBounds6F(-40.0,-42.0,80.0,13.0),BTNFORE,BTNPBACK,goback);
}

void drawOnePlayer(const OnePlayer* op){
    int i;
    glColor4f(0,0,1,1);
    glViewport(0,0,window.width,window.height);
    glBegin(GL_QUADS);
        glVertex2f(-window.width,-window.height);
        glVertex2f(window.width,-window.height);
        glVertex2f(window.width,window.height);
        glVertex2f(-window.width,window.height);
    glEnd();
    for(i=0;i<LEVELNBBUTTONS;i++){
        drawButton(&(op->buttons[i]));
    }
}

/****************************************

        2 JOUEURS

****************************************/

void initTwoPlayers(TwoPlayers* tp, const char* filename, int nbButtons){
    glGenTextures(1,&tp->titleTextureID);
    int width,height;
    makeTexture(tp->titleTextureID,filename,&width,&height);
    tp->buttons = malloc(sizeof(Button)*nbButtons);
    tp->buttons[LEVEL1]= makeButton("Niveau 1",makeBounds6F(-40.0,0.0,80.0,13.0),BTNFORE,BTNPBACK,NULL);
    tp->buttons[LEVEL2]= makeButton("Niveau 2",makeBounds6F(-40.0,-14.0,80.0,13.0),BTNFORE,BTNPBACK,NULL);
    tp->buttons[LEVEL3]= makeButton("Niveau 3",makeBounds6F(-40.0,-28.0,80.0,13.0),BTNFORE,BTNPBACK,NULL);
    tp->buttons[RETOUR]= makeButton("Retour",makeBounds6F(-40.0,-42.0,80.0,13.0),BTNFORE,BTNPBACK,goback);
}


void drawTwoPlayers(const TwoPlayers* tp){
    int i;
    glColor4f(0,1,0,1);
    glViewport(0,0,window.width,window.height);
    glBegin(GL_QUADS);
        glVertex2f(-window.width,-window.height);
        glVertex2f(window.width,-window.height);
        glVertex2f(window.width,window.height);
        glVertex2f(-window.width,window.height);
    glEnd();
    for(i=0;i<LEVELNBBUTTONS;i++){
        drawButton(&(tp->buttons[i]));
    }
}

/****************************************

        INSTRUCTIONS

*****************************************/



void initInstructions(Instruction* i, const char* filename) {
    glGenTextures(1,&i->titleTextureID);
    int width,height;
    makeTexture(i->titleTextureID,filename,&width,&height);
    i->retour = malloc(sizeof(Button));
    i->retour[0]= makeButton("Retour",makeBounds6F(-40.0,0.0,80.0,13.0),BTNFORE,BTNPBACK,goback);
}


void drawInstructions(const Instruction* i) {
    glColor4f(1,1,1,1);
    glViewport(0,0,window.width,window.height);
    glBegin(GL_QUADS);
        glVertex2f(-window.width,-window.height);
        glVertex2f(window.width,-window.height);
        glVertex2f(window.width,window.height);
        glVertex2f(-window.width,window.height);
    glEnd();
    drawButton(&(i->retour[0]));
}


/***************************************

                CREDITS

***************************************/

void initCredits(Credits* c, const char* filename) {
    glGenTextures(1,&c->titleTextureID);
    int width,height;
    makeTexture(c->titleTextureID,filename,&width,&height);
    c->retour = malloc(sizeof(Button));
    c->retour[0]= makeButton("Retour",makeBounds6F(-40.0,-50.0,30.0,13.0),BTNFORE,BTNPBACK,goback);
}

void drawCredits(const Credits* c) {
    glColor4f(1,1,1,1);
    glViewport(0,0,window.width,window.height);
    glBegin(GL_QUADS);
        glVertex2f(-window.width,-window.height);
        glVertex2f(window.width,-window.height);
        glVertex2f(window.width,window.height);
        glVertex2f(-window.width,window.height);
    glEnd();
    drawButton(&(c->retour[0]));
}

/****************************************

        RETOUR

*****************************************/



void handleEventGoBack(Button* retour, const SDL_Event* event){
    if(event->type == SDL_MOUSEBUTTONDOWN){
        float xGL = (event->button.x)*2*(window.orthoGLX)/window.width-window.orthoGLX;
        float yGL = -((event->button.y)*2*(window.orthoGLY)/window.height)+window.orthoGLY;
        if (retour[0].bounds.x1<xGL && xGL<= retour[0].bounds.x2 &&
            retour[0].bounds.y1<yGL && yGL<= retour[0].bounds.y2)
            {
                (retour[0].clickHandle)();

            }
    }
}

void handleEventGoBackPlayer(Button* retour, const SDL_Event* event){
    if(event->type == SDL_MOUSEBUTTONDOWN){
        float xGL = (event->button.x)*2*(window.orthoGLX)/window.width-window.orthoGLX;
        float yGL = -((event->button.y)*2*(window.orthoGLY)/window.height)+window.orthoGLY;
        if (retour[RETOUR].bounds.x1<xGL && xGL<= retour[RETOUR].bounds.x2 &&
            retour[RETOUR].bounds.y1<yGL && yGL<= retour[RETOUR].bounds.y2)
            {
                (retour[RETOUR].clickHandle)();

            }
    }
}
