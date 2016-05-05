#include "textures.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL.h>

#define BUFSIZE 512
#define readLine(strin, bufsize, fp) strin[0] = 0; fgets(strin,bufsize,fp); if(!strin[0]){ return;}
#define ERROR fprintf(stderr,"Error while reading textures files"); return;


GLuint* texturesIDs=NULL;
Point2D* texturesSize=NULL;
int nbTextures=0;

void initTextures(){
    char texture[BUFSIZE/8];
    char strin[BUFSIZE];
    FILE* fp;
    fp = fopen ("textures", "r");
    if(fp == NULL)
    {
        ERROR
    }
    readLine(strin,BUFSIZE,fp);
    if(sscanf(strin, "%d",&nbTextures)<1){
        return;
    }
    if(nbTextures<=0) return;
    texturesIDs = malloc(nbTextures * sizeof(GLuint));
    texturesSize = malloc(nbTextures * sizeof(Point2D));
    glGenTextures(nbTextures,texturesIDs);
    int i, width, height;
    for(i=0;i<nbTextures;i++){
        readLine(strin,BUFSIZE,fp);
        memset(texture,0,BUFSIZE/8);
        if(sscanf(strin, "\"%[^\t\n\"]\"",texture)==1){
            makeTexture(texturesIDs[i],texture,&(width),&(height));
            texturesSize[i].x=width;
            texturesSize[i].y=height;
        }
    }
}

void makeTexture(GLuint id, const char *filename, int *width, int *height){
    SDL_Surface* image = IMG_Load(filename);
    if(image == NULL) {
        fprintf(stderr, "Impossible de charger l'image %s\n", filename);
    }
    else{
        glBindTexture(GL_TEXTURE_2D, id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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
        (*width) = image->w;
        (*height) = image->h;
        glTexImage2D(GL_TEXTURE_2D, 0, format, image->w, image->h,
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

GLuint getTexture(int i){
    if(i<=0 && i>nbTextures) return 0;
    return texturesIDs[i-1];
}

float getTextureWidth(int i){
    if(i<=0 && i>nbTextures) return 0;
    return texturesSize[i-1].x;
}
float getTextureHeight(int i){
    if(i<=0 && i>nbTextures) return 0;
    return texturesSize[i-1].y;
}
