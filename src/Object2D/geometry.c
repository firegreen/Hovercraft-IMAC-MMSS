#include "Object2D/geometry.h"
#include <stdlib.h>
#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif
#include <stdio.h>
#include <math.h>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Point2D makePoint(float x1, float y1){
  Point2D center;
  center.x=x1;
  center.y=y1;
  return center;
}

Bounds4P makeBounds4P(float x1, float y1, float x2, float y2){
    Bounds4P retour;
    retour.leftTop.x = x1; retour.leftTop.y = y1;
    retour.leftBottom.x = x1; retour.leftBottom.y = y2;
    retour.rightTop.x = x2; retour.rightTop.y = y1;
    retour.rightBottom.x = x2; retour.rightBottom.y = y2;
    return retour;
}

<<<<<<< HEAD
Bounds2P makeBounds2P(float x1, float y1, float x2, float y2){
    Bounds2P retour;
    retour.leftTop.x = x1; retour.leftTop.y = y1;
    retour.rightBottom.x = x2; retour.rightBottom.y = y2;
=======
Bounds6F makeBounds6F(float x1, float y1, float width, float height){
    Bounds6F retour;
    retour.x1 = x1;
    retour.y1 = y1;
    retour.height = height;
    retour.width = width;
    retour.x2 = width+x1;
    retour.y2 = height+y1;
>>>>>>> master
    return retour;
}

Point2D makeTranslateAndRotate(Point2D p, float tx, float ty, float angle){
    angle -= 90.;
    float cosShape2 = cosf(angle * M_PI/180.);
    float sinShape2 = sinf(angle * M_PI/180.);
    Point2D A = makePoint(p.x * cosShape2 - p.y * sinShape2 + tx,
                          p.x * sinShape2 + p.y * cosShape2 + ty);
    return A;
}

Vector2D makeVector(float x1, float y1){
    Vector2D center;
    center.x=x1;
    center.y=y1;
    return center;
}

void normalizeVector(Vector2D* v){
    float norm = sqrtf(v->x*v->x+v->y*v->y);
    v->x /= norm;
    v->y /= norm;
}

Vector2D normalizedVector(Vector2D v){
    Vector2D retour;
    float norm = sqrtf(v.x*v.x+v.y*v.y);
    retour.x = v.x / norm;
    retour.y = v.y / norm;
    return retour;
}

Vector2D makeVectorAB(Point2D A, Point2D B){
    Vector2D AB;
    AB.x=B.x - A.x;
    AB.y=B.y - A.y;
    return AB;
}

Line makeLine(Point2D start, Point2D end){
    Line s;
    s.start = start;
    s.end = end;
    s.direction = makeVectorAB(start,end);
    //normalizeVector(&(s.direction));
    s.angle = atanf(s.direction.y/s.direction.x);
    return s;
}
