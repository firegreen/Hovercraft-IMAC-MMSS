#ifndef LEVEL_H
#define LEVEL_H

#include "Mapping/map.h"
#include "Object2D/hovercraft.h"
#ifdef __APPLE__
#include <SDL.h>
#else
#include <SDL/SDL.h>
#endif

typedef struct Level{
  Map map;
  Hovercraft* hovercraft;
  int win;
  unsigned int endStart;
} Level;

void drawLevel(const Level* level);
void updateLevel(Level* level);
void initLevel(Level* level, int mapid);
void handleEventLevel(Level* l, const SDL_Event* event);

#endif
