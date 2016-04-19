#include "Object2D/object.h"
#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "Object2D/hovercraft.h"

#define M_PI 3.14159265358979323846



void drawShape(const Shape* shape){
    int i;
    switch (shape->type) {
    case POLYGON:
        if(shape->spec.polygon.full)
            glBegin(GL_POLYGON);
        else
            glBegin(GL_LINE_LOOP);
        for(i=0;i<shape->spec.polygon.nbPoints;i++)
        {
            glVertex2f(shape->spec.polygon.points[i].x,shape->spec.polygon.points[i].y);
        }
        glEnd();
        break;
    case CIRCLE:
        if(shape->spec.circle.full){
            glBegin(GL_TRIANGLE_FAN);
            glVertex2f(shape->spec.circle.center.x,shape->spec.circle.center.y);
        }
        else
            glBegin(GL_LINE_LOOP);

        for(i=0; i<=50; i++)
        {
            glVertex2f(cos((float)i*M_PI/25.)*shape->spec.circle.radius+shape->spec.circle.center.x,
                       sin((float)i*M_PI/25.)*shape->spec.circle.radius+shape->spec.circle.center.y);
        }
        glEnd();
        break;
    case SEGMENT:
        glBegin(GL_LINE);
        glVertex2f(shape->spec.segment.p1.x,shape->spec.segment.p1.y);
        glVertex2f(shape->spec.segment.p2.x,shape->spec.segment.p2.y);
        glEnd();
    default: break;
    }
}

void drawShapeWithTexture(const Shape* shape, const Point2D* texturesPoints, GLuint textureId){
    switch (shape->type) {
    case POLYGON:
        break;
    case CIRCLE:
        break;
    default: break;
    }
}

void drawObject(const Object* object){
    int i = 0;
    glPushMatrix();
    glTranslatef(object->x,object->y,0);
    glRotatef(object->angle-90.,0,0,1.);
    for(;i<object->nbShapes;i++)
    {
        glColor3f(object->shapes[i].color.r,object->shapes[i].color.g,object->shapes[i].color.b);
        drawShape(object->shapes+i);
    }
    glPopMatrix();
}

void makeRectangle(Shape* shape, float x,float y,float width,float height){
    shape->type = POLYGON;
    shape->spec.polygon.nbPoints = 4;
    shape->spec.polygon.points = malloc(sizeof(Point2D)*shape->spec.polygon.nbPoints);
    shape->spec.polygon.points[0].x = x;
    shape->spec.polygon.points[0].y = y;
    shape->spec.polygon.points[1].x = x+width;
    shape->spec.polygon.points[1].y = y;
    shape->spec.polygon.points[2].x = x+width;
    shape->spec.polygon.points[2].y = y-height;
    shape->spec.polygon.points[3].x = x;
    shape->spec.polygon.points[3].y = y-height;
    shape->color = makeColor3f(0,0,0);
    shape->spec.polygon.full = 1;
}

void makeInversedRectangle(Shape* shape, float x,float y,float width,float height){
    shape->type = POLYGON;
    shape->spec.polygon.nbPoints = 4;
    shape->spec.polygon.points = malloc(sizeof(Point2D)*shape->spec.polygon.nbPoints);
    shape->spec.polygon.points[3].x = x+width;
    shape->spec.polygon.points[3].y = y;
    shape->spec.polygon.points[2].x = x;
    shape->spec.polygon.points[2].y = y;
    shape->spec.polygon.points[1].x = x;
    shape->spec.polygon.points[1].y = y-height;
    shape->spec.polygon.points[0].x = x+width;
    shape->spec.polygon.points[0].y = y-height;
    shape->color = makeColor3f(0,0,0);
    shape->spec.polygon.full = 0;
}

void makeCircle(Shape* shape, float r, Point2D c){
    shape->type = CIRCLE;
    shape->spec.circle.center = c;
    shape->spec.circle.radius = r;
    shape->color = makeColor3f(0.25,0,0.5);
    shape->spec.circle.full = 1;
}

void makeSegment(Shape *shape, Point2D p1, Point2D p2){
    shape->type = SEGMENT;
    shape->spec.segment.p1 = p1;
    shape->spec.segment.p2 = p2;
    shape->color = makeColor3f(0,0,0);
}

int collision_C_C(const Shape* shape1, Point2D p1, float angle1,
                  const Shape* shape2, Point2D p2, float angle2, Intersection* intersect){
    Point2D C1 = makeTranslateAndRotate(shape1->spec.circle.center,p1.x,p1.y,angle1);
    Point2D C2 = makeTranslateAndRotate(shape2->spec.circle.center,p2.x,p2.y,angle2);
    float distanceX = C1.x - C2.x;
    float distanceY = C1.y - C2.y;
    float radiusSum = shape1->spec.circle.radius + shape2->spec.circle.radius;
    if(distanceX*distanceX+distanceY*distanceY<radiusSum*radiusSum){
        intersect->normal1 = makeVector(distanceX,distanceY);
        normalizeVector(&(intersect->normal1));
        intersect->normal2 = makeVector(-intersect->normal1.x,-intersect->normal1.y);
        //intersect->point = ?
        return 1;
    }
    return 0;
}

int makeIntersection_C_S(Point2D C, float radius, Point2D A, Point2D B,
                         Intersection* intersect){
    Vector2D AB = makeVectorAB(A,B);
    Vector2D CA = makeVectorAB(C,A);
    float scalAB = AB.x*AB.x + AB.y * AB.y;
    float scalABCA = AB.x*CA.x + AB.y * CA.y;
    float scalOA = A.x*A.x+
                    A.y*A.y;
    float scalOC = C.x*C.x+
                    C.y*C.y;
    float scalOCOA = A.x*C.x+
                    C.y*A.y;

    float a = scalAB;
    float b = scalABCA * 2.;
    float c = scalOA + scalOC - 2.*(scalOCOA) - radius*radius;
    float delta = b*b - 4.*a*c;
    if(delta>0.){
        float t1 = (-b + sqrtf(delta))/(2.*a);
        float t2 = (-b - sqrtf(delta))/(2.*a);
        if(t1>t2)
        {
            float temp = t1;
            t1 = t2;
            t2 = temp;
        }
        if((t1 <= 0. && t2 >=1.) || (t2>=0. && t2 <=1.) || (t1 >=0. && t1<=1.))
        {
            intersect->normal1 = makeVector(AB.y,-AB.x);
            normalizeVector(&(intersect->normal1));
            intersect->normal2 = makeVector(-intersect->normal1.x,-intersect->normal1.y);
            return 1;
        }
    }
    else if(isZero(delta)){
        float t = (-b)/(2.*a);
        if(t>=0. && t<=1.){
            intersect->normal1 = makeVector(-AB.y,AB.x);
            normalizeVector(&(intersect->normal1));
            intersect->normal2 = makeVector(-intersect->normal1.x,-intersect->normal1.y);
            return 1;
        }
    }
    return 0;
}


int collision_C_P(const Shape* shape1, Point2D p1, float angle1, const Shape* shape2, Point2D p2, float angle2,
                  Intersection* intersect){
    int i = 0;
    Point2D C = makeTranslateAndRotate(shape1->spec.circle.center,p1.x,p1.y,angle1);
    for(;i<shape2->spec.polygon.nbPoints-1;i++)
    {
        Point2D A = makeTranslateAndRotate(shape2->spec.polygon.points[i],p2.x,p2.y,angle2);
        Point2D B = makeTranslateAndRotate(shape2->spec.polygon.points[i+1],p2.x,p2.y,angle2);
        if(makeIntersection_C_S(C,shape1->spec.circle.radius,A,B,intersect))
            return 1;
    }
    Point2D A = makeTranslateAndRotate(shape2->spec.polygon.points[shape2->spec.polygon.nbPoints-1],p2.x,p2.y,angle2);
    Point2D B = makeTranslateAndRotate(shape2->spec.polygon.points[0],p2.x,p2.y,angle2);
    return makeIntersection_C_S(C,shape1->spec.circle.radius,A,B,intersect);
}

int collision_S_V(Point2D P1, Point2D P2, Point2D A, Point2D B, Intersection* intersect){
    Vector2D vAP1,vAP2,vAB;
    vAB = makeVectorAB(A,B);
    vAP1 = makeVectorAB(A,P1);
    vAP2= makeVectorAB(A,P2);
    float d1 = vAB.x*vAP1.y - vAB.y*vAP1.x;
    float d2 = vAB.x*vAP2.y - vAB.y*vAP2.x;
    if(d1>0 || d2>0){
        intersect->normal1 = makeVector(-vAB.y,vAB.x);
        normalizeVector(&(intersect->normal1));
        intersect->normal2 = makeVector(P1.y-P2.y,P1.x-P2.x);
        normalizeVector(&(intersect->normal2));
        return 1;
    }
    return 0;
}

int collision_P_P(const Shape *shape1, Point2D p1, float angle1,
                  const Shape *shape2, Point2D p2, float angle2,
                  Intersection* intersect){
    int i, j, isIn;
    Point2D A1,B1,A2,B2;
    for (i=0; i<shape1->spec.polygon.nbPoints-1; i++)
    {
        A1 = makeTranslateAndRotate(shape1->spec.polygon.points[i],p1.x,p1.y,angle1);
        B1  = makeTranslateAndRotate(shape1->spec.polygon.points[i+1],p1.x,p1.y,angle1);
        isIn = 1;
        for (j=0; j<shape2->spec.polygon.nbPoints-1; j++)
        {
            A2  = makeTranslateAndRotate(shape2->spec.polygon.points[j],p2.x,p2.y,angle2);
            B2  = makeTranslateAndRotate(shape2->spec.polygon.points[j+1],p2.x,p2.y,angle2);
            isIn = collision_S_V(A1,B1,A2,B2,intersect);
            if(!isIn) break;
        }
        if(intersect){
            A2  = makeTranslateAndRotate(shape2->spec.polygon.points[j],p2.x,p2.y,angle2);
            B2  = makeTranslateAndRotate(shape2->spec.polygon.points[0],p2.x,p2.y,angle2);
            isIn = collision_S_V(A1,B1,A2,B2,intersect);
            if(isIn) return isIn;
        }
    }
    A1 = makeTranslateAndRotate(shape1->spec.polygon.points[i],p1.x,p1.y,angle1);
    B1  = makeTranslateAndRotate(shape1->spec.polygon.points[0],p1.x,p1.y,angle1);
    isIn = 1;
    for (j=0; j<shape2->spec.polygon.nbPoints-1; j++)
    {
        A2  = makeTranslateAndRotate(shape2->spec.polygon.points[j],p2.x,p2.y,angle2);
        B2  = makeTranslateAndRotate(shape2->spec.polygon.points[j+1],p2.x,p2.y,angle2);
        isIn = collision_S_V(A1,B1,A2,B2,intersect);
        if(!isIn) break;
    }
    if(intersect){
        A2  = makeTranslateAndRotate(shape2->spec.polygon.points[j],p2.x,p2.y,angle2);
        B2  = makeTranslateAndRotate(shape2->spec.polygon.points[0],p2.x,p2.y,angle2);
        isIn = collision_S_V(A1,B1,A2,B2,intersect);
        if(isIn) return isIn;
    }
    return isIn;
}

int collisionBetweenShapes(const Shape *shape1, Point2D p1, float angle1,
                           const Shape *shape2, Point2D p2, float angle2,
                           Intersection *intersect){
    switch (shape1->type) {
    case CIRCLE:
        switch (shape2->type) {
        case CIRCLE:
            return collision_C_C(shape1,p1,angle1,shape2,p2,angle2,intersect);
        case POLYGON:
            return collision_C_P(shape1,p1,angle1,shape2,p2,angle2,intersect);
        default:
            break;
        }
        break;
    case POLYGON:
        switch (shape2->type) {
        case CIRCLE:
            return collision_C_P(shape2,p2,angle2,shape1,p1,angle1,intersect);
        case POLYGON:
            return collision_P_P(shape1,p1,angle1,shape2,p2,angle2,intersect);
        default:
            break;
        }
        break;
    default:
        break;
    }
    return 0;
}

int collisionBetweenObject(const Object *o1, const Object *o2, Intersection *intersect){
    int i,j;
    intersect->normal1 = makeVector(0,0);
    intersect->normal2 = makeVector(0,0);
    for(i=0;i<o1->nbShapes;i++)
    {
        for(j=0;j<o2->nbShapes;j++)
        {
            if(collisionBetweenShapes(o1->shapes+i,makePoint(o1->x,o1->y), o1->angle,
                                      o2->shapes+j,makePoint(o2->x,o2->y), o2->angle,
                                      intersect))
            {
                return 1;
            }
        }
    }
    return 0;
}

void makeObject(Object* o, int nbEffect, int nbShape,
                float life, float colliderRadius, unsigned char isStatic,
                float strenth, unsigned char isUnbreakable){
    o->angle=90;
    o->colliderRadius=colliderRadius;
    o->nbEffect=nbEffect;
    o->nbShapes=nbShape;
    o->isStatic=isStatic;
    o->isUnbreakable = isUnbreakable;
    o->effectDelays = malloc(sizeof(int)*nbEffect);
    o->effectsAtCollision = malloc(sizeof(Effect)*nbEffect);
    o->effectsTypesAtCollision = malloc(sizeof(EFFECT_TYPE)*nbEffect);
    o->life=life;
    o->strenth=strenth;
    o->receivedModifs=NULL;
    o->shapes=malloc(sizeof(Shape)*nbShape);

    o->ax = o->ay = o->aAngle = o->vAngle = o->vy = o->vx = o->x=o->y=0;
}


void setModifications(EFFECT_TYPE type, Effect effect, int delay, Intersection intersect,
                                Object* o){
    Modification* modif = malloc(sizeof(Modification));
    modif->effectType = type;
    modif->effect = effect;
    modif->initialDelay = modif->delay = delay;
    modif->intersection = intersect;
    modif->next = NULL;

    modif->next = o->receivedModifs;
    o->receivedModifs = modif;
}

int handleCollision(Object *o1, Object *o2){
    Intersection inter;
    o1->x += o1->vx/2;
    o1->y += o1->vy/2;
    o2->x += o2->vx/2;
    o2->y += o2->vy/2;
    if(collisionBetweenObject(o1,o2,&inter)){
        int i;
        if(!o1->isUnbreakable)
            o1->life -= o2->strenth;
        if(!o2->isUnbreakable)
            o2->life -= o1->strenth;
        o1->x += o1->vx/2;
        o1->y += o1->vy/2;
        o2->x += o2->vx/2;
        o2->y += o2->vy/2;
        if(o2->life>0){
            for(i=0;i<o1->nbEffect;i++){
                setModifications(o1->effectsTypesAtCollision[i],o1->effectsAtCollision[i],5,inter,o2);
            }
        }
        if(o1->life>0){
            for(i=0;i<o2->nbEffect;i++){
                setModifications(o2->effectsTypesAtCollision[i],o2->effectsAtCollision[i],5,inter,o1);
            }
        }
        return 1;
    }
    o1->x -= o1->vx/2;
    o1->y -= o1->vy/2;
    o2->x -= o2->vx/2;
    o2->y -= o2->vy/2;
    return 0;
}

int applyEffectToObject(Modification *modif, Object *o){
    float valueX,valueY;
    /*for(i=0;i<o->nbShapes;i++)
        o->shapes[i].color.r -= (0.5f/(float)modif->initialDelay);*/
    switch (modif->effectType) {
    case ACCELERATION:
        if(!o->isStatic){
            valueX = modif->intersection.normal1.x * modif->effect.acceleration.acceleration_value;
            valueY = modif->intersection.normal1.y * modif->effect.acceleration.acceleration_value;
            o->vx += valueX;
            o->vy += valueY;
        }
        else
            return 1;
        break;
    case REBOUND:
        if(!o->isStatic){
            float resistanceX = fabs(modif->effect.rebound.resistance * modif->intersection.normal1.x);
            float resistanceY = fabs(modif->effect.rebound.resistance * modif->intersection.normal1.y);
            if(resistanceX < fabs(o->vx)){
                o->vx += resistanceX;
            }
            else{
                valueX = modif->intersection.normal1.x *
                         modif->effect.rebound.rebound_value *
                         fabs(o->vx);
                o->ax = valueX;
                o->vx = o->ax;
                o->x += o->vx;
            }
            if(resistanceY < fabs(o->vy)){
                o->vy += resistanceY;
            }
            else{
                valueY = modif->intersection.normal1.y *
                         modif->effect.rebound.rebound_value *
                         fabs(o->vy);
                o->ay = valueY;
                o->vy = o->ay;
                o->y += o->vy;
            }
        }
        return 1;
    default:
        return 0;
    }
    modif->delay--;
    if(modif->delay<=0) return 1;
    return 0;
}

int applyEffectToHovercraft(Modification *modif, Hovercraft *h){
    switch (modif->effectType) {
    case POINTSPLUS:
        h->points += modif->effect.points.ammount;
        return 1;
    case POINTMINUS:
        h->points -= modif->effect.points.ammount;
        return 1;
    default:
        return 0;
    }
    modif->delay--;
    if(modif->delay<=0) return 1;
    return 0;
}
void updateObject(Object *object){
    if(object->receivedModifs!=NULL){
        Modification* current = object->receivedModifs;
        Modification* nextNext;

        while(current->next!=NULL){
            if(applyEffectToObject(current->next,object)){
                nextNext = current->next->next;
                free(current->next);
                current->next = nextNext;
            }
            else
                current = current->next;
        }
        if(applyEffectToObject(object->receivedModifs,object)){
            current = object->receivedModifs->next;
            free(object->receivedModifs);
            object->receivedModifs = current;
        }
    }

    object->angle += object->vAngle;

    if(isZero(object->vx))
        object->vx=0;
    if(isZero(object->vy))
        object->vy=0;

    object->vx += object->ax;
    object->vy += object->ay;

    object->x += object->vx;
    object->y += object->vy;
}
