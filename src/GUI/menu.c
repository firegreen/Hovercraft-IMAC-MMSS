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
    transitionLeftMode(MODE_ONEPLAYER);
}
void deux_joueurs(){
    transitionLeftMode(MODE_MULTIPLAYERS);
}
void instructions(){
    transitionLeftMode(MODE_INSTRUCTIONS);
}

void credits(){
    transitionLeftMode(MODE_CREDITS);
    }
void goback(){
    transitionRightMode(MODE_MAINMENU);
}
void quitter(){exit(0);}

void deux_joueurs_1(){
    initLevel(&Game.modeStructNext->level,1, 2, rand()%(3));
    transitionLeftMode(MODE_LEVEL);
}
void deux_joueurs_2(){
    initLevel(&Game.modeStructNext->level,2, 2, rand()%(3));
    transitionLeftMode(MODE_LEVEL);
}
void trois_joueurs_1(){
    initLevel(&Game.modeStructNext->level,1, 3, rand()%(2));
    transitionLeftMode(MODE_LEVEL);
}
void trois_joueurs_2(){
    initLevel(&Game.modeStructNext->level,2, 3, rand()%(2));
    transitionLeftMode(MODE_LEVEL);
}
void quatre_joueurs_1(){
    initLevel(&Game.modeStructNext->level,1, 4, 0);
    transitionLeftMode(MODE_LEVEL);
}
void quatre_joueurs_2(){
    initLevel(&Game.modeStructNext->level,2, 4,0);
    transitionLeftMode(MODE_LEVEL);
}
void niveau1() {
    initLevel(&Game.modeStructNext->level,1, 1, (rand()%3)+1);
    transitionLeftMode(MODE_LEVEL);
}
void niveau2() {
    initLevel(&Game.modeStructNext->level,2, 1,(rand()%3)+1);
    transitionLeftMode(MODE_LEVEL);
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
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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

    char strfile[10] = "map/0";
    int error;
    readFile(&m->map,strfile,&error);
    if(error!=NONEERROR){
        fprintf(stderr,"Error while reading map %d, exit...\n",0);
        switch(error){
        case BADFORMATOBJECT:
            fprintf(stderr," format obstacle errone");
            break;
        case BADFORMATITEM:
            fprintf(stderr," format item errone");
            break;
        case BADFORMATSHAPE:
            fprintf(stderr," format forme errone");
            break;
        case BADFORMATMAP:
            fprintf(stderr," format map errone");
            break;
        case BADFORMATEFFECT:
            fprintf(stderr," format effet errone");
            break;
        case FILENOTEXIST:
            fprintf(stderr," fichier non existant");
        case EOFREACHED:
            fprintf(stderr," fin de fichier atteint");
            break;
        }
        fprintf(stderr,", exit...\n");
        exit(1);
    }
    int i;
    m->nbBots = 3;
    m->bots = malloc(sizeof(BotHovercraft)*m->nbBots);
    for(i=0;i<m->nbBots;i++){
        initBotHovercraft(m->bots+i,&m->map,0.002,makePoint(0,0),window.width,window.height);
        m->bots[i].h.physical_body.x += i*20;
        m->bots[i].h.physical_body.y += i*20;
        m->bots[i].h.physical_body.angle = i*0.5*180./3.14;
    }
    m->indice=0;
    playAudioFadeIn(m->map.audioID,1);
}

void drawMenu(const Menu* m, int viewportX){
    int i;
    Bounds2P bTitle, bImage;
    updateViewOfHovercraft(&m->bots[1].h);
    applyCameraTransform(&m->bots[1].h.view);
    drawMap(&m->map);
    for(i=0;i<m->nbBots;i++){
        drawHovercraft(&m->bots[i].h);
    }
    loadCustomViewport(viewportX,0,window.width,window.height);
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

void drawTwoMenu(const Menu *m, const Menu *m2, int viewportX1, int viewportX2){
    int i;
    Bounds2P bTitle, bImage;
    bImage = makeBounds2P(-0.8*window.orthoGLX,0.7*window.orthoGLY,
                          0.8*window.orthoGLX,-0.7*window.orthoGLY);
    bTitle = makeBounds2P(-0.7*window.orthoGLX,0.7*window.orthoGLY,
                          0.7*window.orthoGLX,0.2*window.orthoGLY);
    updateViewOfHovercraft(&m->bots[1].h);
    applyCameraTransform(&m->bots[1].h.view);
    drawMap(&m->map);
    for(i=0;i<m->nbBots;i++){
        drawHovercraft(&m->bots[i].h);
    }
    loadCustomViewport(viewportX1,0,window.width,window.height);
    for(i=0;i<m->nbButtons;i++){
        drawButton(&(m->buttons[i]));
    }
    if(m->titleTextureID)
        drawTextureQuad(m->titleTextureID,&WHITE,&bTitle);
    if(m->imageTextureID)
        drawTextureQuad(m->imageTextureID,&WHITE,&bImage);

    loadCustomViewport(viewportX2,0,window.width,window.height);
    for(i=0;i<m2->nbButtons;i++){
        drawButton(&(m2->buttons[i]));
    }
    if(m2->titleTextureID)
        drawTextureQuad(m2->titleTextureID,&WHITE,&bTitle);
    if(m2->imageTextureID)
        drawTextureQuad(m2->imageTextureID,&WHITE,&bImage);
}

void specialdrawMenu(const Menu *m, float state, int viewportX){
    Bounds2P b;
    b = makeBounds2P(-0.5*window.orthoGLX,0.5*window.orthoGLX,
                          0.5*window.orthoGLX,-0.5*window.orthoGLX);
    Color4f c;
    if(state<40)
        c = makeColor4f(1,1,1,state/40.);
    else if(state<60)
        c = makeColor4f(1,1,1,1);
    else if(state<90)
        c = makeColor4f(1,1,1,(90.-state)/40.);
    else{
        drawMap(&m->map);
        loadCustomViewport(-window.width*(100.-state)/10.,0,window.width,window.height);
        Bounds2P bTitle = makeBounds2P(-0.7*window.orthoGLX,0.7*window.orthoGLY,
                              0.7*window.orthoGLX,0.2*window.orthoGLY);
        if(m->titleTextureID)
            drawTextureQuad(m->titleTextureID,&WHITE,&bTitle);
        return;
    }
    drawTextureQuad(getTexture(37),&c,&b);
}

int specialUpdateMenu(const Menu *m, float *state){
    if(*state<90)
        *state +=0.5;
    else{
        *state +=0.09;
    }
    return *state<100;
}

void updateMenu(Menu *m){
    updateMap(&m->map);
    int i,j;
    Chained_Object* co;
    for(i=0;i<m->nbBots;i++){
        if(m->bots[i].path==NULL)
            giveGoalToBot(m->bots+i,makePoint((rand()%(int)(m->map.width*2))-m->map.width,
                                              (rand()%(int)(m->map.height*2))-m->map.height));
        applyFrottement(&(m->map), &(m->bots[i].h.physical_body));
        updateBotHovercraft(m->bots+i,m->map.objects,m->map.items);
        co = m->map.objects;
        while(co!=NULL){
            handleCollision(&(m->bots[i].h.physical_body), co->object);
            co = co->next;
        }
        for(j=i+1;j<m->nbBots;j++){
            handleCollision(&(m->bots[i].h.physical_body),&(m->bots[j].h.physical_body));
        }
    }
}

void handleEventMenu(Menu *m, const SDL_Event *event){
    if(event->type == SDL_VIDEORESIZE){
        m->bots[1].h.view.viewportWidth = event->resize.w;
        m->bots[1].h.view.viewportHeight=event->resize.h;
        return;
    }
    if(Game.specialMode) return;
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
    m->buttons[ONEPLAYERS]= makeButton("1 Joueur",makeBounds6F(-40.0,0.0,80.0,13.0),BTNFORE,BTNPBACK,un_joueur);
    m->buttons[TWOPLAYERS]= makeButton("Multijoueurs",makeBounds6F(-40.0,-14.0,80.0,13.0),BTNFORE,BTNPBACK,deux_joueurs);
    m->buttons[INSTRUCTIONS]= makeButton("Instructions",makeBounds6F(-40.0,-28.0,80.0,13.0),BTNFORE,BTNPBACK,instructions);
    m->buttons[CREDITS]= makeButton("Credits",makeBounds6F(-40.0,-42.0,80.0,13.0),BTNFORE,BTNPBACK,credits);
    m->buttons[EXIT_GAME]= makeButton("Quitter",makeBounds6F(-40.0,-56.0,80.0,13.0),BTNFORE,BTNPBACK,quitter);
}


void initOnePlayer(Menu *menuOP){
    initMenu(menuOP,"images/niveau.png",LEVELNBBUTTONS,NULL);
    menuOP->buttons[LEVEL1]= makeButton("Niveau 1",makeBounds6F(-40.0,0.0,80.0,13.0),BTNFORE,BTNPBACK,niveau1);
    menuOP->buttons[LEVEL2]= makeButton("Niveau 2",makeBounds6F(-40.0,-14.0,80.0,13.0),BTNFORE,BTNPBACK,niveau2);
    menuOP->buttons[RETOUR]= makeButton("Retour",makeBounds6F(-40.0,-42.0,80.0,13.0),BTNFORE,BTNPBACK,goback);
}

void initMultiPlayers(Menu *menuMP){
    initMenu(menuMP,"images/niveau.png",MULTINBBUTTONS,NULL);
    menuMP->buttons[JOUEURSNB2_1]= makeButton("2 Joueurs - Niveau 1",makeBounds6F(-65.0,0.0,60.0,13.0),BTNFORE,BTNPBACK,deux_joueurs_1);
    menuMP->buttons[JOUEURSNB2_2]= makeButton("2 Joueurs - Niveau 2",makeBounds6F(7.0,0.0,60.0,13.0),BTNFORE,BTNPBACK,deux_joueurs_2);
    menuMP->buttons[JOUEURSNB3_1]= makeButton("3 Joueurs - Niveau 1",makeBounds6F(-65.0,-14.0,60.0,13.0),BTNFORE,BTNPBACK,trois_joueurs_1);
    menuMP->buttons[JOUEURSNB3_2]= makeButton("3 Joueurs - Niveau 2",makeBounds6F(7.0,-14.0,60.0,13.0),BTNFORE,BTNPBACK,trois_joueurs_2);
    menuMP->buttons[JOUEURSNB4_1]= makeButton("4 Joueurs - Niveau 1",makeBounds6F(-65.0,-28.0,60.0,13.0),BTNFORE,BTNPBACK,quatre_joueurs_1);
    menuMP->buttons[JOUEURSNB4_2]= makeButton("4 Joueurs - Niveau 2",makeBounds6F(7.0,-28.0,60.0,13.0),BTNFORE,BTNPBACK,quatre_joueurs_2);
    menuMP->buttons[RETOURMULTI]= makeButton("Retour",makeBounds6F(-40.0,-42.0,80.0,13.0),BTNFORE,BTNPBACK,goback);
}

void initInstruction(Menu *menuI){
    initMenu(menuI,NULL,1,"images/instructions.png");
    menuI->buttons[0]= makeButton("Retour",makeBounds6F(-40.0,-window.orthoGLY*0.95,80.0,window.orthoGLY*0.15),BTNFORE,BTNPBACK,goback);
}

void initCredits(Menu* menuC){
    initMenu(menuC,NULL,1,"images/credits.png");
    menuC->buttons[0]= makeButton("Retour",makeBounds6F(-40.0,-window.orthoGLY*0.95,80.0,window.orthoGLY*0.15),BTNFORE,BTNPBACK,goback);
}
