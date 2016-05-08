#ifndef GAME_H
#define GAME_H
#define NBLEVELS 10
#define NBJOYSTICK 4
#define NBMODE 3

#include "GUI/level.h"
#include "GUI/mode.h"
#include "GUI/control.h"
#include "Mapping/camera.h"
#include "SDL_tools.h"
#include "GUI/menu.h"

typedef union ModeStruct {
    Level level;
    Menu menu;
} ModeStruct;

struct Game{

  Mode currentMode;
  ModeStruct* currentModeStruct;
  int windowWidth, windowHeight;
  unsigned char fullscreen;
  Camera currentCamera;
  Joystick joysticks[NBJOYSTICK];
} Game;

void drawGame();
void update();
int handleEvent();
void transition(Mode nextMode);
void initializeGame();
void game();

#endif
