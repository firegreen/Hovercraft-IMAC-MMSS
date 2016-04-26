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

#define MARGE 0.05
#define INITIALVIEWSIZE 55
#define h_object (hovercraft->physical_body)

void initHovercraft(Hovercraft* hovercraft, const Map* map){
    makeObject(&(h_object),1,2,100,6,0,10,0);
    makeRectangle(h_object.shapes+1,-1.6,2,3.2,4,makeColor4f(0.3,0.2,0.5,1),1);
    makeCircle(h_object.shapes,1.6,makePoint(0,2.3f),makeColor4f(0.2,0.2,0.4,1),1);

    Effect e;
    e.rebound.resistance = 100;
    e.rebound.rebound_value =1;
    hovercraft->physical_body.effectDelays[0]=2;
    hovercraft->physical_body.effectsAtCollision[0] = e;
    hovercraft->physical_body.effectsTypesAtCollision[0] = REBOUND;
    hovercraft->moteur = 0.045;
    hovercraft->max_linear_speed=1.5;

    h_object.angle =0;
    h_object.aAngle = 0.5;
    hovercraft->max_rotation_speed=2;

    hovercraft->points=0;
    hovercraft->linearAccelerate = hovercraft->rotationAccelerate = 0;
    makeCamera(&hovercraft->view,INITIALVIEWSIZE*2,INITIALVIEWSIZE*2,map);
}

void drawHovercraft(const Hovercraft* hovercraft){
    glPushMatrix();
    drawObject(&(h_object));
    glPopMatrix();
    glColor3f(0.7,0.1,0.1);
}

void updateHovercraft(Hovercraft* hovercraft){
    if(h_object.receivedModifs!=NULL){
        Modification* current = h_object.receivedModifs;
        Modification* nextNext;

        while(current->next!=NULL){
            if(applyEffectToHovercraft(current->next,hovercraft)){
                nextNext = current->next->next;
                free(current->next);
                current->next = nextNext;
            }
            else
                current = current->next;
        }
        if(applyEffectToHovercraft(h_object.receivedModifs,hovercraft)){
            current = h_object.receivedModifs->next;
            free(h_object.receivedModifs);
            h_object.receivedModifs = current;
        }
    }

    if(fabs(h_object.vAngle) < hovercraft->max_rotation_speed){
        h_object.vAngle += h_object.aAngle*hovercraft->rotationAccelerate;
    }


    if(hovercraft->linearAccelerate)
    {

        h_object.ax= hovercraft->moteur *
                cosf((h_object.angle) * M_PI / 180.)*
                hovercraft->linearAccelerate;
        h_object.ay= hovercraft->moteur *
                sinf((h_object.angle) * M_PI / 180.)*
                hovercraft->linearAccelerate;
        float speedX = h_object.vx+h_object.ax; speedX *= speedX;
        float speedY = h_object.vy+h_object.ay; speedY *= speedY;
        float speedMax = hovercraft->max_linear_speed*hovercraft->max_linear_speed;
        if(speedX+speedY > speedMax){
            h_object.ax=0;
            h_object.ay=0;
            h_object.vx=hovercraft->max_linear_speed *
                    cosf((h_object.angle) * M_PI / 180.)*
                    hovercraft->linearAccelerate;
            h_object.vy=hovercraft->max_linear_speed *
                    sinf((h_object.angle) * M_PI / 180.)*
                    hovercraft->linearAccelerate;
        }
    }
    else{
        h_object.ax=0;
        h_object.ay=0;
    }

    updateObject(&h_object);
}
