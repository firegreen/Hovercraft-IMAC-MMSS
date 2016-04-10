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

#define FROTTEMENT 0.05
#define MARGE 0.05

void initHovercraft(Hovercraft* hovercraft){
    hovercraft->physical_body = *(makeObject(0,2,100,2));
    makeRectangle(hovercraft->physical_body.shapes+1,-2,2,4,4);
    makeCircle(hovercraft->physical_body.shapes,2,makePoint(0,2.2f));
    hovercraft->physical_body.shapes[0].color = makeColor3f(0.3,0.2,0.5);
    hovercraft->physical_body.shapes[1].color = makeColor3f(0.2,0.2,0.4);

    hovercraft->moteur = 0.06;
    hovercraft->ax=hovercraft->ay=0;
    hovercraft->max_speed=1.5;
    hovercraft->vx=hovercraft->vy = 0;
    hovercraft->points=0;
    hovercraft->linearBoost = 0;

    hovercraft->rotationAcceleration = 0.05;
    hovercraft->rotationSpeed = 0;
    hovercraft->rotationBoost = 0;

    hovercraft->view.leftTop.x = hovercraft->physical_body.x - 50;
    hovercraft->view.leftTop.y = hovercraft->physical_body.y + 50;
    hovercraft->view.rightBottom.x = hovercraft->physical_body.x + 50;
    hovercraft->view.rightBottom.y = hovercraft->physical_body.y - 50;
}

void drawHovercraft(const Hovercraft* hovercraft){
    glPushMatrix();
    drawObject(&(hovercraft->physical_body));
    glPopMatrix();
    glColor3ub(214,227,95);
    float width = hovercraft->view.rightBottom.x - hovercraft->view.leftTop.x;
    float height = hovercraft->view.leftTop.y - hovercraft->view.rightBottom.y;
    float borne = hovercraft->max_speed/2.;
    float margeDX,margeDY;
    float a,b;
    if(hovercraft->vx < borne && hovercraft->vx > -borne){
        a = (- width/4.) / borne;
        b = width/2.;
    }
    else{
        a = (width / 2.) / borne;
        b = -width/4.;
    }
    margeDX =(a * fabs(hovercraft->vx) + b);
    if(hovercraft->vy < borne && hovercraft->vy > -borne){
        a = (- height/4.) / borne;
        b = height/2.;
    }
    else{
        a = (height / 2.) / borne;
        b = -height/4.;
    }
    margeDY =(a * fabs(hovercraft->vy) + b);
    /*glColor3f(0.7,0.1,0.1);
    glBegin(GL_LINES);
    glVertex2f(hovercraft->view.leftTop.x,hovercraft->view.leftTop.y - margeDY);
    glVertex2f(hovercraft->view.rightBottom.x,hovercraft->view.leftTop.y - margeDY);
    glEnd();
    glColor3f(0.1,0.1,0.7);
    glBegin(GL_LINES);
    glVertex2f(hovercraft->view.leftTop.x,hovercraft->view.rightBottom.y + margeDY);
    glVertex2f(hovercraft->view.rightBottom.x,hovercraft->view.rightBottom.y + margeDY);
    glEnd();
    glColor3ub(0.1,0.7,0.1);
    glBegin(GL_LINES);
    glVertex2f(hovercraft->view.rightBottom.x - margeDX,hovercraft->view.rightBottom.y);
    glVertex2f(hovercraft->view.rightBottom.x - margeDX,hovercraft->view.leftTop.y);
    glEnd();
    glColor3ub(0.7,0.1,0.7);
    glBegin(GL_LINES);
    glVertex2f(hovercraft->view.leftTop.x + margeDX,hovercraft->view.rightBottom.y);
    glVertex2f(hovercraft->view.leftTop.x +margeDX,hovercraft->view.leftTop.y);
    glEnd();*/
}

void updateView(Hovercraft* hovercraft){
    float width = hovercraft->view.rightBottom.x - hovercraft->view.leftTop.x;
    float height = hovercraft->view.leftTop.y - hovercraft->view.rightBottom.y;
    float borne = hovercraft->max_speed/2.;
    float margeDX,margeDY;
    float a,b;
    if(hovercraft->vx < borne && hovercraft->vx > -borne){
        a = (- width/4.) / borne;
        b = width/2.;
    }
    else{
        a = (width / 2.) / borne;
        b = -width/4.;
    }
    margeDX =25;//(a * fabs(hovercraft->vx) + b);
    if(hovercraft->vy < borne && hovercraft->vy > -borne){
        a = (- height/4.) / borne;
        b = height/2.;
    }
    else{
        a = (height / 2.) / borne;
        b = -height/4.;
    }
    margeDY =25;//(a * fabs(hovercraft->vy) + b);

        if(hovercraft->physical_body.x>hovercraft->view.rightBottom.x - margeDX){
            hovercraft->view.rightBottom.x = hovercraft->physical_body.x + margeDX;
            hovercraft->view.leftTop.x = hovercraft->view.rightBottom.x - width;
        }
        if(hovercraft->physical_body.x<hovercraft->view.leftTop.x + margeDX){
            hovercraft->view.leftTop.x = hovercraft->physical_body.x - margeDX;
            hovercraft->view.rightBottom.x = hovercraft->view.leftTop.x + width;
        }
        if(hovercraft->physical_body.y>hovercraft->view.leftTop.y-margeDY){
            hovercraft->view.leftTop.y = hovercraft->physical_body.y + margeDY;
            hovercraft->view.rightBottom.y = hovercraft->view.leftTop.y - height;
        }
        if(hovercraft->physical_body.y<hovercraft->view.rightBottom.y+margeDY){
            hovercraft->view.rightBottom.y = hovercraft->physical_body.y - margeDY;
            hovercraft->view.leftTop.y = hovercraft->view.rightBottom.y + height;
        }
}

void updateHovercraft(Hovercraft* hovercraft){
    Modification* current = hovercraft->physical_body.receivedModifs;
    if(current!=NULL){
        while(current->next!=NULL){
            if(applyEffectToHovercraft(current->next,hovercraft)){
                Modification* nextNext = current->next->next;
                free(current->next);
                current->next = nextNext;
            }
            else
                current = current->next;
        }
        while(applyEffectToHovercraft(hovercraft->physical_body.receivedModifs,hovercraft)){
            hovercraft->physical_body.receivedModifs = hovercraft->physical_body.receivedModifs->next;
            if(hovercraft->physical_body.receivedModifs==NULL)
                break;
        }
    }

    hovercraft->rotationSpeed += hovercraft->rotationAcceleration*hovercraft->rotationBoost;
    hovercraft->physical_body.angle += hovercraft->rotationSpeed;
    if(isZero(hovercraft->vx))
        hovercraft->vx=0;
    else
        hovercraft->vx -= hovercraft->vx*FROTTEMENT;
    if(isZero(hovercraft->vy))
        hovercraft->vy -= 0;
    else
        hovercraft->vy -= hovercraft->vy*FROTTEMENT;
    if(hovercraft->linearBoost)
    {
        hovercraft->ax= hovercraft->moteur *
                cos((hovercraft->physical_body.angle) * M_PI / 180.)*
                hovercraft->linearBoost;
        hovercraft->ay= hovercraft->moteur *
                sin((hovercraft->physical_body.angle) * M_PI / 180.)*
                hovercraft->linearBoost;
    }
    else{
        hovercraft->ax=0;
        hovercraft->ay=0;
    }
    float speedX = hovercraft->vx*hovercraft->vx;
    float speedY = hovercraft->vy*hovercraft->vy;
    if(speedX+speedY <= hovercraft->max_speed*hovercraft->max_speed){
        hovercraft->vx += hovercraft->ax;
        hovercraft->vy += hovercraft->ay;
    }
    hovercraft->physical_body.x += hovercraft->vx;
    hovercraft->physical_body.y += hovercraft->vy;

    updateView(hovercraft);
}
