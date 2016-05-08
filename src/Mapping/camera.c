#include "Mapping/camera.h"
#include "Mapping/map.h"
#include "Object2D/hovercraft.h"
#include <math.h>
#include <stdio.h>
#include "SDL_tools.h"

#define h_object h->physical_body

void makeCamera(Camera *c, float width, float height, const Map* map){
    c->leftTop = makePoint(-width/2.,height/2.);
    c->rightBottom = makePoint(width/2.,-height/2.);
    c->map = map;
}

void updateViewOfHovercraft(Hovercraft* h){
    float width = h->view.rightBottom.x - h->view.leftTop.x;
    float height = h->view.leftTop.y - h->view.rightBottom.y;
    h->view.rightBottom.x = h_object.x + width/2.;
    h->view.leftTop.x =  h_object.x - width/2.;

    h->view.leftTop.y = h_object.y + height/2.;
    h->view.rightBottom.y = h_object.y - height/2.;
}

void applyCameraTransform(const Camera *c){
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    float scaleX = (window.orthoGLX*2)/(c->rightBottom.x - c->leftTop.x);
    float scaleY = (window.orthoGLY*2)/(c->leftTop.y - c->rightBottom.y);
    glScalef(scaleX,scaleY,1.);
    float translateX,translateY;
    if(c->leftTop.x < c->map->bounds.leftTop.x)
        translateX = -window.orthoGLX/scaleX - c->map->bounds.leftTop.x;
    else if (c->rightBottom.x > c->map->bounds.rightBottom.x)
        translateX = window.orthoGLX/scaleX - c->map->bounds.rightBottom.x;
    else
        translateX = -window.orthoGLX/scaleX - c->leftTop.x;

    if(c->leftTop.y > c->map->bounds.leftTop.y)
        translateY = window.orthoGLY/scaleY - c->map->bounds.leftTop.y;
    else if (c->rightBottom.y < c->map->bounds.rightBottom.y)
        translateY = -window.orthoGLY/scaleY - c->map->bounds.rightBottom.y;
    else
        translateY = window.orthoGLY/scaleY - c->leftTop.y;

    glTranslatef(translateX, translateY,0);
}


void zoomMinus(Camera *c){
    if(c->rightBottom.x-c->leftTop.x < c->map->width/2+MINVIEWWIDTH){
        c->leftTop.x-=10;
        c->rightBottom.x+=10;
        c->leftTop.y+=10;
        c->rightBottom.y-=10;
    }
}

void zoomPlus(Camera *c){
    if(c->rightBottom.x-c->leftTop.x > MINVIEWWIDTH){
        c->leftTop.x+=10;
        c->rightBottom.x-=10;
        c->leftTop.y-=10;
        c->rightBottom.y+=10;
    }
}

void zoom(Camera *c, float scale){
    float width = c->rightBottom.x - c->leftTop.x;
    float zoomW = width*scale/2.;
    c->leftTop.x-=zoomW;
    c->rightBottom.x+=zoomW;
    c->leftTop.y+=zoomW;
    c->rightBottom.y-=zoomW;
}

void getMousePositionInGL(const Camera *c, const SDL_Event *mouseEvent, float *x, float *y){
    Point2D camCenter;
    camCenter.x = window.width/2;
    camCenter.y = window.height/2;
    *x = mouseEvent->motion.x - camCenter.x;
    *y = (window.height - mouseEvent->motion.y) - camCenter.y;
    *x *= window.orthoGLX/window.width;
    *y *= window.orthoGLY/window.height;
    *x *= (c->rightBottom.x - c->leftTop.x)/window.orthoGLX;
    *y *= (c->leftTop.y - c->rightBottom.y)/window.orthoGLY;
    *x += (c->rightBottom.x + c->leftTop.x)/2;
    *y += (c->leftTop.y + c->rightBottom.y)/2;
}
