#ifndef LEVEL_H
#define LEVEL_H

#include "Mapping/map.h"
#include "Object2D/hovercraft.h"

typedef struct Level{
  Map map;
  int nbPlayers;
  Hovercraft* players;
} Level;

void drawLevel(const Level* level);
void updateLevel(Level* level);

#endif
