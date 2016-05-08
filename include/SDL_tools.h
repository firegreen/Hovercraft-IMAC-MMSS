#ifndef SDL_TOOLS_H
#define SDL_TOOLS_H

#ifdef __APPLE__
#include <SDL.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <SDL/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#define BITS_PER_PIXEL 32
#define MILLISECOND_PER_FRAME 1000 / 60
#define GLREPERE 100.
#define GL_BRG 0x80E
#define GL_BRGA 0x801

#include "Object2D/Color.h"
#include "Object2D/geometry.h"

struct {
  float width;
  float height;
  float orthoGLX;
  float orthoGLY;
} window;

void resize_handler(float width, float height);
void reloadGLrepere();
void loadCustomViewport(int x, int y, int width, int height);
void initialize_window(float width, float height, unsigned char fullscreen);
int windowEventHandler(const SDL_Event* event);


#endif
