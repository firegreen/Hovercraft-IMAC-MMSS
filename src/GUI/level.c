#include "GUI/level.h"
#include "Mapping/camera.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <SDL_tools.h>
#include "game.h"
#include "textures.h"
#include <stdlib.h>

Color4f white =  { 1, 1, 1, 1};
Color4f black =  { 0, 0, 0, 1};
Color4f red =  { 1, 0, 0, 1};
Color4f green =  { 0, 1, 0, 1};
Color4f blue =  { 0, 0, 1, 1};

void drawView(const Level* l, int currentHovercraft){
    if(currentHovercraft>=0){
        applyCameraTransform(&l->players[currentHovercraft].view);
    }
    else{
        if(l->indice<l->nbBots){
            updateViewOfHovercraft(&l->bots[0].h);
            applyCameraTransform(&l->bots[0].h.view);
        }
        else{
            Camera c;
            makeCamera(&c,l->map.width,l->map.height,&l->map,makePoint(0,0),window.width,window.height);
            applyCameraTransform(&c);
        }
    }
    drawMap(&l->map);
    int i;
    for(i=0;i<l->nbPlayers;i++)
        drawHovercraft(l->players+i);
    for(i=0;i<l->nbBots;i++){
        drawHovercraft(&l->bots[i].h);
    }
    reloadGLrepere();
}

void specialDrawView(const Level* l, int currentHovercraft, float state){
    if(state<20){
        float coef = (0.8 + state/100.);
        glPushMatrix();
        glScalef(coef,coef,1);
        drawView(l,currentHovercraft);
        glPopMatrix();
    }
    else if(state<50){
        drawView(l,currentHovercraft);
        Bounds2P b = makeBounds2P(l->players[currentHovercraft].view.leftTop.x,
                                   l->players[currentHovercraft].view.leftTop.y,
                                   l->players[currentHovercraft].view.rightBottom.x,
                                   l->players[currentHovercraft].view.rightBottom.y);
        glPushMatrix();
        if(state<30){
            glScalef(0.5+(state-20.)/100.,0.5+(state-20.)/100.,1);
            drawTextureQuad(getTexture(1), &white, &b);
        }
        else if(state<40){
            glScalef(0.5+(state-30.)/100.,0.5+(state-30.)/100.,1);
            drawTextureQuad(getTexture(2), &white, &b);
        }
        else{
            glScalef(0.5+(state-40.)/100.,0.5+(state-40.)/100.,1);
            drawTextureQuad(getTexture(3), &white, &b);
        }
        glPopMatrix();
    }
}

void specialDrawLevel(const Level *level, float state){
    if(state<50){
        int i,j,line=0;
        int width, height = window.height/((level->nbPlayers+1)/2);
        for(i=level->nbPlayers;i>0;i-=2){
            width = window.width/((i+1)%2+1);
            for(j=0;j<(i+1)%2+1;j++)
            {
                glViewport(width*j,height*line,width,height);
                glMatrixMode(GL_MODELVIEW);
                glLoadIdentity();
                specialDrawView(level, (level->nbPlayers-i)+j, state);
            }
            line++;
        }
    }
    else{

    }
}

int specialUpdateLevel(const Level *level, float *state){
    if(*state<20){
        (*state) +=0.75;
        return *state<50.;
    }
    else if(*state<50){
        (*state) +=0.25;
        return *state<50.;
    }
    else{
        (*state) +=0.5;
        return *state<100.;
    }
}

void initLevel(Level *level, int mapid, int nbplayers, int nbBots){
    char strfile[10];
    memset(strfile,0,10);
    strcpy(strfile,"map/");
    itoa(mapid,strfile+4,10);
    int error;
    readFile(&level->map,strfile,&error);
    if(error!=NONEERROR){
        fprintf(stderr,"Error while reading map %d, exit...\n",mapid);
        switch(error){
        case BADFORMATOBJECT:
            fprintf(stderr," format obstacle errone");
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
        case EOFREACHED:
            fprintf(stderr," fin de fichier atteint");
            break;

        }
        fprintf(stderr,", exit...\n");
        exit(1);
    }
    level->nbPlayers = nbplayers;
    level->players = malloc(sizeof(Hovercraft)*nbplayers);
    int i;
    switch (nbplayers) {
    case 1:
        initHovercraft(level->players,&level->map,1,makePoint(0,0),
                       window.width,window.height, getControls(CONTROLKEYBOARD1));
        break;
    case 2:
        initHovercraft(level->players,&level->map,1,makePoint(0,0),
                       window.width/2.,window.height, getControls(CONTROLKEYBOARD1));
        initHovercraft(level->players+1,&level->map,2,makePoint(window.width/2.,0),
                       window.width/2.,window.height, getControls(CONTROLKEYBOARD2));
        break;
    case 3:
        initHovercraft(level->players,&level->map,1,makePoint(0,window.height/2.),
                       window.width/2.,window.height/2, getControls(CONTROLKEYBOARD1));
        initHovercraft(level->players+1,&level->map,2,makePoint(window.width/2.,window.height/2.),
                       window.width/2.,window.height/2, getControls(CONTROLKEYBOARD2));
        initHovercraft(level->players+2,&level->map,3,makePoint(0,0),
                       window.width,window.height/2, getControls(CONTROLKEYBOARD3));
        break;
    case 4:
        initHovercraft(level->players,&level->map,1,makePoint(0,window.height/2),
                       window.width/2.,window.height/2, getControls(CONTROLKEYBOARD1));
        initHovercraft(level->players+1,&level->map,2,makePoint(window.width/2.,window.height/2),
                       window.width/2.,window.height/2, getControls(CONTROLKEYBOARD2));
        initHovercraft(level->players+2,&level->map,3,makePoint(0,0),
                       window.width/2,window.height/2, getControls(CONTROLKEYBOARD3));
        initHovercraft(level->players+3,&level->map,4,makePoint(window.width/2,0),
                       window.width/2,window.height/2, getControls(CONTROLMOUSE));
        break;
    default:
        break;
    }
    float angle=0;
    for(i=0;i<nbplayers;i++){
        level->players[i].physical_body.x += 5*cos(angle);
        level->players[i].physical_body.y += 5*sin(angle);
        level->players[i].physical_body.angle = angle*180./M_PI;
        angle+=M_PI/2.;
        updateViewOfHovercraft(level->players+i);
    }

    level->nbBots = nbBots;
    level->bots = malloc(sizeof(BotHovercraft)*nbBots);
    angle=nbplayers*M_PI/2.;
    for(i=0;i<nbBots;i++){
        initBotHovercraft(level->bots+i,&level->map,0.002,makePoint(0,0),window.width,window.height);
        level->bots[i].h.physical_body.x += 5*cos(angle);
        level->bots[i].h.physical_body.y += 5*sin(angle);
        level->bots[i].h.physical_body.angle = angle*180./M_PI;
        angle+=M_PI/2.;
        giveGoalToBot(level->bots+i,makePoint(200,200));
    }
    level->state=0;
    level->indice=0;
}

void drawLevel(const Level* level){
    int i;
    if(level->nbPlayers){
        for(i=0;i<level->nbPlayers;i++){
            drawView(level,i);
        }
    }
    else{
        drawView(level,-1);
    }
}

void updateLevel(Level* level){
    updateMap(&(level->map));
    int i =0,j;
    Chained_Object* co;
    for(;i<level->nbPlayers;i++){
        applyFrottement(&(level->map), &(level->players[i].physical_body));
        for(j=i+1;j<level->nbPlayers;j++){
            handleCollision(&(level->players[i].physical_body),&(level->players[j].physical_body));
        }
        co = level->map.objects;
        while(co!=NULL){
            handleCollision(&(level->players[i].physical_body), co->object);
            co = co->next;
        }
        updateHovercraft(level->players+i);
        updateViewOfHovercraft(level->players+i);
    }
    for(i=0;i<level->nbBots;i++){
        updateBotHovercraft(level->bots+i,level->map.objects);
        for(j=0;j<level->nbPlayers;j++){
            handleCollision(&(level->bots[i].h.physical_body),&(level->players[j].physical_body));
        }
        co = level->map.objects;
        while(co!=NULL){
            handleCollision(&(level->bots[i].h.physical_body), co->object);
            co = co->next;
        }
        for(j=i+1;j<level->nbBots;j++){
            handleCollision(&(level->bots[i].h.physical_body),&(level->bots[j].h.physical_body));
        }
    }
    level->state+=1;
    if(level->state%100==0){
        float randomvalue;
        randomvalue = (float)rand()/(float)RAND_MAX;
        level->indice = (level->nbBots+2) * randomvalue;
    }
}

void handleEventLevelResize(Level *l){
    switch (l->nbPlayers) {
    case 1:
        l->players[0].view.viewportWidth  = window.width;
        l->players[0].view.viewportHeight =window.height;
        break;
    case 2:
        l->players[0].view.viewportWidth  = window.width/2;
        l->players[0].view.viewportHeight =window.height;
        l->players[1].view.viewportleftTop.x  = window.width/2;
        l->players[1].view.viewportWidth  = window.width/2;
        l->players[1].view.viewportHeight =window.height;
        break;
    case 3:
        l->players[0].view.viewportWidth  = window.width/2;
        l->players[0].view.viewportHeight =window.height/2;
        l->players[1].view.viewportleftTop.x  = window.width/2;
        l->players[1].view.viewportWidth  = window.width/2;
        l->players[1].view.viewportHeight =window.height/2;
        l->players[2].view.viewportleftTop.y  = window.height/2;
        l->players[2].view.viewportWidth  = window.width;
        l->players[2].view.viewportHeight =window.height/2;
        break;
    case 4:
        l->players[0].view.viewportWidth  = window.width/2;
        l->players[0].view.viewportHeight =window.height/2;
        l->players[1].view.viewportleftTop.x  = window.width/2;
        l->players[1].view.viewportWidth  = window.width/2;
        l->players[1].view.viewportHeight =window.height/2;
        l->players[2].view.viewportleftTop.y  = window.height/2;
        l->players[2].view.viewportWidth  = window.width/2;
        l->players[2].view.viewportHeight =window.height/2;
        l->players[3].view.viewportleftTop.y  = window.height/2;
        l->players[3].view.viewportleftTop.x  = window.width/2;
        l->players[3].view.viewportWidth  = window.width/2;
        l->players[3].view.viewportHeight =window.height/2;

        break;
    default:
        break;
    }
}

void handleEventLevel(Level *l, const SDL_Event *event){
    int i;
    for(i=0;i<l->nbPlayers;i++){
        if(handleEventHovercraft(l->players+i,event))
            return;
    }
}
