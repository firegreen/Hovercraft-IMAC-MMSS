#include "GUI/mode.h"
#include "game.h"

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#endif
#include <stdio.h>


void vBitmapOutput(int x, int y, char *string, void *font)
{
    int len,i; // len donne la longueur de la chaîne de caractères

    glRasterPos2f(x,y); // Positionne le premier caractère de la chaîne
    len = (int) strlen(string); // Calcule la longueur de la chaîne
    for (i = 0; i < len; i++) glutBitmapCharacter(font,string[i]); // Affiche chaque caractère de la chaîne
}

void drawMenu()
{
  glColor3d(1,1,0); // Texte en blanc
  vBitmapOutput(0,50,"Test menu",GLUT_BITMAP_HELVETICA_18);

  glBegin(GL_QUADS);
  glColor3f(1, 1, 1);
  glVertex2f(-10, 5);
  glVertex2f(10, 5);
  glVertex2f(10, 0);
  glVertex2f(-10, 0);
  glEnd();
}
