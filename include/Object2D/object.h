#ifndef OBJECT_H
#define OBJECT_H

#include "geometry.h"
#include "Color.h"

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif


typedef enum EFFECT_TYPE {MAX_SPEED, ACCELERATION,
                          REBOUND, SPEED, POINTSMODIF, FESTIVAL} EFFECT_TYPE;
typedef enum SHAPE_TYPE {POLYGON, SEGMENT, CIRCLE} SHAPE_TYPE;

struct Level;
typedef union Effect {
  struct { int ammount; } points;
  struct { float acceleration_value;} acceleration;
  struct { float resistance; float rebound_value;} rebound;
  struct { float force_value;} force;
  struct { float max_speed_value; } maxSpeed;
  struct { struct Level* level; } festival;
  /* and other...*/
} Effect;

typedef struct Shape{
    union {
      struct { Point2D* points; int full; unsigned int nbPoints;} polygon;
      struct { Point2D p1,p2;} segment;
      struct { Point2D center; float radius; int full;} circle;
      /* and other...*/
    } spec;
    Color4f color;
    SHAPE_TYPE type;
    GLuint textureID;
    unsigned int nbTexturePoints;
    TexturePoint2D* texturesPoints;
} Shape;

typedef struct Modification{
  EFFECT_TYPE effectType;
  Effect effect;
  int initialDelay;
  int delay;
  Intersection intersection;
  struct Modification* next;
} Modification;

typedef struct Object{
  EFFECT_TYPE* effectsTypesAtCollision;
  Effect* effectsAtCollision;
  int* effectDelays;
  Shape* shapes;
  unsigned int nbShapes;
  unsigned int nbEffect;

  float x,y;
  float vx,vy;
  float ax,ay;
  float angle;
  float vAngle;
  float aAngle;

  unsigned char isStatic;
  unsigned char isUnbreakable;

  float colliderRadius;
  float life;
  float strenth;

  Modification* receivedModifs;
} Object;

struct Hovercraft;

void makePolygon(Shape* shape, unsigned int nbPoints, Color4f color, int full);
void makePolygonWithTexture(Shape* shape, unsigned int nbPoints, Color4f color,
                            GLuint textureID, unsigned int nbTexPoints);
void setPolygonPoint(Shape* shape, int pointIndice, Point2D p);
void setPolygonTexturePoint(Shape* shape, int pointIndice, TexturePoint2D p);
void makeRectangle(Shape* shape, float x, float y, float width,
                   float height, Color4f color, int full);
void makeRectangleWithTexture(Shape *shape, float x, float y, float width, float height,
                              Color4f color, GLuint textureID, float texWidth, float texHeight, char realsize);
void makeInversedRectangle(Shape* shape, float x, float y, float width, float height,
                           Color4f color, int full);
void makeInversedRectangleWithTexture(Shape* shape, float x, float y, float width, float height,
                           Color4f color, GLuint textureID, float texWidth, float texHeight);
void makeCircle(Shape* shape, float r, Point2D c, Color4f color, int full);
void makeCircleWithTexture(Shape* shape, float r, Point2D c, Color4f color,
                           GLuint textureID, float textureWidth, float textureHeight);
void makeSegment(Shape* shape, Point2D p1, Point2D p2, Color4f color);
void cpyShape(Shape* s1, const Shape* s2);
void setModifications(Object* sender, Object *reveiver, EFFECT_TYPE type, Effect effect, int delay, Intersection intersect);
void removeModifications(Object *object, Modification **modif);
void makeObject(Object* o, int nbEffect, int nbShape, float life, float colliderRadius,
                unsigned char isStatic, float strenth, unsigned char isUnbreakable);
void cpyObject(Object* o1, const Object* o2);
void drawMiniShape(const Shape* shape);
void drawShape(const Shape* shape);
void drawShapeWithTexture(const Shape* shape);
void drawMiniObject(const Object* object);
void drawObject(const Object* object);
void updateObject(Object* object);
void freeObject(Object** o);


int handleCollision(Object* o1, Object* o2);
int collisionBetweenLineAndObject(Line l, const Object* o2, Intersection* intersect);
int collisionBetweenObjectCollider(const Object* o1, const Object* o2);
int collisionBetweenObject(const Object* o1, const Object* o2, Intersection* intersect);
int collisionBetweenShapes(const Shape* shape1, Point2D p1, float angle1, Vector2D direction1,
                           const Shape* shape2, Point2D p2, float angle2,
                           Intersection* intersect);

typedef struct EffectStatus {
    unsigned char destroyEffect;
    unsigned char destroyObject;
}EffectStatus;

struct Hovercraft;

int applyEffectToObject(Modification* modif, Object* o);
int applyEffectToHovercraft(Modification* modif, struct Hovercraft* h);
#endif
