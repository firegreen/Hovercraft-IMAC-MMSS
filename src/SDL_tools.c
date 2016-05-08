#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#include <SDL.h>
#include <SDL_image.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#endif

#include "SDL_tools.h"

void resize_handler(float width, float height)
{
    window.width = width;
    window.height = height;
    window.orthoGLX = GLREPERE;
    window.orthoGLY = GLREPERE * height/width;
    reloadGLrepere();
}

void reloadGLrepere(){
#ifndef WIN32
    if (SDL_SetVideoMode(window.width, window.height, BITS_PER_PIXEL, window.option) == NULL )
    {
        fprintf(stderr, "Couldn't set %fx%fx%d video mode: %s\n",window.width, window.height,BITS_PER_PIXEL,SDL_GetError());
        exit(1);
    }
#endif
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-window.orthoGLX, window.orthoGLX, -window.orthoGLY, window.orthoGLY);
    glViewport(0,0,window.width,window.height);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}


void loadCustomViewport(int x, int y, int width, int height){
#ifndef WIN32
    if (SDL_SetVideoMode(window.width, window.height, BITS_PER_PIXEL, window.option) == NULL )
    {
        fprintf(stderr, "Couldn't set %fx%fx%d video mode: %s\n",window.width, window.height,BITS_PER_PIXEL,SDL_GetError());
        exit(1);
    }
#endif
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-window.orthoGLX, window.orthoGLX, -window.orthoGLY, window.orthoGLY);
    glViewport(x,y,width,height);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void initialize_window(float width, float height, unsigned char fullscreen)
{
    /* Initialize the SDL library (starts the event loop) */
    if ( SDL_Init(SDL_INIT_VIDEO  | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK) < 0 )
    {
        fprintf(stderr,"Couldn't initialize SDL: %s\n", SDL_GetError());
        exit(1);
    }

    /* Clean up on exit, exit on window close and interrupt */
    atexit(SDL_Quit);
    window.option = SDL_GL_DOUBLEBUFFER | SDL_OPENGL | SDL_RESIZABLE
            | (SDL_FULLSCREEN * fullscreen);
    /* The mouse isn't much use unless we have a display for reference */
    if (SDL_SetVideoMode(width, height, BITS_PER_PIXEL, window.option) == NULL )
    {
        fprintf(stderr, "Couldn't set %fx%fx%d video mode: %s\n",width,height,BITS_PER_PIXEL,SDL_GetError());
        exit(1);
    }
    SDL_WM_SetCaption("Over the sea","icon.png");
    SDL_JoystickEventState(SDL_ENABLE);
    glDisable(GL_DEPTH_TEST);
    resize_handler(width,height);
    char fakeParam[] = "fake";
    char *fakeargv[] = { fakeParam, NULL };
    int fakeargc = 1;
    glutInit( &fakeargc, fakeargv);
}
int windowEventHandler(const SDL_Event* event)
{
    switch(event->type)
    {
    case SDL_VIDEORESIZE:
        resize_handler(event->resize.w,event->resize.h);
        return 1;
    default:
        break;
    }
    return 0;
}
