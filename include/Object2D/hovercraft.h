#ifndef HOVERCRAFT_H
#define HOVERCRAFT_H

#include "Mapping/camera.h"
#include "Object2D/object.h"
#include "SDL/SDL.h"

typedef struct Hovercraft{
  struct Object physical_body;

  float max_linear_speed;
  float moteur;

  float max_rotation_speed;

  unsigned int points;
  float linearAccelerate;
  float rotationAccelerate;
  Camera view;

  Vector2D direction;
} Hovercraft;


void initHovercraft(Hovercraft* hovercraft, const Map *map);
void drawHovercraft(const Hovercraft* hovercraft);
void drawMiniHovercraft(const Hovercraft* hovercraft);
void updateHovercraft(Hovercraft* hovercraft);
int handleEventHovercraft(Hovercraft* hovercraft, const SDL_Event* e);


#endif
