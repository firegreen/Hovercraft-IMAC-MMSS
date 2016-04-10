#include "GUI/level.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <SDL_tools.h>

void drawView(const Map* map, const Hovercraft* hovercraft){
    float scaleX = (2*GLREPERE)/(hovercraft->view.rightBottom.x - hovercraft->view.leftTop.x);
    float scaleY = (2*GLREPERE)/(hovercraft->view.leftTop.y - hovercraft->view.rightBottom.y);
    glScalef(scaleX,scaleY,1.);
    float translateX,translateY;
    if(hovercraft->view.leftTop.x < map->bounds.leftTop.x)
        translateX = -GLREPERE/scaleX - map->bounds.leftTop.x;
    else if (hovercraft->view.rightBottom.x > map->bounds.rightBottom.x)
        translateX = GLREPERE/scaleX - map->bounds.rightBottom.x;
    else
        translateX = -GLREPERE/scaleX - hovercraft->view.leftTop.x;

    if(hovercraft->view.leftTop.y > map->bounds.leftTop.y)
        translateY = GLREPERE/scaleY - map->bounds.leftTop.y;
    else if (hovercraft->view.rightBottom.y < map->bounds.rightBottom.y)
        translateY = -GLREPERE/scaleY - map->bounds.rightBottom.y;
    else
        translateY = GLREPERE/scaleY - hovercraft->view.leftTop.y;

    glTranslatef(translateX, translateY,0);
    drawMap(map);
    drawHovercraft(hovercraft);
}

void drawLevelForOnePlayer(const Level* level){
    glViewport(0,0,window.width,window.height);
    drawView(&(level->map), level->players);
    int i =0;
    for(i;i<2;i++){
        glColor3f(0.1,0.6,0);
        glBegin(GL_LINES);
        glVertex2f(i*2,-100);
        glVertex2f(i*2,100);
        glEnd();
        glColor3f(0.6,0.1,0);
        glBegin(GL_LINES);
        glVertex2f(-100,i*2);
        glVertex2f(100,i*2);
        glEnd();
        glColor3f(0.1,0.6,0);
        glBegin(GL_LINES);
        glVertex2f(-i*2,-100);
        glVertex2f(-i*2,100);
        glEnd();
        glColor3f(0.6,0.1,0);
        glBegin(GL_LINES);
        glVertex2f(-100,-i*2);
        glVertex2f(100,-i*2);
        glEnd();
    }
}

void drawLevelForTwoPlayer(const Level* level){
    int width=window.width/2;
    glViewport(0,0,width,window.height);
    drawView(&(level->map), level->players);
    glViewport(width,0,width,window.height);
    drawView(&(level->map), level->players+1);
}
void drawLevelForThreePlayer(const Level* level){
    int width=window.width/2;
    int height=window.height/2;
    glViewport(0,0,width,height);
    drawView(&(level->map), level->players);
    glViewport(width,0,width,height);
    drawView(&(level->map), level->players+1);
    glViewport(0,height,width,height);
    drawView(&(level->map), level->players+2);
}

void drawLevelForFourPlayer(const Level* level){
    int width=window.width/2;
    int height=window.height/2;
    glViewport(0,0,width,height);
    drawView(&(level->map), level->players);
    glViewport(width,0,width,height);
    drawView(&(level->map), level->players+1);
    glViewport(0,height,width,height);
    drawView(&(level->map), level->players+2);
    glViewport(width,height,width,height);
    drawView(&(level->map), level->players+3);
}
void drawLevel(const Level* level){
  switch(level->nbPlayers){
  case 1:
      drawLevelForOnePlayer(level);
      break;
  case 2:
      drawLevelForTwoPlayer(level);
      break;
  case 3:
      drawLevelForThreePlayer(level);
      break;
  case 4:
      drawLevelForFourPlayer(level);
      break;
  }
}

void updateLevel(Level* level){
  updateMap(&(level->map));
  int i =0;
  for(;i<level->nbPlayers;i++){
    updateHovercraft(level->players+i);
    Chained_Object* co = level->map.objects;
    for(;co!=NULL;co=co->next){
        handleCollision(&(level->players[i].physical_body),co->object);
        Modification* current = co->object->receivedModifs;
        if(current!=NULL){
            while(current->next!=NULL){
                if(applyEffectToObject(current,co->object)){
                    Modification* nextNext = current->next->next;
                    free(current->next);
                    current->next = nextNext;
                }
                else
                    current = current->next;
            }
            while(applyEffectToObject(co->object->receivedModifs,co->object)){
                co->object->receivedModifs = co->object->receivedModifs->next;
                if(co->object->receivedModifs==NULL)
                    break;
            }
        }
    }
  }

}
