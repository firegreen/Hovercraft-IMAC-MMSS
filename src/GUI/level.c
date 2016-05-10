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

void drawView(const Level* l, int currentHovercraft){
    glPushMatrix();
    Camera c;
    glColor3f(1,1,1);
    if(currentHovercraft>=0){
        c = l->players[currentHovercraft].view;
        applyCameraTransform(&l->players[currentHovercraft].view);
    }
    else{
        if(l->indice<l->nbBots){
            c=l->bots[l->indice].h.view;
            updateViewOfHovercraft(&l->bots[l->indice].h);
            applyCameraTransform(&l->bots[l->indice].h.view);
        }
        else{
            makeCamera(&c,l->map.width,l->map.height,&l->map,makePoint(0,0),window.width,window.height);
            applyCameraTransform(&c);
        }
    }
    drawMap(&l->map);
    int i;
    for(i=0;i<l->nbPlayers;i++)
        drawHovercraft(l->players+i);
    for(i=0;i<l->nbBots;i++){
        drawHovercraft(&l->bots[i].h);
    }
    glPopMatrix();
    glPushMatrix();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-50, 50,-50,50);
    char* pointsSTR = malloc(sizeof(char)*10);
    sprintf(pointsSTR,"%d",l->players[currentHovercraft].points);
    switch (currentHovercraft) {
    case 0:
        glViewport(0,
                   c.viewportleftTop.y+9*c.viewportHeight/10,
                   c.viewportWidth/10,c.viewportHeight/10);
        break;
    case 1:
        glViewport(c.viewportleftTop.x+9*c.viewportWidth/10,
                   c.viewportleftTop.y+9*c.viewportHeight/10,
                   c.viewportWidth/10,c.viewportHeight/10);
        break;
    case 2:
        glViewport(0,
                   0,
                   c.viewportWidth/10,c.viewportHeight/10);
        break;
    case 3:
        glViewport(c.viewportleftTop.x+9*c.viewportWidth/10,
                   0,
                   c.viewportWidth/10,c.viewportHeight/10);
        break;
    default:
        break;
    }
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glBegin(GL_TRIANGLE_FAN);
    float cosInc = cosf((float)M_PI/25.);
    float sinInc = sinf((float)M_PI/25.);
    float x=1,y=0, oldX;
    for(i=0; i<=50; i++)
    {
        glColor3f(x*0.05+0.5,0.5,y*0.01+0.2);
        glVertex2f(x*50,y*50);
        oldX = x;
        x = x*cosInc - y*sinInc;
        y = oldX*sinInc + y*cosInc;
    }
    glEnd();
    glBegin(GL_TRIANGLE_FAN);
    x=1,y=0;
    for(i=0; i<=50; i++)
    {
        glColor3f(x*0.05,0.01,y*0.3+0.2);
        glVertex2f(x*45,y*45);
        oldX = x;
        x = x*cosInc - y*sinInc;
        y = oldX*sinInc + y*cosInc;
    }
    glEnd();

    glLoadIdentity();
    glColor3ub(255,255,100);
    if(window.width<700){
        float width =  glutBitmapLength(GLUT_BITMAP_HELVETICA_18,pointsSTR)*(window.orthoGLX*10)/window.width;
        float height = glutBitmapWidth(GLUT_BITMAP_HELVETICA_18,'_')*4;
        vBitmapOutput(- width*0.5,-height*0.5,pointsSTR,GLUT_BITMAP_HELVETICA_18);
    }
    else{
        float width =  glutBitmapLength(GLUT_BITMAP_TIMES_ROMAN_24,pointsSTR)*(window.orthoGLX*10)/window.width;
        float height = glutBitmapWidth(GLUT_BITMAP_TIMES_ROMAN_24,'_')*2;
        vBitmapOutput(- width*0.5,-height*0.5 ,pointsSTR,GLUT_BITMAP_TIMES_ROMAN_24);
    }
    free(pointsSTR);
    glPopMatrix();
    reloadGLrepere();
}

void specialDrawView(const Level* l, int currentHovercraft, float state){
    if(!state){
        float coef = 1.4;
        glPushMatrix();
        l->players[currentHovercraft].view.leftTop.x*=coef;
        l->players[currentHovercraft].view.leftTop.y*=coef;
        l->players[currentHovercraft].view.rightBottom.x*=coef;
        l->players[currentHovercraft].view.rightBottom.y*=coef;
        drawView(l,currentHovercraft);
        l->players[currentHovercraft].view.leftTop.x/=coef;
        l->players[currentHovercraft].view.leftTop.y/=coef;
        l->players[currentHovercraft].view.rightBottom.x/=coef;
        l->players[currentHovercraft].view.rightBottom.y/=coef;
        glPopMatrix();
    }
    if(state<20){
        float coef = (1.4 - 2.*state/100.);
        glPushMatrix();
        l->players[currentHovercraft].view.leftTop.x*=coef;
        l->players[currentHovercraft].view.leftTop.y*=coef;
        l->players[currentHovercraft].view.rightBottom.x*=coef;
        l->players[currentHovercraft].view.rightBottom.y*=coef;
        drawView(l,currentHovercraft);
        l->players[currentHovercraft].view.leftTop.x/=coef;
        l->players[currentHovercraft].view.leftTop.y/=coef;
        l->players[currentHovercraft].view.rightBottom.x/=coef;
        l->players[currentHovercraft].view.rightBottom.y/=coef;
        glPopMatrix();
    }
    else if(state<50){
        drawView(l,currentHovercraft);
    }
    else if(state<85){
        float coef;
        if(l->map.width<l->map.height){
            float width = l->players[currentHovercraft].view.rightBottom.x-
                    l->players[currentHovercraft].view.leftTop.x;
            coef = ((l->map.width/width) * (state-80.)/5.);
        }else{
            float height = l->players[currentHovercraft].view.leftTop.y-
                    l->players[currentHovercraft].view.rightBottom.y;
            coef = ((l->map.height/height) * (state-80.)/5.);
        }
        glPushMatrix();
        Point2D leftTop = l->players[currentHovercraft].view.leftTop;
        Point2D rightBottom = l->players[currentHovercraft].view.rightBottom;
        zoom(&l->players[currentHovercraft].view,coef-1);
        drawView(l,currentHovercraft);
        l->players[currentHovercraft].view.leftTop = leftTop;
        l->players[currentHovercraft].view.rightBottom = rightBottom;
        glPopMatrix();
    }
    else{
        float coef;
        if(l->map.width<l->map.height){
            float width = l->players[currentHovercraft].view.rightBottom.x-
                    l->players[currentHovercraft].view.leftTop.x;
            coef = ((l->map.width/width));
        }else{
            float height = l->players[currentHovercraft].view.leftTop.y-
                    l->players[currentHovercraft].view.rightBottom.y;
            coef = ((l->map.height/height));
        }
        glPushMatrix();
        Point2D leftTop = l->players[currentHovercraft].view.leftTop;
        Point2D rightBottom = l->players[currentHovercraft].view.rightBottom;
        zoom(&l->players[currentHovercraft].view,coef-1);
        drawView(l,currentHovercraft);
        if(state>90){
            if((53*(int)state/100)%2==0){
                glViewport(l->players[currentHovercraft].view.viewportleftTop.x,
                           l->players[currentHovercraft].view.viewportleftTop.y,
                           l->players[currentHovercraft].view.viewportWidth,
                           l->players[currentHovercraft].view.viewportHeight);
                if(currentHovercraft==l->iWinner)
                {
                    Bounds2P b = makeBounds2P(-window.orthoGLX*0.5,window.orthoGLY*0.5,
                                              window.orthoGLX*0.5,-window.orthoGLY*0.5);
                    drawTextureQuad(40,&white,&b);
                }
                else{
                    glColor3f(0.8,0.2,0.5);
                    float width  =  glutBitmapLength(GLUT_BITMAP_HELVETICA_18,"FAILED :C")*(window.orthoGLX*2)/window.width;
                    float height = glutBitmapWidth(GLUT_BITMAP_HELVETICA_18,'_')*4;
                    vBitmapOutput(-width/2,-height/2,"FAILED :C",GLUT_BITMAP_HELVETICA_18);
                }
            }
        }
        l->players[currentHovercraft].view.leftTop = leftTop;
        l->players[currentHovercraft].view.rightBottom = rightBottom;
        glPopMatrix();
    }
}

void specialDrawLevel(const Level *level, float state){
    int i;
    if(!state){
        for(i=0;i<level->nbPlayers;i++){
            specialDrawView(level, i, state);
        }
    }
    else if(state<50){
        for(i=0;i<level->nbPlayers;i++){
            specialDrawView(level, i, state);
        }
        Bounds2P b = makeBounds2P(-window.orthoGLX,window.orthoGLY,window.orthoGLX,-window.orthoGLY);
        glPushMatrix();
        if(state>20){
            if(state<30){
                glScalef(0.5+(state-20.)/100.,0.5+(state-20.)/100.,1);
                drawTextureQuad(getTexture(3), &white, &b);
            }
            else if(state<40){
                glScalef(0.5+(state-30.)/100.,0.5+(state-30.)/100.,1);
                drawTextureQuad(getTexture(2), &white, &b);
            }
            else{
                glScalef(0.5+(state-40.)/100.,0.5+(state-40.)/100.,1);
                drawTextureQuad(getTexture(1), &white, &b);
            }
        }
        glPopMatrix();
    }
    else if(state<65){
        if(level->specialObject!=NULL){
            for(i=0;i<level->nbPlayers;i++){
                Vector2D distance = makeVectorAB(makePoint(level->players[i].physical_body.x,level->players[i].physical_body.y),
                                                 makePoint(level->specialObject->x,level->specialObject->y));
                float coef = (state-50.)/15.;
                level->players[i].view.leftTop.x += distance.x * coef;
                level->players[i].view.leftTop.y += distance.y * coef;
                level->players[i].view.rightBottom.x += distance.x * coef;
                level->players[i].view.rightBottom.y += distance.y * coef;
                drawView(level,i);
                level->players[i].view.leftTop.x -= distance.x * coef;
                level->players[i].view.leftTop.y -= distance.y * coef;
                level->players[i].view.rightBottom.x -= distance.x * coef;
                level->players[i].view.rightBottom.y -= distance.y * coef;
            }
        }
    }
    else if(state<75){
        if(level->specialObject!=NULL){
            for(i=0;i<level->nbPlayers;i++){
                Vector2D distance = makeVectorAB(makePoint(level->players[i].physical_body.x,level->players[i].physical_body.y),
                                                 makePoint(level->specialObject->x,level->specialObject->y));
                level->players[i].view.leftTop.x += distance.x;
                level->players[i].view.leftTop.y += distance.y;
                level->players[i].view.rightBottom.x += distance.x;
                level->players[i].view.rightBottom.y += distance.y;
                drawView(level,i);
                level->players[i].view.leftTop.x -= distance.x ;
                level->players[i].view.leftTop.y -= distance.y ;
                level->players[i].view.rightBottom.x -= distance.x ;
                level->players[i].view.rightBottom.y -= distance.y ;
            }
        }
    }
    else if(state<80){
        if(level->specialObject!=NULL){
            for(i=0;i<level->nbPlayers;i++){
                Vector2D distance = makeVectorAB(makePoint(level->players[i].physical_body.x,level->players[i].physical_body.y),
                                                 makePoint(level->specialObject->x,level->specialObject->y));
                float coef = (80-state)/5.;
                level->players[i].view.leftTop.x += distance.x * coef;
                level->players[i].view.leftTop.y += distance.y * coef;
                level->players[i].view.rightBottom.x += distance.x * coef;
                level->players[i].view.rightBottom.y += distance.y * coef;
                drawView(level,i);
                level->players[i].view.leftTop.x -= distance.x * coef;
                level->players[i].view.leftTop.y -= distance.y * coef;
                level->players[i].view.rightBottom.x -= distance.x * coef;
                level->players[i].view.rightBottom.y -= distance.y * coef;
            }
        }
    }
    else if(state<90){
        for(i=0;i<level->nbPlayers;i++){
            specialDrawView(level, i, state);
        }
    }
    else if(state<95){
        for(i=0;i<level->nbPlayers;i++){
            specialDrawView(level, i, 87.5-(100.-state)/2.);
        }
    }
    else{
        for(i=0;i<level->nbPlayers;i++){
            specialDrawView(level, i, state);
        }
    }
}

int specialUpdateLevel(Level *level, float *state){
    if(*state<20){
        (*state) +=0.5;
        return *state<50.;
    }
    else if(*state<50){
        (*state) +=0.25;
        return *state<50.;
    }
    else if(*state<80){
        (*state) +=0.1;
        return *state<80.;
    }
    else if(*state<90){
        if(*state<85)
            (*state) +=0.1;
        else
            (*state) +=0.03;
        addRandomItem(&level->map);
        int i;
        if(*state>=90){
            for (i=0;i<level->map.nbItems;i++){
                level->map.potentialItem->freq=0;
            }
            return 0;
        }
        return 1;
    }
    else{
        if(*state>=100){
            if(!Game.transitionMode){
                transitionRightMode(MODE_MAINMENU);
            }
        }
        else{
            if(*state<95)
                (*state) +=0.05;
            else
                (*state) +=0.03;
            stopAudio(4);
            stopAudio(1);
            stopAudioFadeOut(level->map.audioID,0.5);
        }
        return 1;
    }
}

void initLevel(Level *level, int mapid, int nbplayers, int nbBots){
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
    level->nbPlayers = nbplayers;
    level->players = malloc(sizeof(Hovercraft)*nbplayers);
    int i;
    switch (nbplayers) {
    case 1:
        initHovercraft(level->players,&level->map,1,makePoint(0,0),
                       window.width,window.height, getControls(CONTROLKEYBOARD1));
        break;
    case 2:
        initHovercraft(level->players,&level->map,1,makePoint(0,0),
                       window.width/2.,window.height, getControls(CONTROLKEYBOARD1));
        initHovercraft(level->players+1,&level->map,2,makePoint(window.width/2.,0),
                       window.width/2.,window.height, getControls(CONTROLJOYSTICK));
        break;
    case 3:
        initHovercraft(level->players,&level->map,1,makePoint(0,window.height/2.),
                       window.width/2.,window.height/2, getControls(CONTROLKEYBOARD1));
        initHovercraft(level->players+1,&level->map,2,makePoint(window.width/2.,window.height/2.),
                       window.width/2.,window.height/2, getControls(CONTROLKEYBOARD2));
        initHovercraft(level->players+2,&level->map,3,makePoint(0,0),
                       window.width/2,window.height/2, getControls(CONTROLKEYBOARD3));
        break;
    case 4:
        initHovercraft(level->players,&level->map,1,makePoint(0,window.height/2),
                       window.width/2.,window.height/2, getControls(CONTROLKEYBOARD1));
        initHovercraft(level->players+1,&level->map,2,makePoint(window.width/2.,window.height/2),
                       window.width/2.,window.height/2, getControls(CONTROLKEYBOARD2));
        initHovercraft(level->players+2,&level->map,3,makePoint(0,0),
                       window.width/2,window.height/2, getControls(CONTROLKEYBOARD3));
        initHovercraft(level->players+3,&level->map,4,makePoint(window.width/2,0),
                       window.width/2,window.height/2, getControls(CONTROLMOUSE));
        break;
    default:
        break;
    }
    float angle=0;
    for(i=0;i<nbplayers;i++){
        level->players[i].physical_body.x += 8*cosf(angle);
        level->players[i].physical_body.y += 8*sinf(angle);
        level->players[i].physical_body.angle = angle*180./M_PI;
        angle+=M_PI/2.;
        updateViewOfHovercraft(level->players+i);
    }

    level->nbBots = nbBots;
    level->bots = malloc(sizeof(BotHovercraft)*nbBots);
    angle=nbplayers*M_PI/2.;
    for(i=0;i<nbBots;i++){
        initBotHovercraft(level->bots+i,&level->map,0.002,makePoint(0,0),window.width,window.height);
        level->bots[i].h.physical_body.x += 8*cos(angle);
        level->bots[i].h.physical_body.y += 8*sin(angle);
        level->bots[i].h.physical_body.angle = angle*180./M_PI;
        angle+=M_PI/2.;
    }
    level->state=0;
    level->indice=0;
    level->ms = 0;
    level->festival=0;
    level->specialObject = NULL;
}

void drawLevel(const Level* level){
    int i;
    if(level->nbPlayers){
        for(i=0;i<level->nbPlayers;i++){
            drawView(level,i);
        }
    }
    else{
        drawView(level,-1);
    }
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float glX =window.orthoGLX*4, glY=glX*(level->map.height/level->map.width);
    gluOrtho2D(0,glX,0,glY);
    switch(level->nbPlayers){
    case 1:
        glViewport(window.width-200,window.height - 200*(level->map.height/level->map.width),
                   200,200*(level->map.height/level->map.width));
        break;
    case 2:
        glViewport(window.width/2-100,window.height - 200*(level->map.height/level->map.width),
                   200,200*(level->map.height/level->map.width));
        break;
    default:
        glViewport(window.width/2-100,window.height/2 + 50*(level->map.height/level->map.width),
                   200,window.height*0.3*(level->map.height/level->map.width));
        break;
    }

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glScalef((glX-20)/level->map.width,(glY-40)/level->map.height,1);
    glTranslatef(level->map.width/2+20,level->map.height/2+40,0);
    drawMiniMap(&level->map);
    for(i=0;i<level->nbPlayers;i++)
        drawMiniHovercraft(level->players+i);
    for(i=0;i<level->nbBots;i++){
        drawMiniHovercraft(&level->bots[i].h);
    }
    glLoadIdentity();
    glColor3ub(255,255,255);
    glBegin(GL_QUADS);
    glColor3f(0.3,0.7,0.3);
    glVertex3f(10,0,1);
    glVertex3f(0,10,1);
    glVertex3f(0,glY-10,1);
    glVertex3f(10,glY,1);
    glColor3f(0.1,0.0,0.1);
    glEnd();
    glBegin(GL_QUADS);
    glColor3f(0.3,0.7,0.3);
    glVertex3f(glX-10,0,1);
    glVertex3f(glX,10,1);
    glVertex3f(glX,glY-10,1);
    glVertex3f(glX-10,glY,1);
    glColor3f(0.1,0.0,0.1);
    glEnd();
    glBegin(GL_QUADS);
    glColor3f(0.3,0.7,0.3);
    glVertex3f(0,20,0);
    glVertex3f(20,0,1);
    glVertex3f(glX-20,0,1);
    glVertex3f(glX,20,0);
    glColor3f(0.1,0.0,0.1);
    glEnd();
    glBegin(GL_QUADS);
    glColor3f(0.3,0.7,0.3);
    glVertex3f(0,glY-20,0);
    glVertex3f(20,glY,1);
    glVertex3f(glX-20,glY,1);
    glVertex3f(glX,glY-20,0);
    glColor3f(0.1,0.0,0.1);
    glEnd();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0,glX,0,glY);
    switch(level->nbPlayers){
    case 1:
        glViewport(window.width*0.8,0,window.width*0.2,window.width*0.2*(level->map.height/level->map.width));
        break;
    case 2:
        glViewport(window.width*0.5-window.width*0.1,window.height - 450*(level->map.height/level->map.width),
                   window.width*0.2,window.width*0.2*(level->map.height/level->map.width));
        break;
    default:
        glViewport(window.width*0.5-window.width*0.1,window.height/2 - 300*(level->map.height/level->map.width),
                   window.width*0.2,window.width*0.2*(level->map.height/level->map.width));
        break;
    }
    char* secondSTR = malloc(sizeof(char)*10);
    sprintf(secondSTR,"%d",(int)(level->map.time/1000.));
    Shape circle;
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    float cosInc = cosf((float)M_PI/25.);
    float sinInc = sinf((float)M_PI/25.);
    float x=1,y=0, oldX;
    glTranslatef(glX/2,glY/2,0);
    glBegin(GL_TRIANGLE_FAN);
    for(i=0; i<=50; i++)
    {
        glColor3f(0.1,-0.5+y+(float)level->ms/420.,0.5);
        glVertex3f(x*glX/4.,y*glX/4.,0);
        oldX = x;
        x = x*cosInc - y*sinInc;
        y = oldX*sinInc + y*cosInc;
    }
    glEnd();
    makeCircle(&circle,glX/5,makePoint(0, 0),
               makeColor4f(0.2,0.36,0.55,0.1),1);
    drawShape(&circle);
    glColor3ub(255,200,180);
    if(window.width<700){
        float width =  glutBitmapLength(GLUT_BITMAP_HELVETICA_18,secondSTR)*(window.orthoGLX*10)/window.width;
        float height = glutBitmapWidth(GLUT_BITMAP_HELVETICA_18,'_')*4;
        vBitmapOutput(- width, -height*0.6 ,secondSTR,GLUT_BITMAP_HELVETICA_18);
    }
    else{
        float width =  glutBitmapLength(GLUT_BITMAP_TIMES_ROMAN_24,secondSTR)*(window.orthoGLX*10)/window.width;
        float height = glutBitmapWidth(GLUT_BITMAP_TIMES_ROMAN_24,'_')*2;
        vBitmapOutput(- width, -height*0.6 ,secondSTR,GLUT_BITMAP_TIMES_ROMAN_24);
    }
    free(secondSTR);
    glColor3f(1,1,1);
}

void updateLevel(Level* level){
    updateMap(&(level->map));
    if(level->map.time<=20000 && level->specialObject==NULL){
        //Create special Object
        level->specialObject = malloc(sizeof(Object));
        Object* tocpy = &level->map.potentialItem[level->map.nbItems-1].object;
        makeObject(level->specialObject,tocpy->nbEffect+1,tocpy->nbShapes,
                   tocpy->life,tocpy->colliderRadius,tocpy->isStatic,0,0);
        int i;
        for(i=0;i<tocpy->nbShapes;i++){
            cpyShape(level->specialObject->shapes+i,tocpy->shapes+i);
        }
        for(i=0;i<tocpy->nbEffect;i++){
            level->specialObject->effectDelays[i] = tocpy->effectDelays[i];
            level->specialObject->effectsAtCollision[i] = tocpy->effectsAtCollision[i];
            level->specialObject->effectsTypesAtCollision[i] = tocpy->effectsTypesAtCollision[i];
        }
        level->specialObject->effectDelays[i]=1;
        level->specialObject->effectsTypesAtCollision[i] = FESTIVAL;
        level->specialObject->effectsAtCollision[i].festival.level = level;
        level->specialObject->shapes->color = makeColor4f(0.5,0.9,0.3,1);
        char ok = 0;
        Chained_Object* co;
        Intersection inter;

        int cpt=0;
        while(!ok){
            co = level->map.objects;
            ok = 1;
            level->specialObject->x = (float)(rand()%(int)(level->map.bounds.rightBottom.x
                                                           -level->map.bounds.leftBottom.x))
                    + level->map.bounds.leftBottom.x;
            level->specialObject->y = (float)(rand()%(int)(level->map.bounds.leftTop.y-
                                                           level->map.bounds.rightBottom.y))
                    + level->map.bounds.leftBottom.y;
            level->specialObject->vx =20*(float)(rand()%2)-1;
            level->specialObject->vy =20*(float)(rand()%2)-1;
            while(co!=NULL){
                ok = !(collisionBetweenObject(level->specialObject,co->object,&inter));
                if(!ok) break;
                co = co->next;
            }
            cpt++;
            if(cpt>5){
                freeObject(&level->specialObject);
                return;
            }
        }
        Game.specialMode=1;
        Game.specialState=50.1;
        level->specialObject->vx = level->specialObject->vy = 0;
        level->specialObject->vAngle = 1;
        addObjectToMap(&level->map,level->specialObject,makePoint(level->specialObject->x,level->specialObject->y));
        level->map.items = makeChainedObject(level->specialObject,level->map.items,makePoint(level->specialObject->x,level->specialObject->y));
    }
    if(level->map.time<=0){
        Game.specialMode = 1;
        Game.specialState = 91;
        unsigned int pointsMax=0;
        int i;
        for(i=0;i<level->nbBots;i++)
        {
            if(level->bots[i].h.points>pointsMax)
                pointsMax = level->bots[i].h.points;
        }
        level->iWinner=-1;
        for(i=0;i<level->nbPlayers;i++)
        {
            if(level->players[i].points>pointsMax){
                pointsMax = level->players[i].points;
                level->iWinner = i;
            }
        }
        return;
    }
    int i =0,j;
    Chained_Object* co;
    for(;i<level->nbPlayers;i++){
        applyFrottement(&(level->map), &(level->players[i].physical_body));
        for(j=i+1;j<level->nbPlayers;j++){
            handleCollision(&(level->players[i].physical_body),&(level->players[j].physical_body));
        }
        co = level->map.objects;
        while(co!=NULL){
            handleCollision(&(level->players[i].physical_body), co->object);
            co = co->next;
        }
        updateHovercraft(level->players+i);
        updateViewOfHovercraft(level->players+i);
    }
    for(i=0;i<level->nbBots;i++){
        applyFrottement(&(level->map), &(level->bots[i].h.physical_body));
        updateBotHovercraft(level->bots+i,level->map.objects,level->map.items);
        for(j=0;j<level->nbPlayers;j++){
            handleCollision(&(level->bots[i].h.physical_body),&(level->players[j].physical_body));
        }
        co = level->map.objects;
        while(co!=NULL){
            handleCollision(&(level->bots[i].h.physical_body), co->object);
            co = co->next;
        }
        for(j=i+1;j<level->nbBots;j++){
            handleCollision(&(level->bots[i].h.physical_body),&(level->bots[j].h.physical_body));
        }
    }
    level->state+=1;
    if(level->state%100==0){
        float randomvalue;
        randomvalue = (float)rand()/(float)RAND_MAX;
        level->indice = (level->nbBots+2) * randomvalue;
    }
    level->ms += MILLISECOND_PER_FRAME;
    if(level->ms>1000) level->ms = 0;
}

void handleEventLevelResize(Level *l){
    switch (l->nbPlayers) {
    case 1:
        l->players[0].view.viewportWidth  = window.width;
        l->players[0].view.viewportHeight =window.height;
        break;
    case 2:
        l->players[0].view.viewportWidth  = window.width/2;
        l->players[0].view.viewportHeight =window.height;
        l->players[1].view.viewportleftTop.x  = window.width/2;
        l->players[1].view.viewportWidth  = window.width/2;
        l->players[1].view.viewportHeight =window.height;
        break;
    case 3:
        l->players[0].view.viewportWidth  = window.width/2;
        l->players[0].view.viewportHeight =window.height/2;
        l->players[0].view.viewportleftTop.y  = window.height/2;
        l->players[1].view.viewportleftTop.x  = window.width/2;
        l->players[1].view.viewportleftTop.y  = window.height/2;
        l->players[1].view.viewportWidth  = window.width/2;
        l->players[1].view.viewportHeight =window.height/2;
        l->players[2].view.viewportWidth  = window.width/2;
        l->players[2].view.viewportHeight =window.height/2;
        break;
    case 4:
        l->players[0].view.viewportWidth  = window.width/2;
        l->players[0].view.viewportHeight =window.height/2;
        l->players[0].view.viewportleftTop.y  = window.height/2;
        l->players[1].view.viewportleftTop.x  = window.width/2;
        l->players[1].view.viewportleftTop.y  = window.height/2;
        l->players[1].view.viewportWidth  = window.width/2;
        l->players[1].view.viewportHeight =window.height/2;
        l->players[2].view.viewportWidth  = window.width/2;
        l->players[2].view.viewportHeight =window.height/2;
        l->players[3].view.viewportleftTop.x  = window.width/2;
        l->players[3].view.viewportWidth  = window.width/2;
        l->players[3].view.viewportHeight =window.height/2;

        break;
    default:
        break;
    }
}

void handleEventLevel(Level *l, const SDL_Event *event){
    int i;
    if(event->type==SDL_VIDEORESIZE)
        handleEventLevelResize(l);
    for(i=0;i<l->nbPlayers;i++){
        handleEventHovercraft(l->players+i,event);
    }
}

void activeFestival(Level* l){
    Game.specialMode = 1;
    Game.specialState = 81;
    int i;
    stopAudio(l->map.audioID);
    playAudio(6);
    playAudioFadeIn(4,5);
    for (i=0;i<l->map.nbItems;i++){
        l->map.potentialItem->freq *=10;
    }
}
