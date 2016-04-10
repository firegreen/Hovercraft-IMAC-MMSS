#include "Mapping/map.h"
#include "Object2D/object.h"
#include <stdio.h>
#define SEAFILE "images/sea.jpg"

#ifdef __APPLE__
#include <SDL.h>
#include <SDL_image.h>
#else
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#endif

GLuint SEATEXTUREID = 0;

void drawMap(const Map* map){
    glPushMatrix();
    if(SEATEXTUREID){
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, SEATEXTUREID);

        glBegin(GL_QUADS);

        glColor3f(1, 1, 1);

        glTexCoord2f(0, 0);
        glVertex2f(map->bounds.leftTop.x, map->bounds.leftTop.y);

        glTexCoord2f(0, 1);
        glVertex2f(map->bounds.leftBottom.x, map->bounds.leftBottom.y);

        glTexCoord2f(1, 1);
        glVertex2f(map->bounds.rightBottom.x, map->bounds.rightBottom.y);

        glTexCoord2f(1, 0);
        glVertex2f(map->bounds.rightTop.x, map->bounds.rightTop.y);

        glEnd();
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_TEXTURE_2D);
    }
    else{
        glBegin(GL_QUADS);

        glColor3f(0.15, 0.3, 0.65);

        glVertex2f(map->bounds.leftTop.x, map->bounds.leftTop.y);
        glVertex2f(map->bounds.leftBottom.x, map->bounds.leftBottom.y);
        glVertex2f(map->bounds.rightBottom.x, map->bounds.rightBottom.y);
        glVertex2f(map->bounds.rightTop.x, map->bounds.rightTop.y);

        glEnd();
    }
    Chained_Object* current = map->objects;
    while(current!=NULL){
        glColor3f(1, 1, 1);
        drawObject(current->object);
        current=current->next;
    }
    glPopMatrix();

}

void updateMap(Map *map){

}

void initMap(Map* map, float width, float height){
    map->bounds = makeBounds4P(-width/2.,height/2.,width/2.,-height/2.);
    map->height = height;
    map->width = width;
    map->objects = NULL;
    if(!SEATEXTUREID){
        SDL_Surface* image = IMG_Load(SEAFILE);
        if(image == NULL) {
            fprintf(stderr, "Impossible de charger l'image %s\n", SEAFILE);
        }
        else{
            glGenTextures(1, &SEATEXTUREID);
            glBindTexture(GL_TEXTURE_2D, SEATEXTUREID);
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
                            SEATEXTUREID, image->format->BytesPerPixel);
                    glBindTexture(GL_TEXTURE_2D, 0);

                    SDL_FreeSurface(image);
                    return;
            }
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image->w, image->h, 0, format, GL_UNSIGNED_BYTE, image->pixels);

            glBindTexture(GL_TEXTURE_2D, 0);

            SDL_FreeSurface(image);
        }

    }
    Object* o = makeObject(1,1,100,2000);
    makeInversedRectangle(o->shapes,-100,100,200,200);
    Effect e;
    e.acceleration.acceleration_value = 0.1;
    o->effectsAtCollision[0]=e;
    o->effectsTypesAtCollision[0]=ACCELERATION;
    map->objects = makeChainedObject(o,NULL,makePoint(0,0));
}

void addObjectToMap(Map *map, struct Object *o, Point2D position){
    if(map->objects==NULL)
    {
        map->objects = makeChainedObject(o,NULL,position);
        return;
    }
    Chained_Object* current = map->objects;
    while(current->next!=NULL){
        current = current->next;
    }
    current->next = makeChainedObject(o,NULL,position);
}
