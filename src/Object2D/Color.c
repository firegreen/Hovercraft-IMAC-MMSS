#include "Object2D/Color.h"

Color3f makeColor3f(float r, float g, float b){
    Color3f c;
    c.r =r;
    c.g =g;
    c.b =b;
    return c;
}

Color4f makeColor4f(float r, float g, float b, float a){
    Color4f c;
    c.r =r;
    c.g =g;
    c.b =b;
    c.a =a;
    return c;
}
