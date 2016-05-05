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

Level levels[NBLEVELS];
typedef enum { MAINAUDIO1=0, MAINAUDIO2,MAINAUDIO3,ACCAUDIO,COLISIONAUDIO, NBAUDIOS} AudioID;

typedef union ModeStruct {
    Level level;
    Menu menu;
    OnePlayer oneplayer;
    TwoPlayers twoplayers;
    Instruction instruction;
    Credits credit;
} ModeStruct;

struct Game{

  Mode currentMode;
  Mode nextMode;
  ModeStruct* currentModeStruct;
  ModeStruct* ModeStructNext;
  int windowWidth, windowHeight;
  unsigned char fullscreen;
  unsigned char levelIsUnLocked[NBLEVELS];
  unsigned char modeIsUnlocked[NBMODE];
  Camera currentCamera;
  int audioIDs[NBAUDIOS];
  Joystick joysticks[NBJOYSTICK];
  float specialState;
  char specialMode;
  char transitionMode;
} Game;

void drawGame();
void update();
int handleEvent();
void initializeGame();
void game();
void loadConfig();
void saveConfig();

#endif
