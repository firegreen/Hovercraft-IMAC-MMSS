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
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-window.orthoGLX, window.orthoGLX, -window.orthoGLY, window.orthoGLY);
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
    unsigned long option = SDL_GL_DOUBLEBUFFER | SDL_OPENGL | SDL_RESIZABLE
            | (SDL_FULLSCREEN * fullscreen);
    /* The mouse isn't much use unless we have a display for reference */
    if (SDL_SetVideoMode(width, height, BITS_PER_PIXEL, option) == NULL )
    {
        fprintf(stderr, "Couldn't set %fx%fx%d video mode: %s\n",width,height,BITS_PER_PIXEL,SDL_GetError());
        exit(1);
    }
    SDL_JoystickEventState(SDL_ENABLE);
    glDisable(GL_DEPTH_TEST);
    resize_handler(width,height);
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

void makeTexture(GLuint id, const char *filename, GLint textureFormat){
    SDL_Surface* image = IMG_Load(filename);
    if(image == NULL) {
        fprintf(stderr, "Impossible de charger l'image %s\n", filename);
    }
    else{
        glBindTexture(GL_TEXTURE_2D, id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        GLenum format;
        switch(image->format->BytesPerPixel) {
        case 1:
            format = GL_BLUE;
            break;
        case 3:
            format = GL_RGB;
            break;
        case 4:
            format = GL_RGBA;
            break;
        default:
            format = GL_RGBA;
            fprintf(stderr, "Format des pixels de l'image %d non pris en charge - byte per pixel :%d\n",
                    id, image->format->BytesPerPixel);
        }
        glTexImage2D(GL_TEXTURE_2D, 0, textureFormat, image->w, image->h,
                     0, format, GL_UNSIGNED_BYTE, image->pixels);
        SDL_FreeSurface(image);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

void drawTextureQuad(GLuint id, const Color4f* c, const Bounds2P* b){
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, id);
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBegin(GL_QUADS);

    glColor4f(c->r, c->g, c->b,c->a);

    glTexCoord2f(0, 0);
    glVertex2f(b->leftTop.x, b->leftTop.y);

    glTexCoord2f(0, 1);
    glVertex2f(b->leftTop.x, b->rightBottom.y);

    glTexCoord2f(1, 1);
    glVertex2f(b->rightBottom.x, b->rightBottom.y);

    glTexCoord2f(1, 0);
    glVertex2f(b->rightBottom.x, b->leftTop.y);

    glEnd();
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
}
