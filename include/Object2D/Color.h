#ifndef COLOR_H
#define COLOR_H

typedef struct Color3f{
  float r;
  float g;
  float b;
} Color3f;

typedef struct Color4f{
  float r;
  float g;
  float b;
  float a;
} Color4f;

Color3f makeColor3f(float r, float g, float b);
Color4f makeColor4f(float r, float g, float b, float a);

#endif
