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
typedef enum { MAINAUDIO1=0,MAINAUDIO2,MAINAUDIO3,ACCAUDIO,COLISIONAUDIO } AudioID;

typedef union ModeStruct {
    Level level;
    Menu* menu;
} ModeStruct;

struct {
  Mode currentMode;
  ModeStruct* currentModeStruct;
  int windowWidth, windowHeight;
  unsigned char fullscreen;
  unsigned char levelIsUnLocked[NBLEVELS];
  unsigned char modeIsUnlocked[NBMODE];
  Camera currentCamera;
  AudioID audioIDs[NBAUDIOMAX];
  Joystick joysticks[NBJOYSTICK];

} Game;

void drawGame();
void update();
int handleEvent();
void initializeGame();
void game();
void loadConfig();
void saveConfig();

#endif
