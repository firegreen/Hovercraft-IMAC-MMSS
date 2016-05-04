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
#include "quadTree.h"

typedef enum READERROR { EOFREACHED, BADFORMATMAP,BADFORMATOBJECT, BADFORMATSHAPE, BADFORMATEFFECT,
                         FILENOTEXIST, NOERROR } ReadError;

typedef struct Item {
    Object object;
    float freq;
} Item;

typedef struct Map {
  float width, height;
  Bounds4P bounds;
  struct Chained_Object* objects;
  QuadTree tree;
  Object physicalBounds;
  Color3f color;
  float Bcolorevolution;
  float Gcolorevolution;
  float frottement;
  float state;
  int nbItems;
  Item* potentialItem;
  Item specialItem;
  float itemState;
} Map;

void initMap(Map *map, float width, float height, float frottement, int textureID);
void initMapItems(Map *map, int nbItems);
void addObjectToMap(Map *map, Object* o, Point2D position);
void setItem(Map* map, Item item, int indice);
void updateMap(Map *map);
void applyFrottement(const Map* map, Object* o);
void drawMap(const Map* map);
void readFile(Map *m, char* path, int *error);

#endif
