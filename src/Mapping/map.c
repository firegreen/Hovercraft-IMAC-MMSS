#include "Mapping/map.h"
#include "Object2D/object.h"
#include <stdio.h>
#include <stddef.h>
#include "SDL_tools.h"
#define SEAFILE "images/sea.jpg"

#ifdef __APPLE__
#include <SDL.h>
#include <SDL_image.h>
#else
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#endif

#define BUFSIZE 512
#define IMAGEPATH "images/"
#define readLine(strin, bufsize, fp) strin[0] = 0; fgets(strin,bufsize,fp); if(!strin[0]){ *error = EOFREACHED;}

char imagefile[100];

void drawMap(const Map* map){
    glPushMatrix();
    if(map->textureid){
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, map->textureid);

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
    SDL_Surface* image = IMG_Load(file_name);
    if(image == NULL) {
        fprintf(stderr, "Impossible de charger l'image %s\n", file_name);
    }
    else{
        glGenTextures(1, &map->textureid);
        makeTexture(map->textureid,file_name,GL_RGB);
    }
    makeObject(&(map->physicalBounds),1,1,100,2000,1,0,1);
    makeInversedRectangle(map->physicalBounds.shapes,-width/2.,height/2.,width,height,makeColor4f(1,1,1,1),0);
    Effect e;
    e.rebound.resistance = 1000;
    e.rebound.rebound_value = 0.75;
    map->physicalBounds.effectDelays[0]=2;
    map->physicalBounds.effectsAtCollision[0]=e;
    map->physicalBounds.effectsTypesAtCollision[0]=REBOUND;
    map->objects = makeChainedObject(&(map->physicalBounds),NULL,makePoint(0,0));
}

void addObjectToMap(Map *map, struct Object *o, Point2D position){
    static int cpt = 0; cpt++;
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

typedef union readValues {
  float f;
  int i;
  char c;
} readValues;

void readFile(Map* map, char* path, int* error)
{
    int i,j;
    char texture[BUFSIZE/8];
    char type;
    float frottement, width, height;
    int nbre_obstacles, nbre_formes, nbeffects;
    float life, colliderRadius, strength;
    int isStatic, isUnbreakable;
    Point2D pos;
    readValues values[10];
    char strin[BUFSIZE];
    char strincmp[BUFSIZE/8];
    Object* o;
    Effect e;
    FILE* fp;
    if(path == NULL)
    {
        *error = FILENOTEXIST;
        return;
    }
    fp = fopen (path, "r");
    if(fp == NULL)
    {
        *error = FILENOTEXIST;
        return;
    }
    readLine(strin,BUFSIZE,fp);
    memset(texture,0,BUFSIZE/8);
    if(sscanf(strin, "texture:\"%[^\t\n\"]\" frottement:%f largeur:%f hauteur:%f",&texture,&frottement,&width,&height)<4){
        *error = BADFORMAT;
        return;
    }
    initMap(map, width, height, frottement, texture);

    readLine(strin,BUFSIZE,fp);
    if(sscanf(strin, "nbobstacles:%d", &nbre_obstacles)<1)
    {
        *error = BADFORMAT;
        return;
    }
    readLine(strin,BUFSIZ,fp);
    memset(strincmp,0,BUFSIZE/8);
    if(sscanf(strin,"%s",strincmp)<1)
    {
        *error = BADFORMAT;
        return;
    }
    if(strcmp(strincmp,"obstacles"))
    {
        *error = BADFORMAT;
        return;
    }
    for (i=0; i<nbre_obstacles; i++)
    {
        readLine(strin,BUFSIZ,fp);
        if(sscanf(strin, "nbformes:%d nbeffets:%d vie:%f rayoncolision:%f statique:%d force:%f incassable:%d position:(%f,%f)",
                         &nbre_formes, &nbeffects, &life, &colliderRadius, &isStatic, &strength, &isUnbreakable,
                  &pos.x,&pos.y)<9)
        {
            *error = BADFORMAT;
            return;
        }
        o = malloc(sizeof(Object));
        makeObject(o,nbeffects,nbre_formes,life,colliderRadius,isStatic,strength,isUnbreakable);
        o->x = pos.x; o->y = pos.y;
        readLine(strin,BUFSIZ,fp);
        memset(strincmp,0,BUFSIZE/8);
        if(sscanf(strin,"%s",strincmp)<1)
        {
            *error = BADFORMAT;
            return;
        }
        if(strcmp(strincmp,"formes"))
        {
            *error = BADFORMAT;
            return;
        }
        for (j=0; j<nbre_formes; j++)
        {
            readLine(strin,BUFSIZ,fp);
            if(sscanf(strin,"type:%c",&type)<1)
            {
                *error = BADFORMAT;
                return;
            }
            switch (type) {
            case 'c':
                if(sscanf(strin+7,"rayon:%f centre:(%f,%f) couleur:(%f,%f,%f,%f) rempli:%d",
                          &(values[0].f),&(values[1].f),&(values[2].f),&(values[3].f),
                          &(values[4].f),&(values[5].f),&(values[6].f),&(values[7].i))<8)
                {
                    *error = BADFORMAT;
                    return;
                }
                makeCircle(o->shapes+j,values[0].f,makePoint(values[1].f,values[2].f),
                        makeColor4f(values[3].f,values[4].f,values[5].f,values[6].f),values[7].i);
                break;
            case 'p':
                if(sscanf(strin+7,"nbpoints:%d couleur:(%f,%f,%f,%f) rempli:%d",
                          &(values[0].i),&(values[1].f),&(values[2].f),&(values[3].f),&(values[4].f),&(values[5].i))<6)
                {
                    *error = BADFORMAT;
                    return;
                }
                makePolygon(o->shapes+j,values[0].i,makeColor4f(values[1].f,values[2].f,values[3].f,values[4].f),values[5].i);
                for(j=0; j<values[0].i; j++){
                   readLine(strin,BUFSIZ,fp);
                   if(sscanf(strin,"(%f,%f)",&(values[0].f),&(values[1].f))<2)
                   {
                       *error = BADFORMAT;
                       return;
                   }
                   setPolygonPoint(o->shapes+i,j,makePoint(values[0].f,values[1].f));
                }
                break;
            case 's':
                if(sscanf(strin+7,"p1:(%f,%f) p2:(%f,%f) couleur:(%f,%f,%f,%f)",
                          &(values[0].f),&(values[1].f),&(values[2].f),&(values[3].f),
                          &(values[4].f),&(values[5].f),&(values[6].f),&(values[7].f))<8)
                {
                    *error = BADFORMAT;
                    return;
                }
                makeSegment(o->shapes+j,makePoint(values[0].f,values[1].f),makePoint(values[2].f,values[3].f),
                            makeColor4f(values[4].f,values[6].f,values[6].f,values[7].f));
                break;
            default:
                *error = BADFORMAT;
                return;
            }
        }
        readLine(strin,BUFSIZ,fp);
        memset(strincmp,0,BUFSIZE/8);
        if(sscanf(strin,"%s",strincmp)<1)
        {
            *error = BADFORMAT;
            return;
        }

        if(strcmp(strincmp,"effets"))
        {
            *error = BADFORMAT;
            return;
        }
        for (j=0; j<nbeffects; j++)
        {
            readLine(strin,BUFSIZ,fp);
            if(sscanf(strin,"type:%c",&type)<1)
            {
                *error = BADFORMAT;
                return;
            }
            switch (type) {
            case 'p':
                if(sscanf(strin+7,"valeur:%f",&(values[0].f))<1)
                {
                    *error = BADFORMAT;
                    return;
                }
                e.points.ammount = values[0].f;
                o->effectsTypesAtCollision[j] = POINTSMODIF;
                break;
            case 'a':
                if(sscanf(strin+7,"delay:%d valeur:%f",&(values[0].i),&(values[1].f))<1)
                {
                    *error = BADFORMAT;
                    return;
                }
                e.acceleration.acceleration_value = values[1].f;
                o->effectsTypesAtCollision[j] = ACCELERATION;
                o->effectDelays[j] = values[0].i;
                break;
            case 'r':
                if(sscanf(strin+7,"resistance:%f rebond:%f",&(values[0].f),&(values[1].f))<1)
                {
                    *error = BADFORMAT;
                    return;
                }
                e.rebound.resistance = values[0].f;
                e.rebound.rebound_value = values[1].f;
                o->effectDelays[j] = 2;
                o->effectsTypesAtCollision[j] = REBOUND;
                break;
            default:
                *error = BADFORMAT;
                return;
            }
            o->effectsAtCollision[j] = e;
        }
        addObjectToMap(map,o,makePoint(o->x,o->y));
    }
    *error = NOERROR;
}
