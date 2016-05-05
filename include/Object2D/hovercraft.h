#ifndef HOVERCRAFT_H
#define HOVERCRAFT_H

#include "Mapping/camera.h"
#include "Object2D/object.h"
#include "SDL/SDL.h"

typedef struct DirectionStack {
    Vector2D direction;
    struct DirectionStack* below;
} DirectionStack;

void freeDirectionStack(DirectionStack* d);
DirectionStack* makeDirectionStack(Vector2D direction);
DirectionStack* addDirection(DirectionStack* d, Vector2D newDirection);

typedef enum { ZOOMMINUS=0, ZOOMPLUS, FORWARD, BACKWARD, FORWARDSTOP, BACKWARDSTOP,
               ROTATELEFT, ROTATERIGHT, ROTATELEFTSTOP, ROTATERIGHTSTOP, NBACTIONS} HovercraftControlID;

typedef struct Hovercraft{
  struct Object physical_body;

  unsigned int id;

  float max_linear_speed;
  float moteur;

  float max_rotation_speed;

  unsigned int points;
  float linearAccelerate;
  float rotationAccelerate;
  Camera view;

  Vector2D direction;

  SDL_Event* actionsEvents;

} Hovercraft;

typedef struct BotHovercraft {
  Hovercraft h;
  DirectionStack* path;
  Point2D goal;
  float error;
} BotHovercraft;

void initHovercraft(Hovercraft* hovercraft, const Map *map, unsigned int id,
                    Point2D viewLeftTop, float viewWidth, float viewHeight, SDL_Event *actionsEvents);
void initBotHovercraft(BotHovercraft *hovercraft, const Map* map, float error,
                       Point2D viewLeftTop, float viewWidth, float viewHeight);
void drawHovercraft(const Hovercraft* hovercraft);
void updateHovercraft(Hovercraft* hovercraft);
void updateBotHovercraft(BotHovercraft* hovercraft, Chained_Object *objects);
void giveGoalToBot(BotHovercraft* hovercraft, Point2D goal);
int handleEventHovercraft(Hovercraft* hovercraft, const SDL_Event* e);


#endif
