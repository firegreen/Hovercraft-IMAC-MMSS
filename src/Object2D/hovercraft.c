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
#define INITIALVIEWSIZE 70
#define h_object (hovercraft->physical_body)

void initHovercraft(Hovercraft* hovercraft, const Map* map, unsigned int id,
                    Point2D viewLeftTop, float viewWidth, float viewHeight,
                    SDL_Event* actionsEvents){
    makeObject(&(h_object),1,2,100,6,0,10,1);
    makeRectangle(h_object.shapes+1,-1.6,2,3.2,4,makeColor4f(0.3,0.2,0.5,1),1);
    makeCircle(h_object.shapes,1.6,makePoint(0,2.3f),makeColor4f(0.2,0.2,0.4,1),1);

    Effect e;
    e.rebound.resistance = 100;
    e.rebound.rebound_value =1;
    hovercraft->physical_body.effectDelays[0]=2;
    hovercraft->physical_body.effectsAtCollision[0] = e;
    hovercraft->physical_body.effectsTypesAtCollision[0] = REBOUND;
    hovercraft->moteur = 0.055;
    hovercraft->max_linear_speed=1.95;
    hovercraft->id = id;

    h_object.angle =0;
    h_object.aAngle = 0.5;
    hovercraft->direction = makeVector(0,1);
    hovercraft->max_rotation_speed=2;

    hovercraft->points=0;
    hovercraft->linearAccelerate = hovercraft->rotationAccelerate = 0;
    hovercraft->actionsEvents = actionsEvents;
    makeCamera(&hovercraft->view,INITIALVIEWSIZE*2,INITIALVIEWSIZE*2,map, viewLeftTop,
               viewWidth,viewHeight);
}

void initBotHovercraft(BotHovercraft *hovercraft, const Map* map, float error,
                       Point2D viewLeftTop, float viewWidth, float viewHeight){
    initHovercraft(&hovercraft->h,map,0,viewLeftTop,viewWidth,viewHeight,NULL);
    hovercraft->path = NULL;
    hovercraft->goal = makePoint(0,0);
    hovercraft->error = error;
}

void drawHovercraft(const Hovercraft* hovercraft){
    glPushMatrix();
    drawObject(&(h_object));
    glPopMatrix();
    glColor3f(0.7,0.1,0.1);
}

int cmpEvent(const SDL_Event* e1, const SDL_Event* e2){
    if(e1->type != e2->type)
        return 0;
    switch(e1->type){
    case SDL_KEYDOWN:
    case SDL_KEYUP:
        return e1->key.keysym.sym == e2->key.keysym.sym &&
                e1->key.keysym.mod == e2->key.keysym.mod;
    case SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEBUTTONUP:
        return e1->button.button == e2->button.button;
    case SDL_MOUSEMOTION:
        return 1;
    default:
        return 0;
    }
}

int handleEventHovercraft(Hovercraft *hovercraft, const SDL_Event* e){
    if(e->type == SDL_MOUSEMOTION && hovercraft->actionsEvents[ROTATELEFT].type==e->type){
        float x,y;
        getMousePositionInGL(&hovercraft->view,e,&x,&y);
        float dx = x - hovercraft->physical_body.x, dy = y - hovercraft->physical_body.y;
        float width = (hovercraft->view.rightBottom.x-hovercraft->view.leftTop.x),
                height = hovercraft->view.leftTop.y - hovercraft->view.leftTop.y;

        float pos = hovercraft->direction.x*dy - hovercraft->direction.y*dx;
        printf("2- %f %f\n",dx,dy);
        if((dx*dx + dy*dy < (width*width+height*height)/15) || ((pos>-1 && pos<1)
            && (dx*hovercraft->direction.x>0 && dy*(hovercraft->direction.y)>0))){
            hovercraft->rotationAccelerate = 0;
        }
        else{
            if(pos>0)
                hovercraft->rotationAccelerate = 1;
            else
                hovercraft->rotationAccelerate = -1;
        }
        return 1;
    }
    int i;
    for(i=0;i<NBACTIONS;i++){
        if(cmpEvent(e,hovercraft->actionsEvents + i)){
            switch (i) {
            case  ZOOMMINUS:
                zoomMinus(&hovercraft->view);
                updateViewOfHovercraft(hovercraft);
                break;
            case ZOOMPLUS:
                zoomPlus(&hovercraft->view);
                updateViewOfHovercraft(hovercraft);
                break;
            case FORWARD:
                hovercraft->linearAccelerate = 1;
                break;
            case BACKWARD:
                hovercraft->linearAccelerate = -0.5;
                break;
            case FORWARDSTOP:
            case BACKWARDSTOP:
                hovercraft->linearAccelerate = 0;
                break;
            case ROTATELEFT:
                hovercraft->rotationAccelerate = 1;
                break;
            case ROTATERIGHT:
                hovercraft->rotationAccelerate = -1;
                break;
            case ROTATERIGHTSTOP:
            case ROTATELEFTSTOP:
                hovercraft->rotationAccelerate = 0;
                break;
            default:
                break;
            }
            return 1;
        }
    }
    return 0;
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

        h_object.ax= hovercraft->moteur * hovercraft->direction.x *
                     hovercraft->linearAccelerate;
        h_object.ay= hovercraft->moteur * hovercraft->direction.y *
                     hovercraft->linearAccelerate;
        float speedX = h_object.vx+h_object.ax; speedX *= speedX;
        float speedY = h_object.vy+h_object.ay; speedY *= speedY;
        float speedMax = hovercraft->max_linear_speed*hovercraft->max_linear_speed;
        if(speedX+speedY > speedMax && h_object.vx*hovercraft->direction.y>0 &&
                h_object.vy*hovercraft->direction.x>0)
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

#undef h_object
#define h_object (hovercraft->h.physical_body)

Vector2D newDirection(BotHovercraft *hovercraft, Chained_Object* objects){
    Chained_Object* co;
    int i=0;
    float cosLeft = cosf(M_PI*0.15), sinLeft =sinf(M_PI*0.15);
    Vector2D direction = hovercraft->path->direction;
    Line l;
    Intersection intersect;
    for(i=0;i<5;i++){
        direction = makeVector(direction.x*cosLeft - direction.y*sinLeft,
                                direction.x*sinLeft + direction.y*cosLeft);
        l.start = makePoint(h_object.x,h_object.y);
        l.end = makePoint(h_object.x+direction.x*6*h_object.colliderRadius,
                          h_object.y+direction.y*6*h_object.colliderRadius);
        co=objects;
        while (co!=NULL) {
            if(collisionBetweenLineAndObject(l,co->object,&intersect)){
                break;
            }
            co = co->next;
        }
        if(co==NULL) return direction;
    }
    float cosRight = cosf(-M_PI*0.15), sinRight =sinf(-M_PI*0.15);
    direction = hovercraft->path->direction;
    for(i=0;i<5;i++){
        direction = makeVector(direction.x*cosRight - direction.y*sinRight,
                                direction.x*sinRight + direction.y*cosRight);
        l.start = makePoint(h_object.x,h_object.y);
        l.end = makePoint(h_object.x+direction.x*6*h_object.colliderRadius,
                          h_object.y+direction.y*6*h_object.colliderRadius);
        co=objects;
        while (co!=NULL) {
            if(collisionBetweenLineAndObject(l,co->object,&intersect)){
                break;
            }
            co = co->next;
        }
        if(co==NULL) return direction;
    }
    float cosBehind = cosf(M_PI), sinBehind =sinf(M_PI);

    direction = makeVector(hovercraft->path->direction.x*cosBehind -
                      hovercraft->path->direction.y*sinBehind,
                      hovercraft->path->direction.x*sinBehind +
                      hovercraft->path->direction.y*cosBehind);
    return direction;
}

void updateBotHovercraft(BotHovercraft *hovercraft, Chained_Object* objects){
    if(h_object.receivedModifs!=NULL){
        Modification* current = h_object.receivedModifs;
        Modification* nextNext;

        while(current->next!=NULL){
            if(applyEffectToHovercraft(current->next,&hovercraft->h)){
                nextNext = current->next->next;
                free(current->next);
                current->next = nextNext;
            }
            else
                current = current->next;
        }
        if(applyEffectToHovercraft(h_object.receivedModifs,&hovercraft->h)){
            current = h_object.receivedModifs->next;
            free(h_object.receivedModifs);
            h_object.receivedModifs = current;
        }
    }
    if(hovercraft->path==NULL){
        h_object.ax=0;
        h_object.ay=0;
        h_object.vx *=0.5;
        h_object.vy *=0.5;
        h_object.vAngle=0;
        updateObject(&h_object);
    }
    else{
        Line l;
        l.start = makePoint(h_object.x,h_object.y);
        l.end = makePoint(h_object.x+hovercraft->path->direction.x*
                            (hovercraft->h.max_linear_speed+h_object.colliderRadius*2),
                          h_object.y+hovercraft->path->direction.y*
                            (hovercraft->h.max_linear_speed+h_object.colliderRadius*2));
        Chained_Object* co=objects;
        Intersection intersect;
        while(co!=NULL){
            if(collisionBetweenLineAndObject(l,co->object,&intersect)){
                hovercraft->path = addDirection(hovercraft->path,newDirection(hovercraft,objects));
                break;
            }
            co = co->next;
        }
        Vector2D distance = makeVectorAB(makePoint(h_object.x,h_object.y),hovercraft->goal);
        if(co==NULL){
            if(hovercraft->path->below==NULL){
                if(distance.x*distance.x +distance.y*distance.y <12){
                    freeDirectionStack(hovercraft->path);
                    hovercraft->path = NULL;
                    return;
                }
                else{
                    hovercraft->path->direction = distance;
                    normalizeVector(&hovercraft->path->direction);
                }
            }
            else{
                co=objects;
                l.end = makePoint(h_object.x+hovercraft->path->below->direction.x*
                                    (hovercraft->h.max_linear_speed+h_object.colliderRadius*2),
                                  h_object.y+hovercraft->path->below->direction.y*
                                    (hovercraft->h.max_linear_speed+h_object.colliderRadius*2));

                while(co!=NULL){
                    if(collisionBetweenLineAndObject(l,co->object,&intersect)){
                        break;
                    }
                    co = co->next;
                }
                if(co==NULL){
                    DirectionStack* tmp =hovercraft->path;
                    hovercraft->path = hovercraft->path->below;
                    free(tmp);
                }
            }
        }
        h_object.ax= hovercraft->h.moteur *
                cosf((h_object.angle) * M_PI / 180.)/2.;
        h_object.ay= hovercraft->h.moteur *
                sin((h_object.angle) * M_PI / 180.)/2.;
        float a = hovercraft->path->direction.x*h_object.ay,
               b = hovercraft->path->direction.y*h_object.ax;
        float speedX,speedY,speedMax;
        if(hovercraft->path->direction.x*h_object.ax>0 || hovercraft->path->direction.y*h_object.ay>0){
            if(fabs(a-b)<hovercraft->error){
                h_object.vAngle = 0;
            }
            else{
                h_object.ax*=0.7;
                h_object.ay*=0.7;
                if(a<b){
                    if(fabs(h_object.vAngle+h_object.aAngle) < hovercraft->h.max_rotation_speed){
                        h_object.vAngle += h_object.aAngle;
                    }
                }
                else{
                    if(fabs(h_object.vAngle-h_object.aAngle) < hovercraft->h.max_rotation_speed){
                        h_object.vAngle -= h_object.aAngle;
                    }
                }
            }
            speedX = h_object.vx+h_object.ax; speedX *= speedX;
            speedY = h_object.vy+h_object.ay; speedY *= speedY;
            speedMax = hovercraft->h.max_linear_speed*hovercraft->h.max_linear_speed;
            if(speedX+speedY > speedMax)
            {
                h_object.ax=0;
                h_object.ay=0;
                h_object.vx=hovercraft->h.max_linear_speed *
                        cosf((h_object.angle) * M_PI / 180.);
                h_object.vy=hovercraft->h.max_linear_speed *
                        sinf((h_object.angle) * M_PI / 180.);
            }
        }
        else{
            h_object.ax*=-1;
            h_object.ay*=-1;
            if(fabs(a-b)<hovercraft->error){
                h_object.vAngle = 0;
            }
            else{
                h_object.ax*=0.5;
                h_object.ay*=0.5;
                if(a>b){
                    if(fabs(h_object.vAngle+h_object.aAngle) < hovercraft->h.max_rotation_speed){
                        h_object.vAngle += h_object.aAngle*0.5;
                    }
                }
                else{
                    if(fabs(h_object.vAngle-h_object.aAngle) < hovercraft->h.max_rotation_speed){
                        h_object.vAngle -= h_object.aAngle*0.5;
                    }
                }
            }
            speedX = h_object.vx+h_object.ax; speedX *= speedX;
            speedY = h_object.vy+h_object.ay; speedY *= speedY;
            speedMax = hovercraft->h.max_linear_speed*hovercraft->h.max_linear_speed;
            if(speedX+speedY > speedMax)
            {
                h_object.ax=0;
                h_object.ay=0;
                h_object.vx=-hovercraft->h.max_linear_speed/2 *
                        cosf((h_object.angle) * M_PI / 180.);
                h_object.vy=-hovercraft->h.max_linear_speed/2 *
                        sinf((h_object.angle) * M_PI / 180.);
            }
        }
        if(distance.x*distance.x +distance.y*distance.y <500){
            h_object.ax*=0.01;
            h_object.ay*=0.01;
            h_object.vAngle*=0.1;
        }
        updateObject(&h_object);
    }
}

void giveGoalToBot(BotHovercraft *hovercraft, Point2D goal){
    Vector2D direction = makeVectorAB(makePoint(h_object.x,h_object.y),goal);
    normalizeVector(&direction);
    hovercraft->goal = goal;
    if(hovercraft->path!=NULL)
        freeDirectionStack(hovercraft->path);
    hovercraft->path = makeDirectionStack(direction);

}

DirectionStack* makeDirectionStack(Vector2D direction){
    DirectionStack* newD = malloc(sizeof(DirectionStack));
    newD->below = NULL;
    newD->direction = direction;
    return newD;
}

DirectionStack* addDirection(DirectionStack* d, Vector2D direction){
    DirectionStack* newD = malloc(sizeof(DirectionStack));
    newD->below = d;
    newD->direction = direction;
    return newD;
}

void freeDirectionStack(DirectionStack *d){
    DirectionStack* below = d->below, *next;
    while(below){
        next = below->below;
        free(below);
        below = next;
    }
    free(d);
}
