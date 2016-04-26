#include "Mapping/camera.h"
#include "Mapping/map.h"
#include "Object2D/hovercraft.h"
#include <math.h>
#include <stdio.h>

#define h_object h->physical_body

void makeCamera(Camera *c, float width, float height, const Map* map){
    c->leftTop = makePoint(-width/2.,height/2.);
    c->rightBottom = makePoint(width/2.,-height/2.);
    c->map = map;
}

void updateViewOfHovercraft(Hovercraft* h){
    float width = h->view.rightBottom.x - h->view.leftTop.x;
    float height = h->view.leftTop.y - h->view.rightBottom.y;
    float borne = h->max_linear_speed/2.;
    float margeDX,margeDY;
    float a,b;
    float power = (h->moteur - h->view.map->frottement)*1.5;
    if(h_object.vx < borne && h_object.vx > -borne){
        a = (- width * power/2.) / borne;
        b = width/2.;
    }
    else{
        a = (width * power) / borne;
        b = (- width * power/2.) + width/2.
             - (width * power);
    }
    margeDX =(a * fabs(h_object.vx) + b);
    if(h_object.vy < borne && h_object.vy > -borne){
        a = (- height * power/2.) / borne;
        b = height/2.;
    }
    else{
        a = (height * power) / borne;
        b = (- height * power/2.) + height/2.
             - (height * power);
    }
    margeDY = (a * fabs(h_object.vy) + b);
    if(h_object.vx>0){
        h->view.rightBottom.x = h_object.x + margeDX;
        h->view.leftTop.x = h->view.rightBottom.x - width;
    }
    else{
        h->view.leftTop.x = h_object.x - margeDX;
        h->view.rightBottom.x = h->view.leftTop.x + width;
    }
    if(h_object.vy>0){
        h->view.leftTop.y = h_object.y + margeDY;
        h->view.rightBottom.y = h->view.leftTop.y - height;
    }
    else{
        h->view.rightBottom.y = h_object.y - margeDY;
        h->view.leftTop.y = h->view.rightBottom.y + height;
    }
}

void applyCameraTransform(const Camera *c, float viewGLX1, float viewGLY1,
                                             float viewGLX2, float viewGLY2){
    float scaleX = (viewGLX2-viewGLX1)/(c->rightBottom.x - c->leftTop.x);
    float scaleY = (viewGLY2-viewGLY1)/(c->leftTop.y - c->rightBottom.y);
    glScalef(scaleX,scaleY,1.);
    float translateX,translateY;
    if(c->leftTop.x < c->map->bounds.leftTop.x)
        translateX = viewGLX1/scaleX - c->map->bounds.leftTop.x;
    else if (c->rightBottom.x > c->map->bounds.rightBottom.x)
        translateX = viewGLX2/scaleX - c->map->bounds.rightBottom.x;
    else
        translateX = viewGLX1/scaleX - c->leftTop.x;

    if(c->leftTop.y > c->map->bounds.leftTop.y)
        translateY = viewGLY2/scaleY - c->map->bounds.leftTop.y;
    else if (c->rightBottom.y < c->map->bounds.rightBottom.y)
        translateY = viewGLY1/scaleY - c->map->bounds.rightBottom.y;
    else
        translateY = viewGLY2/scaleY - c->leftTop.y;

    glTranslatef(translateX, translateY,0);
}
