#ifndef GEOMETRY_H
#define GEOMETRY_H

#define CLAMP(x, min, max) MAX(min, MIN(max, x))
#define EPSILON 0.0005
#define PI 3.14
#define isZero(number) (number > -EPSILON && number < EPSILON)


typedef struct Point2D {
  float x;
  float y;
} Point2D;

typedef Point2D Vector2D;

typedef struct Bounds4F{
  float x,y;
  float width,height;
} Bounds4F;

typedef struct Bounds6F{
    float x1,y1,x2,y2;
    float width,height;
}Bounds6F;

typedef struct Bounds2P{
  Point2D leftTop;
  Point2D rightBottom;
} Bounds2P;

typedef struct Bounds4P{
  Point2D leftTop;
  Point2D rightBottom;
  Point2D leftBottom;
  Point2D rightTop;
} Bounds4P;

typedef struct Intersection{
    Point2D point;
    Vector2D normal1;
    Vector2D normal2;
} Intersection;

typedef struct Line{
    Point2D start;
    Point2D end;
    Vector2D direction;
    float angle;
}Line;

Point2D makePoint(float x1, float y1);
Vector2D makeVector(float x1, float y1);
Vector2D normalizedVector(Vector2D v);
void normalizeVector(Vector2D* v);
Vector2D makeVectorAB(Point2D A, Point2D B);
Point2D makeTranslateAndRotate(Point2D p, float tx, float ty, float angle);
Bounds4P makeBounds4P(float x1, float y1, float x2, float y2);
Bounds2P makeBounds2P(float x1, float y1, float x2, float y2);
Bounds6F makeBounds6F(float x1, float y1, float width, float height);

#endif //GEOMETRY_H
