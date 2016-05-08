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
#include "textures.h"
#include "GUI/level.h"

#define M_PI 3.14159265358979323846


void drawShape(const Shape* shape){
    int i;
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(shape->color.r,shape->color.g,shape->color.b,shape->color.a);
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
            glVertex2f(cosf((float)i*2.*M_PI/50.)*shape->spec.circle.radius+shape->spec.circle.center.x,
                       sinf((float)i*2.*M_PI/50.)*shape->spec.circle.radius+shape->spec.circle.center.y);
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
    glDisable(GL_BLEND);
}

void drawMiniShape(const Shape* shape){
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
        for(i=0; i<=20; i++)
        {
            glVertex2f(cosf((float)i*2.*M_PI/20.)*shape->spec.circle.radius+shape->spec.circle.center.x,
                       sinf((float)i*2.*M_PI/20.)*shape->spec.circle.radius+shape->spec.circle.center.y);
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

void drawShapeWithTexture(const Shape* shape){
    int i=0;
    int iTexture=0;
    if(shape->nbTexturePoints<=0) {
        drawShape(shape);
        return;
    }
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, shape->textureID);
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(shape->color.r,shape->color.g,shape->color.b,shape->color.a);
    switch (shape->type) {
    case POLYGON:
        glBegin(GL_POLYGON);
        for(i=0;i<shape->spec.polygon.nbPoints;i++)
        {
            if(shape->texturesPoints[iTexture].indice==i) {
                glTexCoord2f(shape->texturesPoints[iTexture].x,shape->texturesPoints[iTexture].y);
                iTexture++;
            }
            glVertex2f(shape->spec.polygon.points[i].x,shape->spec.polygon.points[i].y);
        }
        glEnd();
        break;
    case CIRCLE:
        glBegin(GL_POLYGON);
        int j;
        for(j=0;j<shape->nbTexturePoints;j++){
            for(; i<=12*(j+1); i++)
            {
                glTexCoord2f(cosf((float)i*2.*M_PI/(shape->nbTexturePoints*12.))*0.5+0.5,
                             sinf((float)i*2.*M_PI/(shape->nbTexturePoints*12.))*0.5+0.5);
                glVertex2f(cosf((float)i*2.*M_PI/(shape->nbTexturePoints*12.))*shape->spec.circle.radius+shape->spec.circle.center.x,
                           sinf((float)i*2.*M_PI/(shape->nbTexturePoints*12.))*shape->spec.circle.radius+shape->spec.circle.center.y);
            }
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
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
}

void drawObject(const Object* object){
    int i = 0;
    glPushMatrix();
    glTranslatef(object->x,object->y,0);
    glRotatef(object->angle-90.,0,0,1.);
    for(;i<object->nbShapes;i++)
    {
        drawShapeWithTexture(object->shapes+i);
    }
    glPopMatrix();
}

void drawMiniObject(const Object* object){
    int i = 0;
    glPushMatrix();
    glTranslatef(object->x,object->y,0);
    if(object->nbEffect>0){
        switch (object->effectsTypesAtCollision[0]) {
        case REBOUND:
            glColor3f(0.5,0.35,0.6);
            break;
        case POINTSMODIF:
            if(object->effectsAtCollision->points.ammount>0)
                glColor3f(0.2,1,0.6);
            else
                glColor3f(1,0.1,0.3);
            break;
        case ACCELERATION:
            glColor3f(0.2,0.3,0.5);
            break;
        default:
            glColor3f(0.5,0.7,0.43);
            break;
        }
    }
    else{
        glColor4f(0.5,0.5,0.5,0.5);
    }
    for(;i<object->nbShapes;i++)
    {
        drawMiniShape(object->shapes+i);
    }
    glPopMatrix();
}

void makePolygon(Shape *shape, unsigned int nbPoints, Color4f color, int full){
    shape->type = POLYGON;
    shape->spec.polygon.nbPoints = nbPoints;
    shape->spec.polygon.points = malloc(sizeof(Point2D)*shape->spec.polygon.nbPoints);
    shape->color = color;
    shape->spec.polygon.full = full;
    shape->nbTexturePoints = 0;
    shape->texturesPoints = NULL;
    shape->textureID = 0;
}
void makePolygonWithTexture(Shape *shape, unsigned int nbPoints, Color4f color, GLuint textureID,
                            unsigned int nbTexPoints){
    makePolygon(shape,nbPoints,color,1);
    shape->nbTexturePoints = nbTexPoints;
    shape->texturesPoints = malloc(sizeof(TexturePoint2D)*shape->nbTexturePoints);
    shape->textureID = textureID;
}

void setPolygonPoint(Shape* shape, int pointIndice, Point2D p) {
    shape->spec.polygon.points[pointIndice] = p;
}

void setPolygonTexturePoint(Shape *shape, int pointIndice, TexturePoint2D p){
    shape->texturesPoints[pointIndice] = p;
}

void makeRectangle(Shape* shape, float x,float y,float width,float height, Color4f color, int full){
    shape->type = POLYGON;
    shape->spec.polygon.nbPoints = 4;
    shape->spec.polygon.points = malloc(sizeof(Point2D)*shape->spec.polygon.nbPoints);
    shape->spec.polygon.points[0].x = x;
    shape->spec.polygon.points[0].y = y;
    shape->spec.polygon.points[1].x = x;
    shape->spec.polygon.points[1].y = y-height;
    shape->spec.polygon.points[2].x = x+width;
    shape->spec.polygon.points[2].y = y-height;
    shape->spec.polygon.points[3].x = x+width;
    shape->spec.polygon.points[3].y = y;
    shape->color = color;
    shape->spec.polygon.full = full;
    shape->textureID = 0;
    shape->nbTexturePoints = 0;
    shape->texturesPoints = NULL;
}

void makeRectangleWithTexture(Shape *shape, float x, float y, float width,
                              float height, Color4f color, GLuint textureID, float textureWidth,
                              float textureHeight, char realsize){
    makeRectangle(shape,x,y,width,height,color,1);
    shape->textureID = textureID;
    shape->nbTexturePoints = 4;
    shape->texturesPoints = malloc(shape->nbTexturePoints*sizeof(TexturePoint2D));
    float endX, endY;
    if(realsize){
        endX = width/textureWidth;
        endY = height/textureHeight;
    }
    else{
        endX = 1;
        endY = 1;
    }
    int i;
    for(i=0;i<shape->nbTexturePoints;i++)
        shape->texturesPoints[i].indice = i;
    shape->texturesPoints[0].x = 0;
    shape->texturesPoints[0].y = 0;
    shape->texturesPoints[1].x = 0;
    shape->texturesPoints[1].y = endY;
    shape->texturesPoints[2].x = endX;
    shape->texturesPoints[2].y = endY;
    shape->texturesPoints[3].x = endX;
    shape->texturesPoints[3].y = 0;
}

void makeInversedRectangle(Shape* shape, float x,float y,float width,float height,
                           Color4f color, int full){
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
    shape->color = color;
    shape->spec.polygon.full = full;
    shape->nbTexturePoints = 0;
    shape->texturesPoints = NULL;
}

void makeInversedRectangleWithTexture(Shape *shape, float x, float y, float width, float height,
                                      Color4f color, GLuint textureID, float texWidth, float texHeight)
{
    makeInversedRectangle(shape,x,y,width,height,color,1);
    shape->textureID = textureID;
    shape->nbTexturePoints = 4;
    shape->texturesPoints = malloc(shape->nbTexturePoints*sizeof(TexturePoint2D));

    float endX = width/texWidth, endY=height/texHeight;
    int i;
    for(i=0;i<shape->nbTexturePoints;i++)
        shape->texturesPoints[i].indice = i;
    shape->texturesPoints[0].x = 0;
    shape->texturesPoints[0].y = 0;
    shape->texturesPoints[1].x = 0;
    shape->texturesPoints[1].y = endY;
    shape->texturesPoints[2].x = endX;
    shape->texturesPoints[2].y = endY;
    shape->texturesPoints[3].x = endX;
    shape->texturesPoints[3].y = 0;
}


void makeCircle(Shape* shape, float r, Point2D c, Color4f color, int full){
    shape->type = CIRCLE;
    shape->spec.circle.center = c;
    shape->spec.circle.radius = r;
    shape->color = color;
    shape->spec.circle.full = full;
    shape->nbTexturePoints = 0;
    shape->texturesPoints = NULL;
    shape->textureID = 0;
}

void makeCircleWithTexture(Shape *shape, float r, Point2D c, Color4f color,
                           GLuint textureID, float textureWidth, float textureHeight){
    makeCircle(shape,r,c,color,1);
    shape->nbTexturePoints = 4;
    shape->texturesPoints = malloc(sizeof(TexturePoint2D)*shape->nbTexturePoints);
    shape->textureID = textureID;
    float endX = r*2/textureWidth, endY=r*2/textureHeight;
        int i;
    for(i=0;i<shape->nbTexturePoints;i++)
        shape->texturesPoints[i].indice = i;
    shape->texturesPoints[0].x = 0;
    shape->texturesPoints[0].y = 0;
    shape->texturesPoints[1].x = 0;
    shape->texturesPoints[1].y = endY;
    shape->texturesPoints[2].x = endX;
    shape->texturesPoints[2].y = endY;
    shape->texturesPoints[3].x = endX;
    shape->texturesPoints[3].y = 0;
}

void makeSegment(Shape *shape, Point2D p1, Point2D p2, Color4f color){
    shape->type = SEGMENT;
    shape->spec.segment.p1 = p1;
    shape->spec.segment.p2 = p2;
    shape->color = color;
    shape->nbTexturePoints = 0;
    shape->texturesPoints = NULL;
}

int collision_C_C(const Shape* shape1, Point2D p1, float angle1, Vector2D direction,
                  const Shape* shape2, Point2D p2, float angle2, Intersection* intersect){
    Point2D C1 = makeTranslateAndRotate(shape1->spec.circle.center,
                                        p1.x+direction.x,p1.y+direction.y,angle1);
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

int collision_C_S(Point2D C, float radius, Point2D A, Point2D B,Intersection* intersect){
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
            intersect->normal1 = makeVector(AB.y,-AB.x);
            normalizeVector(&(intersect->normal1));
            intersect->normal2 = makeVector(-intersect->normal1.x,-intersect->normal1.y);
            return 1;
        }
    }
    return 0;
}

int collision_S_C(Point2D A, Point2D B, Point2D C, float radius, Intersection* intersect){
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
        float t;
        if((t1 <= 0. && t2 >=1.) || (t2 <= 0. && t1 >=1.))
            t = 0.5;
        else if (t2>=0. && t2 <=1.){
            t = t2;
        }
        else if(t1 >=0. && t1<=1.){
            t = t1;
        }
        else return 0;
        Point2D intersectPoint;
        intersectPoint = makePoint(A.x + AB.x*t,
                                   A.y + AB.y*t);
        intersect->normal1 = makeVectorAB(C,intersectPoint);
        normalizeVector(&(intersect->normal1));
        intersect->normal2 = makeVector(-intersect->normal1.x,-intersect->normal1.y);
        return 1;
    }
    else if(isZero(delta)){
        float t = (-b)/(2.*a);
        if(t>=0. && t<=1.){
            Point2D intersectPoint;
            intersectPoint = makePoint(A.x + AB.x*t,
                                       A.y + AB.y*t);
            intersect->normal1 = makeVectorAB(C,intersectPoint);
            normalizeVector(&(intersect->normal1));
            intersect->normal2 = makeVector(-intersect->normal1.x,-intersect->normal1.y);
            return 1;
        }
    }
    return 0;
}


int collision_C_P(const Shape* shape1, Point2D p1, float angle1, Vector2D direction,
                  const Shape* shape2, Point2D p2, float angle2, Intersection* intersect){
    int i = 0;
    Point2D C = makeTranslateAndRotate(shape1->spec.circle.center,
                                       p1.x+direction.x,p1.y+direction.y,angle1), A, B;
    for(;i<shape2->spec.polygon.nbPoints-1;i++)
    {
        A = makeTranslateAndRotate(shape2->spec.polygon.points[i],p2.x,p2.y,angle2);
        B = makeTranslateAndRotate(shape2->spec.polygon.points[i+1],p2.x,p2.y,angle2);
        if(collision_C_S(C,shape1->spec.circle.radius,A,B,intersect))
            return 1;
    }
    A = makeTranslateAndRotate(shape2->spec.polygon.points[shape2->spec.polygon.nbPoints-1],p2.x,p2.y,angle2);
    B = makeTranslateAndRotate(shape2->spec.polygon.points[0],p2.x,p2.y,angle2);
    return collision_C_S(C,shape1->spec.circle.radius,A,B,intersect);
}
int collision_P_C(const Shape* shape1, Point2D p1, float angle1, Vector2D direction,
                  const Shape* shape2, Point2D p2, float angle2, Intersection* intersect){
    int i = 0;
    Point2D C = makeTranslateAndRotate(shape2->spec.circle.center,
                                       p2.x,p2.y,angle2), A, B;
    p1.x += direction.x;
    p1.y += direction.y;
    for(;i<shape1->spec.polygon.nbPoints-1;i++)
    {
        A = makeTranslateAndRotate(shape1->spec.polygon.points[i],p1.x,p1.y,angle1);
        B = makeTranslateAndRotate(shape1->spec.polygon.points[i+1],p1.x,p1.y,angle1);
        if(collision_S_C(A,B,C,shape2->spec.circle.radius,intersect))
            return 1;
    }
    A = makeTranslateAndRotate(shape1->spec.polygon.points[shape1->spec.polygon.nbPoints-1],p1.x,p1.y,angle1);
    B = makeTranslateAndRotate(shape1->spec.polygon.points[0],p1.x,p1.y,angle1);
    return collision_S_C(A,B,C,shape2->spec.circle.radius,intersect);
}

int collision_S_V(Point2D P1, Point2D P2, Point2D A, Point2D B, Intersection* intersect){
    Vector2D vAP1,vAP2,vAB;
    vAB = makeVectorAB(A,B);
    vAP1 = makeVectorAB(A,P1);
    vAP2= makeVectorAB(A,P2);
    float d1 = vAB.x*vAP1.y - vAB.y*vAP1.x;
    float d2 = vAB.x*vAP2.y - vAB.y*vAP2.x;
    if(d1>=0 || d2>=0){
        intersect->normal1 = makeVector(vAB.y,-vAB.x);
        normalizeVector(&intersect->normal1);
        intersect->normal2 = makeVector(-intersect->normal1.x,-intersect->normal1.y);
        return 1;
    }
    return 0;
}

int collision_D_S(Point2D A,Point2D B,Point2D P1,Point2D P2)
{
    Vector2D vAP1,vAP2,vAB;
    vAB = makeVectorAB(A,B);
    vAP1 = makeVectorAB(A,P1);
    vAP2= makeVectorAB(A,P2);
    float d1 = vAB.x*vAP1.y - vAB.y*vAP1.x;
    float d2 = vAB.x*vAP2.y - vAB.y*vAP2.x;
    return d1*d2<0;
}

int collision_S_S(Point2D A,Point2D B, Point2D O,Point2D P, Intersection* intersect)
{
    if(!collision_D_S(A,B,O,P))
        return 0;
  Vector2D v0P,vAB;
  vAB = makeVectorAB(A,B);
  v0P = makeVectorAB(O,P);
  float k = -(A.x*v0P.y-O.x*v0P.y-v0P.x*A.y+v0P.x*O.y)/(vAB.x*v0P.y-vAB.y*v0P.x);
  if (k>0. && k<1.){
     intersect->normal1 = makeVector(v0P.y,-v0P.x);
     normalizeVector(&intersect->normal1);
     intersect->normal2 = makeVector(-intersect->normal1.x,-intersect->normal1.y);
     return 1;
  }
  else{
     return 0;
  }
}

int collision_S_P(Point2D A1,Point2D B1, const Shape *shape2, Point2D p2, float angle2,
                  Intersection* intersect){
    Point2D A2,B2;
    int j;
    for (j=0; j<shape2->spec.polygon.nbPoints-1; j++)
    {
        A2  = makeTranslateAndRotate(shape2->spec.polygon.points[j],p2.x,p2.y,angle2);
        B2  = makeTranslateAndRotate(shape2->spec.polygon.points[j+1],p2.x,p2.y,angle2);
        if(collision_S_S(A1,B1,A2,B2,intersect)){
            return 1;
        }
    }
    A2  = makeTranslateAndRotate(shape2->spec.polygon.points[j],p2.x,p2.y,angle2);
    B2  = makeTranslateAndRotate(shape2->spec.polygon.points[0],p2.x,p2.y,angle2);
    return collision_S_S(A1,B1,A2,B2,intersect);
}

int collision_P_P(const Shape *shape1, Point2D p1, float angle1, Vector2D direction,
                  const Shape *shape2, Point2D p2, float angle2, Intersection* intersect){
    int i;
    Point2D A1;
    Point2D A1PlusDirection;
    for (i=0; i<shape1->spec.polygon.nbPoints; i++)
    {
        A1 = makeTranslateAndRotate(shape1->spec.polygon.points[i],p1.x,p1.y,angle1);
        A1PlusDirection.x = A1.x + direction.x;
        A1PlusDirection.y = A1.y + direction.y;
        if(collision_S_P(A1,A1PlusDirection,shape2,p2,angle2,intersect))
            return 1;
    }
    A1 = makeTranslateAndRotate(shape1->spec.polygon.points[i],p1.x+direction.x,p1.y+direction.y,angle1);
    A1PlusDirection.x = A1.x + direction.x;
    A1PlusDirection.y = A1.y + direction.y;
    return collision_S_P(A1,A1PlusDirection,shape2,p2,angle2,intersect);
}

int collisionBetweenShapes(const Shape *shape1, Point2D p1, float angle1, Vector2D direction1,
                           const Shape *shape2, Point2D p2, float angle2, Intersection* intersect){
    switch (shape1->type) {
    case CIRCLE:
        switch (shape2->type) {
        case CIRCLE:
            return collision_C_C(shape1,p1,angle1,direction1,shape2,p2,angle2,intersect);
        case POLYGON:
            return collision_C_P(shape1,p1,angle1,direction1,shape2,p2,angle2,intersect);
        default:
            break;
        }
        break;
    case POLYGON:
        switch (shape2->type) {
        case CIRCLE:
            return collision_P_C(shape1,p1,angle1,direction1,shape2,p2,angle2,intersect);
        case POLYGON:
            return collision_P_P(shape1,p1,angle1,direction1,shape2,p2,angle2,intersect);
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
    if(o1->vx*o1->vx + o1->vy*o1->vy > EPSILON || o1->vAngle>1){
        int i,j;
        intersect->normal1 = makeVector(0,0);
        intersect->normal2 = makeVector(0,0);
        Vector2D direction;
        direction = makeVector(o1->vx,o1->vy);
        for(i=0;i<o1->nbShapes;i++)
        {
            for(j=0;j<o2->nbShapes;j++)
            {
                if(collisionBetweenShapes(o1->shapes+i,makePoint(o1->x,o1->y), o1->angle, direction,
                                          o2->shapes+j,makePoint(o2->x,o2->y), o2->angle, intersect))
                {
                    return 1;
                }
            }
        }
    }
    return 0;
}

int collisionBetweenLineAndObject(Line l, const Object* o2, Intersection* intersect){
    int j;
    intersect->normal1 = makeVector(0,0);
    intersect->normal2 = makeVector(0,0);
    for(j=0;j<o2->nbShapes;j++)
    {
        switch (o2->shapes[j].type) {
        case CIRCLE:
            if(collision_S_C(l.start,l.end,makeTranslateAndRotate(o2->shapes[j].spec.circle.center,o2->x,o2->y,o2->angle),
                                 o2->shapes[j].spec.circle.radius,intersect))
                return 1;
            break;
        case POLYGON:
            if(collision_S_P(l.start,l.end,o2->shapes+j,makePoint(o2->x,o2->y),o2->angle,intersect))
                return 1;
            break;
        default:
            break;
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

void cpyShape(Shape *s1, const Shape *s2){
    int i;
    switch (s2->type) {
    case CIRCLE:
        makeCircle(s1,s2->spec.circle.radius,s2->spec.circle.center,s2->color, s2->spec.circle.full);
        break;
    case POLYGON:
        makePolygon(s1,s2->spec.polygon.nbPoints,s2->color,s2->spec.polygon.full);
        for(i=0;i<s1->spec.polygon.nbPoints;i++){
            setPolygonPoint(s1,i,s2->spec.polygon.points[i]);
        }
    default:
        break;
    }
    s1->nbTexturePoints = s2->nbTexturePoints;
    s1->texturesPoints = malloc(sizeof(TexturePoint2D)*s1->nbTexturePoints);
    s1->textureID = s2->textureID;
    for(i=0;i<s1->nbTexturePoints;i++){
        s1->texturesPoints[i].indice = s2->texturesPoints[i].indice;
        s1->texturesPoints[i].x = s2->texturesPoints[i].x;
        s1->texturesPoints[i].y = s2->texturesPoints[i].y;
    }
}

void cpyObject(Object* o1, const Object* o2){
    o1->angle=o2->angle;
    o1->colliderRadius=o2->colliderRadius;
    o1->nbEffect=o2->nbEffect;
    o1->nbShapes=o2->nbShapes;
    o1->isStatic=o2->isStatic;
    o1->isUnbreakable = o2->isUnbreakable;
    o1->effectDelays = malloc(sizeof(int)*o1->nbEffect);
    o1->effectsAtCollision = malloc(sizeof(Effect)*o1->nbEffect);
    o1->effectsTypesAtCollision = malloc(sizeof(EFFECT_TYPE)*o1->nbEffect);
    o1->life=o2->life;
    o1->strenth=o1->strenth;
    o1->receivedModifs=NULL;
    o1->shapes=malloc(sizeof(Shape)*o1->nbShapes);
    o1->ax = o1->ay = o1->aAngle = o1->vAngle = o1->vy = o1->vx = o1->x=o1->y=0;
    int i;
    for(i=0;i<o1->nbShapes;i++){
        cpyShape(o1->shapes+i,o2->shapes+i);
    }
    for(i=0;i<o1->nbEffect;i++){
        o1->effectDelays[i] = o2->effectDelays[i];
        o1->effectsAtCollision[i] = o2->effectsAtCollision[i];
        o1->effectsTypesAtCollision[i] = o2->effectsTypesAtCollision[i];
    }
}


void setModifications(Object* sender, Object* reveiver,
                        EFFECT_TYPE type, Effect effect, int delay, Intersection intersect){
    Modification* modif = malloc(sizeof(Modification));
    modif->effectType = type;
    modif->effect = effect;
    if(modif->effectType==REBOUND)
        modif->effect.rebound.rebound_value *= (1+(sender->vx*sender->vx+sender->vy*sender->vy)/3.);
    modif->initialDelay = modif->delay = delay;
    modif->intersection = intersect;
    modif->next = NULL;

    modif->next = reveiver->receivedModifs;
    reveiver->receivedModifs = modif;
}

void removeModifications(Object *object, Modification **modif){
    if((*modif)->effectType==REBOUND){
        object->ax = 0;
        object->ay = 0;
    }
    Modification* next = (*modif)->next;
    free(*modif);
    (*modif) = next;
}

int handleCollision(Object *o1, Object *o2){
    Intersection inter;
    if(collisionBetweenObject(o1,o2,&inter)){
        int i;
        if(!o1->isUnbreakable)
            o1->life -= o2->strenth;
        if(!o2->isUnbreakable)
            o2->life -= o1->strenth;
        if(o1->life>0){
            for(i=0;i<o2->nbEffect;i++){
                setModifications(o2,o1,o2->effectsTypesAtCollision[i],o2->effectsAtCollision[i],
                                 o2->effectDelays[i],inter);
            }
        }
        if(o2->life>0){
            inter.normal1 = inter.normal2;
            for(i=0;i<o1->nbEffect;i++){
                setModifications(o1,o2,o1->effectsTypesAtCollision[i],o1->effectsAtCollision[i],
                                 o1->effectDelays[i],inter);
            }
        }
        return 1;
    }
    else if(collisionBetweenObject(o2,o1,&inter)){
        int i;
        if(!o1->isUnbreakable)
            o1->life -= o2->strenth;
        if(!o2->isUnbreakable)
            o2->life -= o1->strenth;
        if(o2->life>0){
            for(i=0;i<o1->nbEffect;i++){
                setModifications(o1,o2,o1->effectsTypesAtCollision[i],o1->effectsAtCollision[i],
                                 o1->effectDelays[i],inter);
            }
        }
        if(o1->life>0){
            inter.normal1 = inter.normal2;
            for(i=0;i<o2->nbEffect;i++){
                setModifications(o2,o1,o2->effectsTypesAtCollision[i],o2->effectsAtCollision[i],
                                 o2->effectDelays[i],inter);
            }
        }
        return 1;
    }
    return 0;
}

int applyEffectToObject(Modification *modif, Object *o){
    float valueX,valueY;
    switch (modif->effectType) {
    case ACCELERATION:
        if(!o->isStatic){
            valueX = modif->intersection.normal2.x * modif->effect.acceleration.acceleration_value;
            valueY = modif->intersection.normal2.y * modif->effect.acceleration.acceleration_value;
            o->vx += valueX;
            o->vy += valueY;
        }
        else
            return 1;
        break;
    case REBOUND:
        if(!o->isStatic){
            float resistanceX = modif->effect.rebound.resistance * modif->intersection.normal1.x;
            float resistanceY = modif->effect.rebound.resistance * modif->intersection.normal1.y;
            if(fabs(resistanceX) < fabs(o->vx))
                o->vx -= resistanceX;
            else{
                valueX = modif->intersection.normal1.x *
                         modif->effect.rebound.rebound_value *
                         (0.3+fabs(o->vx));
                o->x -= o->vx/2;
                o->vx += valueX - o->ax/2;
                o->ax = valueX;
            }
            if(fabs(resistanceY) < fabs(o->vy))
                o->vy -= resistanceY;
            else{
                valueY = modif->intersection.normal1.y *
                         modif->effect.rebound.rebound_value *
                         (0.3+fabs(o->vy));
                o->y -= o->vy/2;
                o->vy += valueY - o->ay/2;
                o->ay = valueY;
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
    case POINTSMODIF:
        h->points += modif->effect.points.ammount;
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

        while(current->next!=NULL){
            if(applyEffectToObject(current->next,object)){
                removeModifications(object,&current->next);
            }
            else
                current = current->next;
        }
        if(applyEffectToObject(object->receivedModifs,object)){
            removeModifications(object,&object->receivedModifs);
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

void freeObject(Object **o){
    free((*o)->effectDelays);
    (*o)->effectDelays = NULL;
    free((*o)->effectsAtCollision);
    (*o)->effectsAtCollision = NULL;
    free((*o)->effectsTypesAtCollision);
    (*o)->effectsTypesAtCollision = NULL;
    free(*o);
    *o = NULL;
}
