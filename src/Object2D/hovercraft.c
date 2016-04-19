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

void initHovercraft(Hovercraft* hovercraft){
    makeObject(&(h_object),0,2,100,6,0,10,0);
    makeRectangle(h_object.shapes+1,-1.6,2,3.2,4);
    makeCircle(h_object.shapes,1.6,makePoint(0,2.3f));
    h_object.shapes[0].color = makeColor3f(0.3,0.2,0.5);
    h_object.shapes[1].color = makeColor3f(0.2,0.2,0.4);

    hovercraft->moteur = 0.03;
    hovercraft->max_linear_speed=1.5;

    h_object.angle =0;
    h_object.aAngle = 0.5;
    hovercraft->max_rotation_speed=2;

    hovercraft->points=0;
    hovercraft->linearAccelerate = hovercraft->rotationAccelerate = 0;

    hovercraft->view.leftTop.x = h_object.x - INITIALVIEWSIZE;
    hovercraft->view.leftTop.y = h_object.y + INITIALVIEWSIZE;
    hovercraft->view.rightBottom.x = h_object.x + INITIALVIEWSIZE;
    hovercraft->view.rightBottom.y = h_object.y - INITIALVIEWSIZE;
}

void drawHovercraft(const Hovercraft* hovercraft){
    glPushMatrix();
    drawObject(&(h_object));
    glPopMatrix();
    float width = hovercraft->view.rightBottom.x - hovercraft->view.leftTop.x;
    float height = hovercraft->view.leftTop.y - hovercraft->view.rightBottom.y;
    float borne = hovercraft->max_linear_speed/2.;
    float margeDX,margeDY;
    float a,b;
    if(h_object.vx < borne && h_object.vx > -borne){
        a = (- width/10.) / borne;
        b = width/2.;
    }
    else{
        a = (width / 8.) / borne;
        b = 0.275*width;
    }
    margeDX =(a * fabs(h_object.vx) + b);
    if(h_object.vy < borne && h_object.vy > -borne){
        a = (- height/10.) / borne;
        b = height/2.;
    }
    else{
        a = (height / 8.) / borne;
        b = 0.275*height;
    }
    margeDY =(a * fabs(h_object.vy) + b);

    glColor3f(0.7,0.1,0.1);
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
    glEnd();
}

void updateView(Hovercraft* hovercraft){
    float width = hovercraft->view.rightBottom.x - hovercraft->view.leftTop.x;
    float height = hovercraft->view.leftTop.y - hovercraft->view.rightBottom.y;
    float borne = hovercraft->max_linear_speed/2.;
    float margeDX,margeDY;
    float a,b;
    if(h_object.vx < borne && h_object.vx > -borne){
        a = (- width/10.) / borne;
        b = width/2.;
    }
    else{
        a = (width / 8.) / borne;
        b = 0.275*width;
    }
    margeDX =(a * fabs(h_object.vx) + b);
    if(h_object.vy < borne && h_object.vy > -borne){
        a = (- height/10.) / borne;
        b = height/2.;
    }
    else{
        a = (height / 8.) / borne;
        b = 0.275*height;
    }
    margeDY = (a * fabs(h_object.vy) + b);
    if(h_object.vx>0){
        if(h_object.x>hovercraft->view.rightBottom.x - margeDX){
            hovercraft->view.rightBottom.x = h_object.x + margeDX;
            hovercraft->view.leftTop.x = hovercraft->view.rightBottom.x - width;
        }
    }
    else{
        if(h_object.x<hovercraft->view.leftTop.x + margeDX){
            hovercraft->view.leftTop.x = h_object.x - margeDX;
            hovercraft->view.rightBottom.x = hovercraft->view.leftTop.x + width;
        }
    }
    if(h_object.vy>0){
        if(h_object.y>hovercraft->view.leftTop.y-margeDY){
            hovercraft->view.leftTop.y = h_object.y + margeDY;
            hovercraft->view.rightBottom.y = hovercraft->view.leftTop.y - height;
        }
    }
    else{
        if(h_object.y<hovercraft->view.rightBottom.y+margeDY){
            hovercraft->view.rightBottom.y = h_object.y - margeDY;
            hovercraft->view.leftTop.y = hovercraft->view.rightBottom.y + height;
        }
    }
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
    updateView(hovercraft);
}
