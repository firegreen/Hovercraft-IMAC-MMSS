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
#include "audios.h"
#include "Mapping/map.h"

const Color4f BLACK = {0,0,0,1};
const Color4f BTNPBACK = {0.1,0.2,0.6,0.7};
const Color4f BTNFORE = {0.6,1,0.8,1};
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
    transition(MODE_ONEPLAYER);
}
void credits(){
    transition(MODE_CREDITS);
}
void goback(){
    transition(MODE_MAINMENU);
}
void instructions(){
    transition(MODE_INSTRUCTIONS);
}
void quitter(){exit(0);}

void map1(){
    initLevel(&Game.currentModeStruct->level,1);
    transition(MODE_LEVEL);
}
void map2(){
    initLevel(&Game.currentModeStruct->level,2);
    transition(MODE_LEVEL);
}

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
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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
    glDisable(GL_BLEND);

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
            Color4f fore = makeColor4f(b->fore.r+0.4,b->fore.g+0.6,b->fore.b+0.6,b->fore.a+0.2);
            privateDrawButton(b,&fore,&b->back);
        }
        else{
            privateDrawButton(b,&b->fore,&b->back);
        }
    }
}



/***********************************

        MENUS

***********************************/
void initMenu(Menu* m, const char* titlefilename, int nbButtons, const char *imagefilename){
    int width,height;
    if(titlefilename!=NULL){
        glGenTextures(1,&m->titleTextureID);
        makeTexture(m->titleTextureID,titlefilename,&width,&height);
    }
    else
        m->titleTextureID=0;
    if(imagefilename!=NULL){
        glGenTextures(1,&m->imageTextureID);
        makeTexture(m->imageTextureID,imagefilename,&width,&height);
    }
    else
        m->imageTextureID=0;
    m->buttons = malloc(sizeof(Button)*nbButtons);
    m->nbButtons=nbButtons;
    playAudioFadeIn(2,1); // put the music id
}

void drawMenu(const Menu* m){
    int i;
    Bounds2P bTitle, bImage;
    for(i=0;i<m->nbButtons;i++){
        drawButton(&(m->buttons[i]));
    }
    bImage = makeBounds2P(-0.8*window.orthoGLX,0.7*window.orthoGLY,
                          0.8*window.orthoGLX,-0.7*window.orthoGLY);
    bTitle = makeBounds2P(-0.7*window.orthoGLX,0.7*window.orthoGLY,
                          0.7*window.orthoGLX,0.2*window.orthoGLY);
    if(m->titleTextureID)
        drawTextureQuad(m->titleTextureID,&WHITE,&bTitle);
    if(m->imageTextureID)
        drawTextureQuad(m->imageTextureID,&WHITE,&bImage);
}

void updateMenu(Menu *m){

}

void handleEventMenu(Menu *m, const SDL_Event *event){
    if(event->type == SDL_MOUSEBUTTONUP){
        int i;
        float xGL = (event->button.x)*2*(window.orthoGLX)/window.width-window.orthoGLX;
        float yGL = -((event->button.y)*2*(window.orthoGLY)/window.height)+window.orthoGLY;
        for (i=0; i<m->nbButtons; i++){
        if (m->buttons[i].bounds.x1<xGL && xGL<= m->buttons[i].bounds.x2 &&
            m->buttons[i].bounds.y1<yGL && yGL<= m->buttons[i].bounds.y2)
            {
                (*m->buttons[i].clickHandle)();
                return;
            }
        }
    }
    if(event->type == SDL_MOUSEMOTION){
        int i;
        float xGL = (event->motion.x)*2*(window.orthoGLX)/window.width-window.orthoGLX;
        float yGL = -((event->motion.y)*2*(window.orthoGLY)/window.height)+window.orthoGLY;
        for (i=0; i<m->nbButtons; i++){
            if (m->buttons[i].bounds.x1<xGL && xGL<= m->buttons[i].bounds.x2 &&
                m->buttons[i].bounds.y1<yGL && yGL<= m->buttons[i].bounds.y2)
                {
                    m->buttons[i].hover=1;
                }
            else{
                m->buttons[i].hover=0;
             }
         }
      }
}


void initMainMenu(Menu* m){
    initMenu(m,"images/titre.png",MAINMENUNBBUTTONS,NULL);
    m->buttons[ONEPLAYERS]= makeButton("1 Joueur",makeBounds6F(-40.0,-14.0,80.0,13.0),BTNFORE,BTNPBACK,un_joueur);
    m->buttons[INSTRUCTIONS]= makeButton("Instructions",makeBounds6F(-40.0,-28.0,80.0,13.0),BTNFORE,BTNPBACK,instructions);
    m->buttons[CREDITS]= makeButton("Credits",makeBounds6F(-40.0,-42.0,80.0,13.0),BTNFORE,BTNPBACK,credits);
    m->buttons[EXIT_GAME]= makeButton("Quitter",makeBounds6F(-40.0,-56.0,80.0,13.0),BTNFORE,BTNPBACK,quitter);
}


void initOnePlayer(Menu *menuOP){
    initMenu(menuOP,"images/niveau.png",LEVELNBBUTTONS,NULL);
    menuOP->buttons[LEVEL1]= makeButton("Niveau 1",makeBounds6F(-40.0,0.0,80.0,13.0),BTNFORE,BTNPBACK,map1);
    menuOP->buttons[LEVEL2]= makeButton("Niveau 2",makeBounds6F(-40.0,-14.0,80.0,13.0),BTNFORE,BTNPBACK,map2);
    menuOP->buttons[RETOUR]= makeButton("Retour",makeBounds6F(-40.0,-42.0,80.0,13.0),BTNFORE,BTNPBACK,goback);
}

void initInstruction(Menu *menuI){
    initMenu(menuI,NULL,1,"images/instructions.png");
    menuI->buttons[0]= makeButton("Retour",makeBounds6F(-40.0,-window.orthoGLY*0.95,80.0,window.orthoGLY*0.15),BTNFORE,BTNPBACK,goback);
}

void initCredits(Menu* menuC){
    initMenu(menuC,NULL,1,"images/credits.png");
    menuC->buttons[0]= makeButton("Retour",makeBounds6F(-40.0,-window.orthoGLY*0.95,80.0,window.orthoGLY*0.15),BTNFORE,BTNPBACK,goback);
}
