#ifndef MAP_H
#define MAP_H

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include "Object2D/geometry.h"
#include "Object2D/object.h"

typedef enum READERROR { EOFREACHED, BADFORMATMAP,BADFORMATOBJECT, BADFORMATSHAPE, BADFORMATEFFECT,
                         FILENOTEXIST, BADFORMATITEM, NONEERROR } ReadError;

typedef struct Item {
    Object object;
    float freq;
} Item;

struct Chained_Object{
    Object* object;
    Point2D position;
    struct Chained_Object* next;
};
typedef struct Chained_Object Chained_Object;
Chained_Object* makeChainedObject(struct Object* object, Chained_Object *next, Point2D position);
void freeAllChaineObject(Chained_Object **object);

typedef struct Map {
  float width, height;
  Bounds4P bounds;
  struct Chained_Object* objects;
  struct Chained_Object* items;
  struct Chained_Object* lastitem;
  int nbcurrentItem;
  Object physicalBounds;
  int audioID;
  Color3f color;
  float frottement;
  int nbItems;
  Item* potentialItem;
  float time;
  float itemState;
} Map;

void initMap(Map *map, float width, float height, float frottement, int textureID, int audioID, float time);
void freeMap(Map **map);
void freeMap2(Map *map);
void initMapItems(Map *map, int nbItems);
void addObjectToMap(Map *map, Object* o, Point2D position);
void setItem(Map* map, Item item, int indice);
void addRandomItem(Map* map);
void addItem(Map* map, int i);
void updateMap(Map *map);
void applyFrottement(const Map* map, Object* o);
void drawMap(const Map* map);
void drawMiniMap(const Map* map);
void readFile(Map *m, char* path, int *error);

#endif
