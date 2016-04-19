#include "GUI/level.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <SDL_tools.h>

void drawView(const Map* map, const Hovercraft* hovercraft){
    float scaleX = (2*GLREPERE)/(hovercraft->view.rightBottom.x - hovercraft->view.leftTop.x);
    float scaleY = (2*GLREPERE)/(hovercraft->view.leftTop.y - hovercraft->view.rightBottom.y);
    glScalef(scaleX,scaleY,1.);
    float translateX,translateY;
    if(hovercraft->view.leftTop.x < map->bounds.leftTop.x)
        translateX = -GLREPERE/scaleX - map->bounds.leftTop.x;
    else if (hovercraft->view.rightBottom.x > map->bounds.rightBottom.x)
        translateX = GLREPERE/scaleX - map->bounds.rightBottom.x;
    else
        translateX = -GLREPERE/scaleX - hovercraft->view.leftTop.x;

    if(hovercraft->view.leftTop.y > map->bounds.leftTop.y)
        translateY = GLREPERE/scaleY - map->bounds.leftTop.y;
    else if (hovercraft->view.rightBottom.y < map->bounds.rightBottom.y)
        translateY = -GLREPERE/scaleY - map->bounds.rightBottom.y;
    else
        translateY = GLREPERE/scaleY - hovercraft->view.leftTop.y;

    glTranslatef(translateX, translateY,0);
    drawMap(map);
    drawHovercraft(hovercraft);
}

void drawLevelForOnePlayer(const Level* level){
    glViewport(0,0,window.width,window.height);
    drawView(&(level->map), level->players);
}

void drawLevelForTwoPlayer(const Level* level){
    int width=window.width/2;
    glViewport(0,0,width,window.height);
    drawView(&(level->map), level->players);
    glViewport(width,0,width,window.height);
    drawView(&(level->map), level->players+1);
}
void drawLevelForThreePlayer(const Level* level){
    int width=window.width/2;
    int height=window.height/2;
    glViewport(0,0,width,height);
    drawView(&(level->map), level->players);
    glViewport(width,0,width,height);
    drawView(&(level->map), level->players+1);
    glViewport(0,height,width,height);
    drawView(&(level->map), level->players+2);
}

void drawLevelForFourPlayer(const Level* level){
    int width=window.width/2;
    int height=window.height/2;
    glViewport(0,0,width,height);
    drawView(&(level->map), level->players);
    glViewport(width,0,width,height);
    drawView(&(level->map), level->players+1);
    glViewport(0,height,width,height);
    drawView(&(level->map), level->players+2);
    glViewport(width,height,width,height);
    drawView(&(level->map), level->players+3);
}
void drawLevel(const Level* level){
    switch(level->nbPlayers){
    case 1:
        drawLevelForOnePlayer(level);
        break;
    case 2:
        drawLevelForTwoPlayer(level);
        break;
    case 3:
        drawLevelForThreePlayer(level);
        break;
    case 4:
        drawLevelForFourPlayer(level);
        break;
    }
}

void updateLevel(Level* level){
    updateMap(&(level->map));
    int i =0;
    Chained_Object* co;
    Chained_Object* coNext;
    for(;i<level->nbPlayers;i++){
        applyFrottement(&(level->map), &(level->players[i].physical_body));
        updateHovercraft(level->players+i);
        if(level->map.objects!=NULL){
            co = level->map.objects;
            while(co->next!=NULL){
                handleCollision(&(level->players[i].physical_body),co->next->object);
                if(co->next->object->life > 0){
                    updateObject(co->next->object);
                    applyFrottement(&(level->map), co->next->object);
                    co=co->next;
                }
                else{
                    coNext = co->next->next;
                    free(co->next);
                    co->next = coNext;
                }
            }
            handleCollision(&(level->players[i].physical_body),level->map.objects->object);
            if(level->map.objects->object->life > 0){
                updateObject(level->map.objects->object);
                applyFrottement(&(level->map), level->map.objects->object);
            }
            else{
                coNext = level->map.objects->next;
                free(level->map.objects);
                level->map.objects = coNext;
            }
        }
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
        else if(event->button.button == SDL_BUTTON_WHEELUP){
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
                    updateView(l->players);
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
                    updateView(l->players);
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
