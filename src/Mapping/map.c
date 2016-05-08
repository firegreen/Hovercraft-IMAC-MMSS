#include "Mapping/map.h"
#include "Object2D/object.h"
#include <stdio.h>
#include <stddef.h>
#include "SDL_tools.h"
#include "audios.h"
#include "textures.h"

#ifdef __APPLE__
#include <SDL.h>
#include <SDL_image.h>
#else
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#endif

#define BUFSIZE 512
#define readLine(strin, bufsize, fp) strin[0] = 0; fgets(strin,bufsize,fp); if(!strin[0]){ *error = EOFREACHED; return;}

char imagefile[100];

void drawMap(const Map* map){
    glPushMatrix();
    int i = 0;
    for(i=0;i<4;i++){
        map->physicalBounds.shapes->texturesPoints[i].x += 0.0005;
        map->physicalBounds.shapes->texturesPoints[i].y += 0.0005;
    }
    drawObject(&map->physicalBounds);
    Chained_Object* current = map->objects->next;
    while(current!=NULL){
        drawObject(current->object);
        current=current->next;
    }
    glPopMatrix();

}
void drawMiniMap(const Map* map){
    glPushMatrix();
    glColor4f(0.1,0.2,0.4,0.5);
    drawObject(&map->physicalBounds);
    glColor3f(1,1,1);
    Chained_Object* current = map->objects->next;
    while(current!=NULL){
        drawMiniObject(current->object);
        current=current->next;
    }
    glPopMatrix();

}

void addItem(Map* map, int i){
    if(map->nbcurrentItem<60)
    {
        Object* newO = malloc(sizeof(Object));
        cpyObject(newO,&map->potentialItem[i].object);
        char ok = 0;
        Chained_Object* co;
        Intersection inter;

        int cpt=0;
        while(!ok){
            co = map->objects;
            ok = 1;
            newO->x = (float)(rand()%(int)(map->bounds.rightBottom.x-map->bounds.leftBottom.x))
                    + map->bounds.leftBottom.x;
            newO->y = (float)(rand()%(int)(map->bounds.leftTop.y-map->bounds.rightBottom.y))
                    + map->bounds.leftBottom.y;
            newO->vx =5*((float)(rand()%2)-1);
            newO->vy =5*((float)(rand()%2)-1);
            while(co!=NULL){
                ok = !(collisionBetweenObject(newO,co->object,&inter));
                if(!ok) break;
                co = co->next;
            }
            cpt++;
            if(cpt>2){
                freeObject(&newO);
                return;
            }
        }
        newO->vx = newO->vy = 0;
        newO->vAngle = 1;
        addObjectToMap(map,newO,makePoint(newO->x,newO->y));
        map->items = makeChainedObject(newO,map->items,makePoint(newO->x,newO->y));
        map->nbcurrentItem++;
    }
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
    map->state +=0.001;
    if(map->state>1) map->state = 0;
    Chained_Object* co, *co2;
    Chained_Object* coNext;
    if(map->objects!=NULL){
        co = map->objects;
        while(co->next!=NULL){
            co2 = co->next->next;
            while(co2!=NULL){
                handleCollision(co->next->object,co2->object);
                co2 = co2->next;
            }
            if(co->next->object->life > 0){
                applyFrottement(map,co->next->object);
                updateObject(co->next->object);
                co = co->next;
            }
            else{
                coNext = co->next->next;
                //freeObject(&co->next->object);
                co->next->object->nbEffect=0;
                free(co->next);
                co->next = coNext;
            }
        }
        co2 = map->objects->next;
        while(co2!=NULL){
            handleCollision(map->objects->object,co2->object);
            co2 = co2->next;
        }
        if(map->objects->object->life > 0){
            applyFrottement(map,map->objects->object);
            updateObject(map->objects->object);
        }
        else{
            coNext = map->objects->next;
            //freeObject(&map->objects->object);
            map->objects->object->nbEffect=0;
            free(map->objects);
            map->objects = coNext;
        }
    }
    if(map->items!=NULL){
        Chained_Object* item = map->items;
        while(item->next!=NULL){
            if(item->next->object->nbEffect==0){
                Chained_Object* tmp = item->next->next;
                freeObject(&item->next->object);
                free(item->next);
                item->next = tmp;
            }
            else{
                if(item->next->object->angle>100){
                    item->next->object->vAngle =-1.5;
                    item->next->object->angle = 99;
                }
                else if(item->next->object->angle<60){
                    item->next->object->vAngle =1.5;
                    item->next->object->angle =61;
                }
                item = item->next;
            }
        }
        if(map->items->object->nbEffect==0){
            Chained_Object* tmp = map->items->next;
            freeObject(map->items->object);
            free(map->items);
            map->items = tmp;
        }
        else{
            if(map->items->object->angle>100){
                map->items->object->vAngle =-1.5;
                map->items->object->angle = 99;
            }
            else if(map->items->object->angle<60){
                map->items->object->vAngle =1.5;
                map->items->object->angle =61;
            }
        }
    }
    addRandomItem(map);
    map->time -= MILLISECOND_PER_FRAME;
}

void applyFrottement(const Map *map, Object *o){
    o->vx -= o->vx * map->frottement;
    o->vy -= o->vy * map->frottement;
}

void initMap(Map* map, float width, float height, float frottement, int textureID, int audioID, float time){
    map->bounds = makeBounds4P(-width/2.,height/2.,width/2.,-height/2.);
    map->height = height;
    map->width = width;
    map->objects = NULL;
    map->frottement = frottement;
    map->color = makeColor3f(1,1,1);
    map->Bcolorevolution = -0.003;
    map->Gcolorevolution = -0.0002;
    makeObject(&(map->physicalBounds),1,1,100,2000,1,0,1);
    float ratioX = 2*window.width/(window.orthoGLX*2);
    float ratioY = 2*window.height/(window.orthoGLY*2);
    makeInversedRectangleWithTexture(map->physicalBounds.shapes,-width/2.,height/2.,width,height,
                                     makeColor4f(1,1,1,1),getTexture(textureID),
                                     getTextureWidth(textureID)/ratioX,
                                     getTextureHeight(textureID)/ratioY);
    Effect e;
    e.rebound.resistance = 1000;
    e.rebound.rebound_value = 0.75;
    map->physicalBounds.effectDelays[0]=2;
    map->physicalBounds.effectsAtCollision[0]=e;
    map->physicalBounds.effectsTypesAtCollision[0]=REBOUND;
    map->objects=NULL;
    map->items = NULL;
    addObjectToMap(map,&(map->physicalBounds),makePoint(0,0));
    map->state=0;
    map->itemState = 0;
    map->time = time;
    map->audioID = audioID;
    map->lastitem = NULL;
    map->nbcurrentItem = 0;
    playAudioFadeIn(audioID,0.1);
}

void initMapItems(Map* map, int nbItems){
    map->nbItems = nbItems;
    map->potentialItem = malloc(sizeof(Item) * nbItems);
}

void freeMap(Map** map){

    freeAllChaineObject(&(*map)->objects);
    freeAllChaineObject(&(*map)->items);
    stopAudioFadeOut((*map)->audioID,2);

    free(*map);
    *map = NULL;
}

void freeMap2(Map* map){
    freeAllChaineObject(&map->objects);
    freeAllChaineObject(&map->items);
    stopAudioFadeOut(map->audioID,2);
}

void addRandomItem(Map *map){
    int i = 0;
    map->itemState += (float)rand()/(float)RAND_MAX+EPSILON;
    float inc = 1./(float)map->nbItems;
    for(;i<map->nbItems;i++){
        if((map->potentialItem[i].freq+inc)>map->itemState && map->itemState>inc){
            addItem(map,i);
            break;
        }
        inc+=inc;
    }
    if(map->itemState>1){
        map->itemState = 0;
    }
}

void setItem(Map *map, Item item, int indice){
    if(indice>=0 && indice<map->nbItems)
        map->potentialItem[indice] = item;
}

void addObjectToMap(Map *map, struct Object *o, Point2D position){
    if(map->objects==NULL)
        map->objects = makeChainedObject(o,NULL,position);
    else{
        Chained_Object* co = map->objects;
        while(co->next!=NULL)
            co = co->next;
        co->next = makeChainedObject(o,NULL,position);
    }
}

void readFile(Map* map, char* path, int* error)
{
    int i,j,k,iTex,adapter;
    int textureID;
    int nbObstacles, nbShapes, nbEffects,
            nbPoints, nbTexPoints, nbItems;
    int full;
    int audioID;
    float time;
    float frottement, freq;
    float life, radius, strength;
    int isStatic, isUnbreakable;
    char type;
    Point2D pos,pos2;
    TexturePoint2D texpos;
    Point2D size;
    Color4f color;
    char strin[BUFSIZE];
    char strincmp[BUFSIZE/8];
    Object* o;
    Item item;
    Effect e;
    FILE* fp;
    float ratioX = 2*window.width/(window.orthoGLX*2);
    float ratioY = 2*window.height/(window.orthoGLY*2);
    *error = NONEERROR;

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
    if(sscanf(strin, "texture:%d frottement:%f largeur:%f hauteur:%f audio:%d time:%f",
              &textureID,&frottement,&size.x,&size.y,&audioID,&time)<6){
        *error = BADFORMATMAP;
        return;
    }
    initMap(map, size.x, size.y, frottement, textureID,audioID,time);
    readLine(strin,BUFSIZE,fp);
    if(sscanf(strin, "nbobstacles:%d nbitems:%d", &nbObstacles, &nbItems)<2)
    {
        *error = BADFORMATMAP;
        return;
    }
    readLine(strin,BUFSIZ,fp);
    memset(strincmp,0,BUFSIZE/8);
    if(sscanf(strin,"%s",strincmp)<1)
    {
        *error = BADFORMATMAP;
        return;
    }
    if(strcmp(strincmp,"obstacles"))
    {
        *error = BADFORMATMAP;
        return;
    }

    for (i=0; i<nbObstacles; i++)
    {
        readLine(strin,BUFSIZ,fp);
        if(sscanf(strin, "nbformes:%d nbeffets:%d vie:%f rayoncolision:%f statique:%d force:%f incassable:%d position:(%f,%f)",
                         &nbShapes, &nbEffects, &life, &radius, &isStatic, &strength, &isUnbreakable,
                         &pos.x,&pos.y)<9)
        {
            *error = BADFORMATOBJECT;
            return;
        }
        o = malloc(sizeof(Object));
        makeObject(o,nbEffects,nbShapes,life,radius,isStatic,strength,isUnbreakable);
        o->x = pos.x; o->y = pos.y;
        readLine(strin,BUFSIZ,fp);
        memset(strincmp,0,BUFSIZE/8);
        if(sscanf(strin,"%s",strincmp)<1)
        {
            *error = BADFORMATOBJECT;
            return;
        }
        if(strcmp(strincmp,"formes"))
        {
            *error = BADFORMATOBJECT;
            return;
        }
        for (j=0; j<nbShapes; j++)
        {
            memset(strincmp,0,BUFSIZE/8);
            readLine(strin,BUFSIZ,fp);
            textureID = 0;
            if(sscanf(strin,"type:%c",&type)<1)
            {
                *error = BADFORMATSHAPE;
                return;
            }
            switch (type) {
            case 'c':
                if(sscanf(strin+7,"rayon:%f centre:(%f,%f) couleur:(%f,%f,%f,%f) rempli:%d texture:%d",
                          &radius,&pos.x,&pos.y,&color.r,&color.g,&color.b,&color.a,&full,&textureID)<8)
                {
                    *error = BADFORMATSHAPE;
                    return;
                }
                if(textureID==0)
                    makeCircle(o->shapes+j,radius,pos,color,full);
                else
                    makeCircleWithTexture(o->shapes+j,radius,pos,color,
                                          getTexture(textureID), getTextureWidth(textureID)/ratioX,
                                          getTextureHeight(textureID)/ratioY);
                break;
            case 'r':
                adapter=1;
                if(sscanf(strin+7,"topleft:(%f,%f) size:(%f,%f) couleur:(%f,%f,%f,%f) rempli:%d texture:%d adapter:%d",
                          &(pos.x),&(pos.y),&(size.x),&(size.y),&(color.r),&(color.g),&(color.b),&(color.a),
                          &full,&textureID,&adapter)<9)
                {
                    *error = BADFORMATSHAPE;
                    return;
                }
                if(textureID==0)
                    makeRectangle(o->shapes+j,pos.x,pos.y,size.x,size.y,color,full);
                else
                    makeRectangleWithTexture(o->shapes+j,pos.x,pos.y,size.x,size.y,color,
                                            getTexture(textureID), getTextureWidth(textureID)/ratioX,
                                            getTextureHeight(textureID)/ratioY,adapter);

              break;
            case 'p':
                if(sscanf(strin+7,"nbpoints:%d couleur:(%f,%f,%f,%f) rempli:%d texture:%d nbtexpoints:%d",
                          &nbPoints,&color.r,&color.g,&color.b,&color.a,&full,&textureID,&nbTexPoints)<6)
                {
                    *error = BADFORMATSHAPE;
                    return;
                }
                if(textureID==0)
                    makePolygon(o->shapes+j,nbPoints,color,full);
                else
                    makePolygonWithTexture(o->shapes+j,nbPoints,color,textureID,nbTexPoints);
                readLine(strin,BUFSIZ,fp);
                if(sscanf(strin,"%s",strincmp)<1)
                {
                    *error = BADFORMATSHAPE;
                    return;
                }
                if(strcmp(strincmp,"points"))
                {
                    *error = BADFORMATSHAPE;
                    return;
                }
                iTex=0;
                for(k=0; k<o->shapes[j].spec.polygon.nbPoints; k++){
                   readLine(strin,BUFSIZ,fp);
                   texpos.x=-1;
                   if(sscanf(strin,"(%f,%f) (%f,%f)",&(pos.x),&(pos.y),&texpos.x,&texpos.y)<2)
                   {
                       *error = BADFORMATSHAPE;
                       return;
                   }
                   setPolygonPoint(o->shapes+j,k,pos);
                   if(texpos.x>=0){
                       texpos.indice = k;
                       setPolygonTexturePoint(o->shapes+j,iTex,texpos);
                       iTex++;
                   }
                }
                break;
            case 's':
                if(sscanf(strin+7,"p1:(%f,%f) p2:(%f,%f) couleur:(%f,%f,%f,%f)",
                          &pos.x,&pos.y,&pos2.x,&pos2.y,&color.r,&color.g,&color.b,&color.a)<8)
                {
                    *error = BADFORMATSHAPE;
                    return;
                }
                makeSegment(o->shapes+j,pos,pos2,color);
                break;
            default:
                *error = BADFORMATSHAPE;
                return;
            }
        }
        readLine(strin,BUFSIZ,fp);
        memset(strincmp,0,BUFSIZE/8);
        if(sscanf(strin,"%s",strincmp)<1)
        {
            *error = BADFORMATOBJECT;
            return;
        }
        if(strcmp(strincmp,"effets"))
        {
            *error = BADFORMATOBJECT;
            return;
        }
        for (j=0; j<nbEffects; j++)
        {
            readLine(strin,BUFSIZ,fp);
            if(sscanf(strin,"type:%c",&type)<1)
            {
                *error = BADFORMATEFFECT;
                return;
            }
            switch (type) {
            case 'p':
                if(sscanf(strin+7,"valeur:%d",&(e.points.ammount))<1)
                {
                    *error = BADFORMATEFFECT;
                    return;
                }
                o->effectsTypesAtCollision[j] = POINTSMODIF;
                break;
            case 'a':
                if(sscanf(strin+7,"delay:%d valeur:%f",o->effectDelays + j,&(e.acceleration.acceleration_value))<1)
                {
                    *error = BADFORMATEFFECT;
                    return;
                }
                o->effectsTypesAtCollision[j] = ACCELERATION;
                break;
            case 'r':
                if(sscanf(strin+7,"resistance:%f rebond:%f",&(e.rebound.resistance),&(e.rebound.rebound_value))<1)
                {
                    *error = BADFORMATEFFECT;
                    return;
                }
                o->effectDelays[j] = 2;
                o->effectsTypesAtCollision[j] = REBOUND;
                break;
            default:
                *error = BADFORMATEFFECT;
                return;
            }
            o->effectsAtCollision[j] = e;
        }
        addObjectToMap(map,o,makePoint(o->x,o->y));
    }
    readLine(strin,BUFSIZ,fp);
    memset(strincmp,0,BUFSIZE/8);
    if(sscanf(strin,"%s",strincmp)<1)
    {
        *error = BADFORMATOBJECT;
        return;
    }
    if(strcmp(strincmp,"items"))
    {
        *error = BADFORMATOBJECT;
        return;
    }
    initMapItems(map,nbItems);
    for (i=0; i<nbItems; i++)
    {
        readLine(strin,BUFSIZ,fp);
        if(sscanf(strin, "nbformes:%d nbeffets:%d vie:%f rayoncolision:%f statique:%d force:%f incassable:%d freq:%f",
                         &nbShapes, &nbEffects, &life, &radius, &isStatic, &strength, &isUnbreakable,&freq)<8)
        {
            *error = BADFORMATOBJECT;
            return;
        }
        makeObject(&item.object,nbEffects,nbShapes,life,radius,isStatic,strength,isUnbreakable);
        item.freq = freq;
        readLine(strin,BUFSIZ,fp);
        memset(strincmp,0,BUFSIZE/8);
        if(sscanf(strin,"%s",strincmp)<1)
        {
            *error = BADFORMATOBJECT;
            return;
        }
        if(strcmp(strincmp,"formes"))
        {
            *error = BADFORMATOBJECT;
            return;
        }
        for (j=0; j<nbShapes; j++)
        {
            memset(strincmp,0,BUFSIZE/8);
            readLine(strin,BUFSIZ,fp);
            textureID = 0;
            if(sscanf(strin,"type:%c",&type)<1)
            {
                *error = BADFORMATSHAPE;
                return;
            }
            switch (type) {
            case 'c':
                if(sscanf(strin+7,"rayon:%f centre:(%f,%f) couleur:(%f,%f,%f,%f) rempli:%d texture:%d",
                          &radius,&pos.x,&pos.y,&color.r,&color.g,&color.b,&color.a,&full,&textureID)<8)
                {
                    *error = BADFORMATSHAPE;
                    return;
                }
                if(textureID==0)
                    makeCircle(item.object.shapes+j,radius,pos,color,full);
                else
                    makeCircleWithTexture(item.object.shapes+j,radius,pos,color,
                                          getTexture(textureID), getTextureWidth(textureID)/ratioX,
                                          getTextureHeight(textureID)/ratioY);
                break;
            case 'r':
                if(sscanf(strin+7,"topleft:(%f,%f) size:(%f,%f) couleur:(%f,%f,%f,%f) rempli:%d texture:%d",
                          &(pos.x),&(pos.y),&(size.x),&(size.y),&(color.r),&(color.g),&(color.b),&(color.a),
                          &full,&textureID)<9)
                {
                    *error = BADFORMATSHAPE;
                    return;
                }
                if(textureID==0)
                    makeRectangle(item.object.shapes+j,pos.x,pos.y,size.x,size.y,color,full);
                else{
                    makeRectangleWithTexture(item.object.shapes+j,pos.x,pos.y,size.x,size.y,color,
                                            getTexture(textureID), getTextureWidth(textureID)/ratioX,
                                            getTextureHeight(textureID)/ratioY,0);
                }

              break;
            case 'p':
                if(sscanf(strin+7,"nbpoints:%d couleur:(%f,%f,%f,%f) rempli:%d texture:%d nbtexpoints:%d",
                          &nbPoints,&color.r,&color.g,&color.b,&color.a,&full,&textureID,&nbTexPoints)<6)
                {
                    *error = BADFORMATSHAPE;
                    return;
                }
                if(textureID==0)
                    makePolygon(item.object.shapes+j,nbPoints,color,full);
                else
                    makePolygonWithTexture(item.object.shapes+j,nbPoints,color,textureID,nbTexPoints);
                readLine(strin,BUFSIZ,fp);
                if(sscanf(strin,"%s",strincmp)<1)
                {
                    *error = BADFORMATMAP;
                    return;
                }
                if(strcmp(strincmp,"points"))
                {
                    *error = BADFORMATMAP;
                    return;
                }
                iTex=0;
                for(k=0; k<item.object.shapes[j].spec.polygon.nbPoints; k++){
                   readLine(strin,BUFSIZ,fp);
                   texpos.x=-1;
                   if(sscanf(strin,"(%f,%f) (%f,%f)",&(pos.x),&(pos.y),&texpos.x,&texpos.y)<2)
                   {
                       *error = BADFORMATSHAPE;
                       return;
                   }
                   setPolygonPoint(item.object.shapes+j,k,pos);
                   if(texpos.x>=0){
                       texpos.indice = k;
                       setPolygonTexturePoint(item.object.shapes+j,iTex,texpos);
                       iTex++;
                   }
                }
                break;
            case 's':
                if(sscanf(strin+7,"p1:(%f,%f) p2:(%f,%f) couleur:(%f,%f,%f,%f)",
                          &pos.x,&pos.y,&pos2.x,&pos2.y,&color.r,&color.g,&color.b,&color.a)<8)
                {
                    *error = BADFORMATSHAPE;
                    return;
                }
                makeSegment(item.object.shapes+j,pos,pos2,color);
                break;
            default:
                *error = BADFORMATSHAPE;
                return;
            }
        }
        readLine(strin,BUFSIZ,fp);
        memset(strincmp,0,BUFSIZE/8);
        if(sscanf(strin,"%s",strincmp)<1)
        {
            *error = BADFORMATOBJECT;
            return;
        }
        if(strcmp(strincmp,"effets"))
        {
            *error = BADFORMATOBJECT;
            return;
        }
        for (j=0; j<nbEffects; j++)
        {
            readLine(strin,BUFSIZ,fp);
            if(sscanf(strin,"type:%c",&type)<1)
            {
                *error = BADFORMATEFFECT;
                return;
            }
            switch (type) {
            case 'p':
                if(sscanf(strin+7,"valeur:%d",&(e.points.ammount))<1)
                {
                    *error = BADFORMATEFFECT;
                    return;
                }
                item.object.effectsTypesAtCollision[j] = POINTSMODIF;
                break;
            case 'a':
                if(sscanf(strin+7,"delay:%d valeur:%f",item.object.effectDelays + j,&(e.acceleration.acceleration_value))<1)
                {
                    *error = BADFORMATEFFECT;
                    return;
                }
                item.object.effectsTypesAtCollision[j] = ACCELERATION;
                break;
            case 'r':
                if(sscanf(strin+7,"resistance:%f rebond:%f",&(e.rebound.resistance),&(e.rebound.rebound_value))<1)
                {
                    *error = BADFORMATEFFECT;
                    return;
                }
                item.object.effectDelays[j] = 2;
                item.object.effectsTypesAtCollision[j] = REBOUND;
                break;
            default:
                *error = BADFORMATEFFECT;
                return;
            }
            item.object.effectsAtCollision[j] = e;
        }
        setItem(map,item,i);
    }
    fclose(fp);
}

