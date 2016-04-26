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
typedef enum { MAINAUDIO1=0,MAINAUDIO2,MAINAUDIO3,ACCAUDIO,COLISIONAUDIO, NBAUDIOS} AudioID;
typedef enum { ONETEXTURE=0, TWOTEXTURE, THREETEXTURE, NBTEXTURES} TextureID;

typedef union ModeStruct {
    Level level;
<<<<<<< HEAD
    //caMenu menu;
=======
    Menu menu;
>>>>>>> master
} ModeStruct;

struct Game{
  Mode currentMode;
  ModeStruct* currentModeStruct;
  int windowWidth, windowHeight;
  unsigned char fullscreen;
  unsigned char levelIsUnLocked[NBLEVELS];
  unsigned char modeIsUnlocked[NBMODE];
  Camera currentCamera;
  int audioIDs[NBAUDIOS];
  GLuint textureIDs[NBTEXTURES];
  Joystick joysticks[NBJOYSTICK];
  float specialState;
  unsigned char specialMode;
} Game;

void drawGame();
void update();
int handleEvent();
void initializeGame();
void game();
void loadConfig();
void saveConfig();

#endif
