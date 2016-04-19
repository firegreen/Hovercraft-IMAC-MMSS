#ifndef MODE_H
#define MODE_H

#include "geometry.h"

typedef enum Mode {MODE_MAINMENU, MODE_LEVELMENU, MODE_LEVEL} Mode;



void drawMode(Mode mode);
void updateMode(Mode mode);
