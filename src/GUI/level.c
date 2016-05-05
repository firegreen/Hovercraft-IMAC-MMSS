#include "GUI/level.h"
#include "Mapping/camera.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <SDL_tools.h>
#include "game.h"

Color4f white =  { 1, 1, 1, 1};
Color4f black =  { 0, 0, 0, 1};
Color4f red =  { 1, 0, 0, 1};
Color4f green =  { 0, 1, 0, 1};
Color4f blue =  { 0, 0, 1, 1};

void drawView(const Level* l, int currentHovercraft){
    applyCameraTransform(&l->players[currentHovercraft].view, -window.orthoGLX, -window.orthoGLY,
                         window.orthoGLX, window.orthoGLY);
    drawMap(&l->map);
    int i;
    for(i=0;i<l->nbPlayers;i++)
        drawHovercraft(l->players+i);
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
            drawTextureQuad(Game.textureIDs[ONETEXTURE], &white, &b);
        }
        else if(state<40){
            glScalef(0.5+(state-30.)/100.,0.5+(state-30.)/100.,1);
            drawTextureQuad(Game.textureIDs[TWOTEXTURE], &white, &b);
        }
        else{
            glScalef(0.5+(state-40.)/100.,0.5+(state-40.)/100.,1);
            drawTextureQuad(Game.textureIDs[THREETEXTURE], &white, &b);
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

void initLevel(Level *level, int mapid, int nbplayers){
    char strfile[10];
    memset(strfile,0,10);
    strcpy(strfile,"map/");
    itoa(mapid,strfile+4,10);
    int error;
    readFile(&level->map,strfile,&error);
    if(error!=NONEERROR){
        fprintf(stderr,"Error while reading map %d, exit...\n",mapid);
        exit(1);
    }
    level->nbPlayers = nbplayers;
    level->players = malloc(sizeof(Hovercraft)*nbplayers);
    int i;
    float angle=0;
    for(i=0;i<nbplayers;i++){
        initHovercraft(level->players+i,&level->map);
        level->players[i].physical_body.x += 5*cos(angle);
        level->players[i].physical_body.y += 5*sin(angle);
        level->players[i].physical_body.angle = angle*180./M_PI;
        angle+=M_PI/2.;
        updateViewOfHovercraft(level->players+i);
    }
}

void drawLevel(const Level* level){
    int i,j,line=0;
    int width, height = window.height/((level->nbPlayers+1)/2);
    for(i=level->nbPlayers;i>0;i-=2){
        width = window.width/((i+1)%2+1);
        for(j=0;j<(i+1)%2+1;j++)
        {
            glViewport(width*j,height*line,width,height);
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
            drawView(level, (level->nbPlayers-i)+j);
        }
        line++;
    }
}

void updateLevel(Level* level){
    updateMap(&(level->map));
    int i =0,j;
    Chained_Object* co;
    Chained_Object* coNext;
    for(;i<level->nbPlayers;i++){
        applyFrottement(&(level->map), &(level->players[i].physical_body));
        updateHovercraft(level->players+i);
        for(j=0;j<i;j++)
        {
            handleCollision(&(level->players[i].physical_body),&(level->players[j].physical_body));
        }
        for(j=i+1;j<level->nbPlayers;j++){
            handleCollision(&(level->players[i].physical_body),&(level->players[j].physical_body));
        }
        if(level->map.objects!=NULL){
            co = level->map.objects;
            while(co->next!=NULL){
                handleCollision(co->next->object,&(level->players[i].physical_body));
                if(co->next->object->life > 0){
                    updateObject(co->next->object);
                    co=co->next;
                }
                else{
                    coNext = co->next->next;
                    free(co->next);
                    co->next = coNext;
                }
            }
            handleCollision(level->map.objects->object,&(level->players[i].physical_body));
            if(level->map.objects->object->life > 0){
                updateObject(level->map.objects->object);
            }
            else{
                coNext = level->map.objects->next;
                free(level->map.objects);
                level->map.objects = coNext;
            }
        }
        updateViewOfHovercraft(level->players+i);
    }
    co = level->map.objects;
    while(co!=NULL){
        applyFrottement(&level->map,level->map.objects->object);
        co = co->next;
    }

}

void handleEventLevel(Level *l, const SDL_Event *event){
    switch(event->type)
    {
    case SDL_KEYDOWN:
        if( event->button.button == SDL_BUTTON_WHEELDOWN)
        {
            l->players[0].view.leftTop.x-=10;
            l->players[0].view.rightBottom.x+=10;
            l->players[0].view.leftTop.y+=10;
            l->players[0].view.rightBottom.y-=10;
        }
        else if(event->button.button == SDL_BUTTON_WHEELUP)
        {
            l->players[0].view.leftTop.x+=10;
            l->players[0].view.rightBottom.x-=10;
            l->players[0].view.leftTop.y-=10;
            l->players[0].view.rightBottom.y+=10;
        }
        switch(event->key.keysym.sym ){
        case SDLK_UP:
            l->players[0].linearAccelerate = 1;
            //playAudioFadeIn(Game.audioIDs[ACCAUDIO],0.8);
            break;
        case SDLK_DOWN:
            l->players[0].linearAccelerate = -1./2.;
            break;
        case SDLK_LEFT:
            l->players[0].rotationAccelerate =1;
            break;
        case SDLK_RIGHT:
            l->players[0].rotationAccelerate =-1;
            break;
        case SDLK_p:
            if(event->key.keysym.mod == KMOD_LCTRL || event->key.keysym.mod == KMOD_RCTRL)
            {
                if(l->players[0].view.rightBottom.x-
                        l->players[0].view.leftTop.x > 40){
                    l->players[0].view.leftTop.x+=10;
                    l->players[0].view.rightBottom.x-=10;
                    l->players[0].view.leftTop.y-=10;
                    l->players[0].view.rightBottom.y+=10;
                    updateViewOfHovercraft(l->players);
                }
            }
            break;
        case SDLK_m:
            if(event->key.keysym.mod == KMOD_LCTRL || event->key.keysym.mod == KMOD_RCTRL)
            {
                if(l->players[0].view.rightBottom.x-
                        l->players[0].view.leftTop.x < l->map.width-20){
                    l->players[0].view.leftTop.x-=10;
                    l->players[0].view.rightBottom.x+=10;
                    l->players[0].view.leftTop.y+=10;
                    l->players[0].view.rightBottom.y-=10;
                    updateViewOfHovercraft(l->players);
                }
            }
            break;
        default:
            break;
        }
        break;
    case SDL_KEYUP:
        switch( event->key.keysym.sym ){
        case SDLK_UP:
            //stopAudioFadeOut(Game.audioIDs[ACCAUDIO],1.2);
            l->players[0].linearAccelerate = 0;
            break;
        case SDLK_DOWN:
            l->players[0].linearAccelerate = 0;
            break;
        case SDLK_LEFT:
            l->players[0].physical_body.vAngle = 0;
            l->players[0].rotationAccelerate = 0;
            break;
        case SDLK_RIGHT:
            l->players[0].physical_body.vAngle = 0;
            l->players[0].rotationAccelerate = 0;
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
}
