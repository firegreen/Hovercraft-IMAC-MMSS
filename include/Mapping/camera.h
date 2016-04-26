#ifndef CAMERA_H
#define CAMERA_H

#include "Object2D/geometry.h"
#include "map.h"

typedef struct Camera{
    Point2D leftTop;
    Point2D rightBottom;
    const Map* map;
} Camera;

struct Hovercraft;

void makeCamera(Camera* c, float width, float height, const Map *map);
void updateViewOfHovercraft(struct Hovercraft* hovercraft);
void applyCameraTransform(const Camera* c, float viewGLX1, float viewGLY1, float viewGLX2, float viewGLY2);

#endif
