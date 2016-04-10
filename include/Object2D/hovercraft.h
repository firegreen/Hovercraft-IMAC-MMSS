#ifndef HOVERCRAFT_H
#define HOVERCRAFT_H

#include "object.h"
#include "Mapping/camera.h"

typedef struct Hovercraft{
  Object physical_body;

  float max_speed;
  float vx,vy;
  float ax,ay;
  float moteur;

  unsigned int points;
  float rotationSpeed;
  float rotationAcceleration;
  float linearBoost;
  int rotationBoost;
  Camera view;
} Hovercraft;

void initHovercraft(Hovercraft* hovercraft);
void drawHovercraft(const Hovercraft* hovercraft);
void updateHovercraft(Hovercraft* hovercraft);
void updateView(Hovercraft* hovercraft);


#endif
