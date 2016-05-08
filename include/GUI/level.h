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
  unsigned char nbPlayers;
  unsigned char nbBots;
  Hovercraft* players;
  BotHovercraft* bots;
  unsigned long state;
  unsigned int indice;
  Object* specialObject;
  char festival;
  int iWinner;
  int ms;
} Level;

void specialDrawLevel(const Level* level, float state);
int specialUpdateLevel(Level* level, float* state);
void drawLevel(const Level* level);
void updateLevel(Level* level);
void initLevel(Level* level, int mapid, int nbplayers, int nbBots);
void activeFestival(Level* l);
void handleEventLevel(Level* l, const SDL_Event* event);

#endif
