#include "GUI/level.h"
#include "Mapping/camera.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <SDL_tools.h>
#include "game.h"
#include "textures.h"
#include <stdlib.h>
#include "GUI/menu.h"
#include "GL/glut.h"
#include "audios.h"
#include "GUI/menu.h"
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Color4f white =  { 1, 1, 1, 1};
Color4f black =  { 0, 0, 0, 1};
Color4f red =  { 1, 0, 0, 1};
Color4f green =  { 0, 1, 0, 1};
Color4f blue =  { 0, 0, 1, 1};

void drawView(const Level* l){
    glPushMatrix();
    applyCameraTransform(&l->hovercraft->view);
    drawMap(&l->map);
    drawHovercraft(l->hovercraft);
    glPopMatrix();
    glPushMatrix();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-50, 50,-50,50);
    glViewport(0,0,window.height/8.,window.width/8.);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glColor3f(0.1,0.3,1);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(0,0);
    int i;
    for(i=0; i<=50; i++)
    {
        glVertex2f(cosf((float)i*((2*M_PI)/50.))*50,sinf((float)i*((2*M_PI)/50.))*50);
    }
    glEnd();
    glBegin(GL_TRIANGLE_FAN);
    glColor3f(0.8,0.9,0.8);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(0,0);
    for(i=0; i<=50; i++)
    {
        glVertex2f(cosf((float)i*((2*M_PI)/50.))*45,sinf((float)i*((2*M_PI)/50.))*45);
    }
    glEnd();

    glLoadIdentity();
    glColor3ub(255,255,100);
    char* pointsSTR = malloc(sizeof(char)*10);
    sprintf(pointsSTR,"%d",l->hovercraft->points);
    float width =  glutBitmapLength(GLUT_BITMAP_HELVETICA_18,pointsSTR)*(window.orthoGLX*10)/window.width;
    float height = glutBitmapWidth(GLUT_BITMAP_HELVETICA_18,'_')*4;
    vBitmapOutput(- width*0.5,-height*0.5,pointsSTR,GLUT_BITMAP_HELVETICA_18);
    free(pointsSTR);
    glPopMatrix();
}

void initLevel(Level *level, int mapid){
    char strfile[10];
    memset(strfile,0,10);
    sprintf(strfile,"map/%d",mapid);
    int error;
    readFile(&level->map,strfile,&error);
    if(error!=NONEERROR){
        fprintf(stderr,"Error while reading map %d, exit...\n",mapid);
        switch(error){
        case BADFORMATOBJECT:
            fprintf(stderr," format obstacle errone");
            break;
        case BADFORMATITEM:
            fprintf(stderr," format item errone");
            break;
        case BADFORMATSHAPE:
            fprintf(stderr," format forme errone");
            break;
        case BADFORMATMAP:
            fprintf(stderr," format map errone");
            break;
        case BADFORMATEFFECT:
            fprintf(stderr," format effet errone");
            break;
        case FILENOTEXIST:
            fprintf(stderr," fichier non existant");
        case EOFREACHED:
            fprintf(stderr," fin de fichier atteint");
            break;
        }
        fprintf(stderr,", exit...\n");
        exit(1);
    }
    level->hovercraft = malloc(sizeof(Hovercraft));
    initHovercraft(level->hovercraft,&level->map);
    level->endStart=0;
}

void drawLevel(const Level* level){
    drawView(level);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float glX =window.orthoGLX*4, glY=glX*(level->map.height/level->map.width);
    gluOrtho2D(0,glX,0,glY);
    glViewport(window.width-200,window.height - 200*(level->map.height/level->map.width),
               200,200*(level->map.height/level->map.width));
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glScalef((glX-20)/level->map.width,(glY-40)/level->map.height,1);
    glTranslatef(level->map.width/2+20,level->map.height/2+40,0);
    drawMiniMap(&level->map);
    drawMiniHovercraft(level->hovercraft);
    glLoadIdentity();
    glColor3ub(255,255,255);
    glBegin(GL_QUADS);
    glColor3f(0.3,0.5,0.3);
    glVertex3f(10,0,1);
    glVertex3f(0,10,1);
    glVertex3f(0,glY-10,1);
    glVertex3f(10,glY,1);
    glColor3f(0.1,0.0,0.1);
    glEnd();
    glBegin(GL_QUADS);
    glColor3f(0.3,0.5,0.3);
    glVertex3f(glX-10,0,1);
    glVertex3f(glX,10,1);
    glVertex3f(glX,glY-10,1);
    glVertex3f(glX-10,glY,1);
    glColor3f(0.1,0.0,0.1);
    glEnd();
    glBegin(GL_QUADS);
    glColor3f(0.3,0.5,0.3);
    glVertex3f(0,20,0);
    glVertex3f(20,0,1);
    glVertex3f(glX-20,0,1);
    glVertex3f(glX,20,0);
    glColor3f(0.1,0.0,0.1);
    glEnd();
    glBegin(GL_QUADS);
    glColor3f(0.3,0.5,0.3);
    glVertex3f(0,glY-20,0);
    glVertex3f(20,glY,1);
    glVertex3f(glX-20,glY,1);
    glVertex3f(glX,glY-20,0);
    glColor3f(0.1,0.0,0.1);
    glEnd();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0,glX,0,glY);
    glViewport(window.width*0.8,0,window.width*0.2,window.width*0.2*(level->map.height/level->map.width));
    char* secondSTR = malloc(sizeof(char)*10);
    sprintf(secondSTR,"%d",(int)(level->map.time/1000.));
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(glX/2,glY/2,0);
    glBegin(GL_TRIANGLE_FAN);
    glColor3f(0.1,0.5,0.5);
    glVertex2f(0,0);
    int i;
    for(i=0; i<=50; i++)
    {
        glVertex2f(cosf((float)i*(50./(2*M_PI)))*glX/4,sinf((float)i*(50./(2*M_PI)))*glX/4);
    }
    glEnd();
    glColor3ub(255,200,180);
    float width =  glutBitmapLength(GLUT_BITMAP_HELVETICA_18,secondSTR)*(window.orthoGLX*10)/window.width;
    float height = glutBitmapWidth(GLUT_BITMAP_HELVETICA_18,'_')*4;
    vBitmapOutput(- width, -height*0.6 ,secondSTR,GLUT_BITMAP_HELVETICA_18);
    free(secondSTR);
    if(level->endStart>0){
        if((25*(int)(SDL_GetTicks() - level->endStart)/100)%2==0){
            glViewport(0,0,window.width,window.height);
            if(level->win)
            {
                Bounds2P b = makeBounds2P(-window.orthoGLX*0.5,window.orthoGLY*0.5,
                                          window.orthoGLX*0.5,-window.orthoGLY*0.5);
                drawTextureQuad(29,&white,&b);
            }
            else{
                glColor3f(0.8,0.2,0.5);
                float width  =  glutBitmapLength(GLUT_BITMAP_HELVETICA_18,"FAILED :C")*(window.orthoGLX*2)/window.width;
                float height = glutBitmapWidth(GLUT_BITMAP_HELVETICA_18,'_')*4;
                vBitmapOutput(-width/2,-height/2,"FAILED :C",GLUT_BITMAP_HELVETICA_18);
            }
        }
    }
    glColor3f(1,1,1);
}

void updateLevel(Level* level){
    updateMap(&(level->map));
    if(level->map.time<=0){
        level->win = level->hovercraft->points>100;
        if(level->endStart==0){
            level->endStart = SDL_GetTicks();
        }
        else{
            if(SDL_GetTicks() - level->endStart>2000){
                exit(0);
            }
        }
        return;
    }
    int i =0;
    Chained_Object* co;
    applyFrottement(&(level->map), &(level->hovercraft->physical_body));
    co = level->map.objects;
    while(co!=NULL){
        handleCollision(&(level->hovercraft->physical_body), co->object);
        co = co->next;
    }
    updateHovercraft(level->hovercraft);
    updateViewOfHovercraft(level->hovercraft);
}


void handleEventLevel(Level *l, const SDL_Event *event){
    handleEventHovercraft(l->hovercraft,event);
}
