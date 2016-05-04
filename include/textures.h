#ifndef __IMAGESRC
#define __IMAGESRC
#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif
#include "Object2D/geometry.h"
#include "Object2D/Color.h"

void initTextures();
GLuint getTexture(int i);
float getTextureWidth(int i);
float getTextureHeight(int i);
void makeTexture(GLuint id, const char *filename, int *width, int *height);
void drawTextureQuad(GLuint id, const Color4f *c, const Bounds2P *b);

#endif
