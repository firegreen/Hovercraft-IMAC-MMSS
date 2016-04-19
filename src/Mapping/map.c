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

        glColor3f(map->color.r, map->color.g, map->color.b);

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

        glColor3f(map->color.r,map->color.g,map->color.b);

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
    if(map->color.b<=0.8 || map->color.b>1.0){
        map->Bcolorevolution*=-1;
    }
    if(map->color.g<=0.9 || map->color.g>1.0){
        map->Gcolorevolution*=-1;
    }
    map->color.g += map->color.g*map->Gcolorevolution;
    map->color.b += map->color.b*map->Bcolorevolution;
}

void applyFrottement(const Map *map, Object *o){
    o->vx -= o->vx * map->frottement;
    o->vy -= o->vy * map->frottement;
}

void initMap(Map* map, float width, float height, float frottement, const char* file_name){
    map->bounds = makeBounds4P(-width/2.,height/2.,width/2.,-height/2.);
    map->height = height;
    map->width = width;
    map->objects = NULL;
    map->frottement = frottement;
    map->color = makeColor3f(1,1,1);
    map->Bcolorevolution = -0.003;
    map->Gcolorevolution = -0.0002;
    if(!SEATEXTUREID){
        SDL_Surface* image = IMG_Load(file_name);
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
    makeObject(&(map->physicalBounds),1,1,100,2000,1,0,1);
    makeInversedRectangle(map->physicalBounds.shapes,-width/2.,height/2.,width,height);
    map->physicalBounds.shapes->color=makeColor3f(25,60,98);
    Effect e;
    e.rebound.resistance = 100;
    e.rebound.rebound_value = 0.5;
    map->physicalBounds.effectsAtCollision[0]=e;
    map->physicalBounds.effectsTypesAtCollision[0]=REBOUND;
    map->objects = makeChainedObject(&(map->physicalBounds),NULL,makePoint(0,0));
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

void read_fichier(char* path, Map* map)
{
  int i;
  char* texture;
  char type;
  float frottement, width, height;
  float nbre_obstacles, nbre_formes;
  Object objet;
  FILE* fp;
  if(path != NULL)
  {
    fp = fopen (path, "r+");      /* Ouverture du fichier path en lecture + écriture */
  }
  else
  {
    fp = stdin;
  }
  while (fgetc(fp) != " ")
  {
    fscanf(fp, "Texture de la carte : %s\n", &texture);
    fscanf(fp, "Frottement de la carte : %f\n", &frottement);
    fscanf(fp, "Largeur de la carte : %f\n", &width);
    fscanf(fp, "Hauteur de la carte : %f\n", &height);
    initMap(map, width, height, frottement, texture);
    fscanf(fp, "Nombre d'obstacles : %f\nListe des obstables :\n", &nbre_obstacles);
    for (i=0; i<nbre_obstacles; i++)
    {
      fscanf(fp, "%f, ", &nbre_formes);
      for (i=0; i<nbre_formes; i++)
      {
        while(fgetc(fp) != '\n')
        {
        fscanf(fp, "%c, ", &type);
        if (type = "c")
        {
          circle c;
          Object.shapes[i].spec.circle = c;
        }
        if (type = "p")
        {
          polygone p;
          Object.shapes[i].spec.polygone = p;
        }
        if (type = "s")
        {
          segment s;
          Object.shapes[i].spec.segment = s;
        }
      }
    }
  }
}
