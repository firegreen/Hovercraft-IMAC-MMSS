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
#include "quadTree.h"
struct Object;

typedef struct Map {
  float width, height;
  Bounds4P bounds;
  struct Chained_Object* objects;
  QuadTree tree;
} Map;

void initMap(Map *map, float width, float height);
void addObjectToMap(Map *map, struct Object* o, Point2D position);
void updateMap(Map *map);
void drawMap(const Map* map);
#endif
