#ifndef CAMERA_H
#define CAMERA_H

#include "Object2D/geometry.h"
#include "map.h"
#include "SDL_tools.h"
#define MINVIEWWIDTH 50

typedef struct Camera{
    Point2D leftTop;
    Point2D rightBottom;
    const Map* map;
} Camera;

struct Hovercraft;

void makeCamera(Camera* c, float width, float height, const Map *map);
void updateViewOfHovercraft(struct Hovercraft* hovercraft);
void applyCameraTransform(const Camera* c);
void zoomMinus(Camera* c);
void zoomPlus(Camera* c);
void zoom(Camera* c, float scale);
void getMousePositionInGL(const Camera* c, const SDL_Event *mouseEvent, float *x, float *y);

#endif
