#include "Object2D/hovercraft.h"
#include <stdlib.h>
#include "Object2D/object.h"
#include <math.h>
#include <stdio.h>

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif
#include <math.h>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#include "textures.h"
#include "audios.h"

#define MARGE 0.05
#define INITIALVIEWSIZE 70
#define h_object (hovercraft->physical_body)

void initHovercraft(Hovercraft* hovercraft, const Map* map){
    makeObject(&(h_object),1,2,100,10,0,10,1);
    Color4f c;
    c = makeColor4f(1,0.9,0.3,1);
    makeRectangleWithTexture(h_object.shapes,-3,4,6,8,c,getTexture(27),
                             getTextureWidth(27),getTextureHeight(27),0);
    makeCircleWithTexture(h_object.shapes+1,3,makePoint(0,3.8f),c,getTexture(28),
               getTextureWidth(28),getTextureHeight(28));

    Effect e;
    e.rebound.resistance = 100;
    e.rebound.rebound_value =1;
    hovercraft->physical_body.effectDelays[0]=1;
    hovercraft->physical_body.effectsAtCollision[0] = e;
    hovercraft->physical_body.effectsTypesAtCollision[0] = REBOUND;
    hovercraft->moteur = 0.055;
    hovercraft->max_linear_speed=1.95;

    h_object.angle =0;
    h_object.aAngle = 0.5;
    hovercraft->direction = makeVector(0,1);
    hovercraft->max_rotation_speed=2;

    hovercraft->points=0;
    hovercraft->linearAccelerate = hovercraft->rotationAccelerate = 0;
    makeCamera(&hovercraft->view,INITIALVIEWSIZE*2,INITIALVIEWSIZE*2,map);
}

void drawHovercraft(const Hovercraft* hovercraft){
    glPushMatrix();
    drawObject(&(h_object));
    glPopMatrix();
}

void drawMiniHovercraft(const Hovercraft* hovercraft){
    glPushMatrix();
    int i;
    glColor3f(h_object.shapes[0].color.r,h_object.shapes[0].color.g,h_object.shapes[0].color.b);
    glTranslatef(h_object.x,h_object.y,0);
    glRotatef(h_object.angle-90.,0,0,1.);
    glScalef(1.5,1.5,1);
    for(i=0;i<h_object.nbShapes;i++)
    {
        drawMiniShape(h_object.shapes+i);
    }
    glPopMatrix();
}

int handleEventHovercraft(Hovercraft *hovercraft, const SDL_Event* e){
    if(e->type==SDL_KEYDOWN){
        switch (e->key.keysym.sym) {
        case SDLK_KP_PLUS:
            zoomPlus(&hovercraft->view);
            break;
        case SDLK_KP_MINUS:
            zoomMinus(&hovercraft->view);
            break;
        case SDLK_UP:
            hovercraft->linearAccelerate=1;
            break;
        case SDLK_DOWN:
            hovercraft->linearAccelerate=-1;
            break;
        case SDLK_LEFT:
            hovercraft->rotationAccelerate=1;
            break;
        case SDLK_RIGHT:
            hovercraft->rotationAccelerate=-1;
            break;
        default:
            return 0;
            break;
        }
        return 1;
    }
    else if(e->type==SDL_KEYUP){
        switch (e->key.keysym.sym) {
        case SDLK_UP:
        case SDLK_DOWN:
            hovercraft->linearAccelerate=0;
            break;
        case SDLK_LEFT:
        case SDLK_RIGHT:
            hovercraft->rotationAccelerate=0;
            break;
        default:
            return 0;
            break;
        }
        return 1;
    }
    return 0;
}

void updateHovercraft(Hovercraft* hovercraft){
    if(h_object.receivedModifs!=NULL){
        Modification* current = h_object.receivedModifs;
        while(current->next!=NULL){
            if(applyEffectToHovercraft(current->next,hovercraft)){
                removeModifications(&hovercraft->physical_body,&current->next);
            }
            else
                current = current->next;
        }
        if(applyEffectToHovercraft(h_object.receivedModifs,hovercraft)){
            removeModifications(&hovercraft->physical_body,&(h_object.receivedModifs));
        }
    }
    if(hovercraft->rotationAccelerate){
        if(fabs(h_object.vAngle+h_object.aAngle*hovercraft->rotationAccelerate) < hovercraft->max_rotation_speed){
            h_object.vAngle += h_object.aAngle*hovercraft->rotationAccelerate;
        }
    }
    else{
        h_object.vAngle = 0;
    }
    float angle=h_object.angle * M_PI / 180.;
    hovercraft->direction.x = cosf(angle);
    hovercraft->direction.y = sinf(angle);
    if(hovercraft->linearAccelerate)
    {
        playAudioFadeIn(1,0.5);
        h_object.ax= hovercraft->moteur * hovercraft->direction.x *
                     hovercraft->linearAccelerate;
        h_object.ay= hovercraft->moteur * hovercraft->direction.y *
                     hovercraft->linearAccelerate;
        float speedX = h_object.vx+h_object.ax; speedX *= speedX;
        float speedY = h_object.vy+h_object.ay; speedY *= speedY;
        float speedMax = hovercraft->max_linear_speed*hovercraft->max_linear_speed;
        if(speedX+speedY > speedMax && h_object.vx*hovercraft->direction.x>0 &&
                h_object.vy*hovercraft->direction.y>0)
        {
            h_object.ax=0;
            h_object.ay=0;
            h_object.vx=hovercraft->max_linear_speed * hovercraft->direction.x;
            h_object.vy=hovercraft->max_linear_speed * hovercraft->direction.y;
        }
    }
    else{
        h_object.ax=0;
        h_object.ay=0;
    }

    updateObject(&h_object);
}
