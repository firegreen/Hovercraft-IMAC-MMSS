#include "Mapping/camera.h"
#include "Mapping/map.h"
#include "Object2D/hovercraft.h"
#include <math.h>
#include <stdio.h>
#include "SDL_tools.h"

#define h_object h->physical_body

void makeCamera(Camera *c, float width, float height, const Map* map,
                Point2D viewportLeftTop, unsigned int viewportWidth, unsigned int viewportHeight){
    c->leftTop = makePoint(-width/2.,height/2.);
    c->rightBottom = makePoint(width/2.,-height/2.);
    c->map = map;
    c->viewportleftTop = viewportLeftTop;
    c->viewportHeight = viewportHeight;
    c->viewportWidth = viewportWidth;
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

void applyCameraTransform(const Camera *c){
    /*glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-window.orthoGLX, window.orthoGLX,-window.orthoGLY,window.orthoGLY);
    glViewport(c->viewportleftTop.x,c->viewportleftTop.y,c->viewportWidth,c->viewportHeight);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();*/
    loadCustomViewport(c->viewportleftTop.x,c->viewportleftTop.y,c->viewportWidth,c->viewportHeight);

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

void getMousePositionInCamera(const Camera *c, const SDL_Event* mouseEvent, int *x, int *y){
    Point2D camCenter;
    camCenter.x = c->viewportleftTop.x + c->viewportWidth/2;
    camCenter.y = c->viewportleftTop.y + c->viewportHeight/2;
    *x = mouseEvent->motion.x - camCenter.x;
    *y = (window.height - mouseEvent->motion.y) - camCenter.y;
}

void getMousePositionInGL(const Camera *c, const SDL_Event *mouseEvent, float *x, float *y){
    int xInt,yInt;
    getMousePositionInCamera(c,mouseEvent,&xInt,&yInt);
    if(xInt<-c->viewportWidth/2 || xInt>c->viewportWidth/2 ||
            yInt>c->viewportHeight/2 || yInt<-c->viewportHeight/2)
    {
        *x = 0;
        *y = 0;
        return;
    }
    *x = (float)xInt * window.orthoGLX/c->viewportWidth;
    *y = (float)yInt * window.orthoGLY/c->viewportHeight;
    *x *= (c->rightBottom.x - c->leftTop.x)/window.orthoGLX;
    *y *= (c->leftTop.y - c->rightBottom.y)/window.orthoGLY;
    *x += (c->rightBottom.x + c->leftTop.x)/2;
    *y += (c->leftTop.y + c->rightBottom.y)/2;
}
