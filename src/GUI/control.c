#include <stdlib.h>
#include "GUI/control.h"
#include "SDL_tools.h"
#include "Object2D/hovercraft.h"


SDL_Event controls[NBCONTROLS][NBACTIONS];

void initControls(){
    int i,j;
    for(i=CONTROLKEYBOARD1; i<=CONTROLKEYBOARD3;i++){
        for(j=0;j<NBACTIONS;j++){
            controls[i][j].type = SDL_KEYDOWN;
            controls[i][j].key.keysym.mod = KMOD_NONE;
        }
        controls[i][FORWARDSTOP].type = SDL_KEYUP;
        controls[i][BACKWARDSTOP].type = SDL_KEYUP;
        controls[i][ROTATELEFTSTOP].type = SDL_KEYUP;
        controls[i][ROTATERIGHTSTOP].type = SDL_KEYUP;
    }
    /*for(j=0;j<NBACTIONS;j++){
        controls[CONTROLKEYBOARD3][j].key.keysym.mod = KMOD_NUM;
    }*/
    controls[CONTROLKEYBOARD1][ZOOMMINUS].key.keysym.sym = SDLK_m;
    controls[CONTROLKEYBOARD1][ZOOMPLUS].key.keysym.sym = SDLK_p;
    controls[CONTROLKEYBOARD1][FORWARD].key.keysym.sym =
    controls[CONTROLKEYBOARD1][FORWARDSTOP].key.keysym.sym = SDLK_UP;
    controls[CONTROLKEYBOARD1][BACKWARD].key.keysym.sym =
    controls[CONTROLKEYBOARD1][BACKWARDSTOP].key.keysym.sym = SDLK_DOWN;
    controls[CONTROLKEYBOARD1][ROTATELEFT].key.keysym.sym =
    controls[CONTROLKEYBOARD1][ROTATELEFTSTOP].key.keysym.sym = SDLK_LEFT;
    controls[CONTROLKEYBOARD1][ROTATERIGHT].key.keysym.sym =
    controls[CONTROLKEYBOARD1][ROTATERIGHTSTOP].key.keysym.sym = SDLK_RIGHT;

    controls[CONTROLKEYBOARD2][ZOOMMINUS].key.keysym.sym = SDLK_r;
    controls[CONTROLKEYBOARD2][ZOOMPLUS].key.keysym.sym = SDLK_t;
#ifdef WIN32
    controls[CONTROLKEYBOARD2][FORWARD].key.keysym.sym =
    controls[CONTROLKEYBOARD2][FORWARDSTOP].key.keysym.sym = SDLK_w;
    controls[CONTROLKEYBOARD2][BACKWARD].key.keysym.sym =
    controls[CONTROLKEYBOARD2][BACKWARDSTOP].key.keysym.sym = SDLK_s;
    controls[CONTROLKEYBOARD2][ROTATELEFT].key.keysym.sym =
    controls[CONTROLKEYBOARD2][ROTATELEFTSTOP].key.keysym.sym = SDLK_a;
    controls[CONTROLKEYBOARD2][ROTATERIGHT].key.keysym.sym =
    controls[CONTROLKEYBOARD2][ROTATERIGHTSTOP].key.keysym.sym = SDLK_d;
#else
    controls[CONTROLKEYBOARD2][FORWARD].key.keysym.sym =
    controls[CONTROLKEYBOARD2][FORWARDSTOP].key.keysym.sym = SDLK_z;
    controls[CONTROLKEYBOARD2][BACKWARD].key.keysym.sym =
    controls[CONTROLKEYBOARD2][BACKWARDSTOP].key.keysym.sym = SDLK_s;
    controls[CONTROLKEYBOARD2][ROTATELEFT].key.keysym.sym =
    controls[CONTROLKEYBOARD2][ROTATELEFTSTOP].key.keysym.sym = SDLK_q;
    controls[CONTROLKEYBOARD2][ROTATERIGHT].key.keysym.sym =
    controls[CONTROLKEYBOARD2][ROTATERIGHTSTOP].key.keysym.sym = SDLK_d;
#endif
    controls[CONTROLKEYBOARD3][ZOOMMINUS].key.keysym.sym = SDLK_KP_PLUS;
    controls[CONTROLKEYBOARD3][ZOOMPLUS].key.keysym.sym = SDLK_KP_MINUS;
    controls[CONTROLKEYBOARD3][FORWARD].key.keysym.sym =
    controls[CONTROLKEYBOARD3][FORWARDSTOP].key.keysym.sym = SDLK_KP8;
    controls[CONTROLKEYBOARD3][BACKWARD].key.keysym.sym =
    controls[CONTROLKEYBOARD3][BACKWARDSTOP].key.keysym.sym = SDLK_KP5;
    controls[CONTROLKEYBOARD3][ROTATELEFT].key.keysym.sym =
    controls[CONTROLKEYBOARD3][ROTATELEFTSTOP].key.keysym.sym = SDLK_KP4;
    controls[CONTROLKEYBOARD3][ROTATERIGHT].key.keysym.sym =
    controls[CONTROLKEYBOARD3][ROTATERIGHTSTOP].key.keysym.sym = SDLK_KP6;

    controls[CONTROLMOUSE][ZOOMMINUS].type =
    controls[CONTROLMOUSE][ZOOMPLUS].type = SDL_MOUSEBUTTONDOWN;
    controls[CONTROLMOUSE][ZOOMPLUS].button.button = SDL_BUTTON_WHEELDOWN;
    controls[CONTROLMOUSE][ZOOMMINUS].button.button = SDL_BUTTON_WHEELUP;
    controls[CONTROLMOUSE][FORWARD].type =
    controls[CONTROLMOUSE][BACKWARD].type = SDL_MOUSEBUTTONDOWN;
    controls[CONTROLMOUSE][FORWARDSTOP].type =
    controls[CONTROLMOUSE][BACKWARDSTOP].type = SDL_MOUSEBUTTONUP;
    controls[CONTROLMOUSE][FORWARD].button.button =
    controls[CONTROLMOUSE][FORWARDSTOP].button.button = SDL_BUTTON_LEFT;
    controls[CONTROLMOUSE][BACKWARD].button.button =
    controls[CONTROLMOUSE][BACKWARDSTOP].button.button = SDL_BUTTON_RIGHT;
    controls[CONTROLMOUSE][ROTATELEFT].type =
    controls[CONTROLMOUSE][ROTATELEFTSTOP].type =
    controls[CONTROLMOUSE][ROTATERIGHT].type =
    controls[CONTROLMOUSE][ROTATERIGHTSTOP].type= SDL_MOUSEMOTION;

    controls[CONTROLJOYSTICK][ROTATERIGHT].type = SDL_JOYBUTTONDOWN;
    controls[CONTROLJOYSTICK][ROTATERIGHTSTOP].type = SDL_JOYBUTTONUP;
    controls[CONTROLJOYSTICK][ROTATERIGHT].jbutton.button =
    controls[CONTROLJOYSTICK][ROTATERIGHTSTOP].jbutton.button = 2;
    controls[CONTROLJOYSTICK][ROTATELEFT].type = SDL_JOYBUTTONDOWN;
    controls[CONTROLJOYSTICK][ROTATELEFTSTOP].type = SDL_JOYBUTTONUP;
    controls[CONTROLJOYSTICK][ROTATELEFT].jbutton.button =
    controls[CONTROLJOYSTICK][ROTATELEFTSTOP].jbutton.button = 1;
    controls[CONTROLJOYSTICK][BACKWARD].type = SDL_JOYBUTTONDOWN;
    controls[CONTROLJOYSTICK][BACKWARDSTOP].type = SDL_JOYBUTTONUP;
    controls[CONTROLJOYSTICK][BACKWARD].jbutton.button =
    controls[CONTROLJOYSTICK][BACKWARDSTOP].jbutton.button = 0;
    controls[CONTROLJOYSTICK][FORWARD].type = SDL_JOYBUTTONDOWN;
    controls[CONTROLJOYSTICK][FORWARDSTOP].type = SDL_JOYBUTTONUP;
    controls[CONTROLJOYSTICK][FORWARD].jbutton.button =
    controls[CONTROLJOYSTICK][FORWARDSTOP].jbutton.button = 3;
}

void initJoystick(Joystick *input,int numeroJoystick)
{

    if(numeroJoystick < SDL_NumJoysticks())
    {

        input->joystick = SDL_JoystickOpen(numeroJoystick);
        input->numero = numeroJoystick;
        input->boutons = (char*) malloc(SDL_JoystickNumButtons(input->joystick) * sizeof(char));
        input->axes = (int*) malloc(SDL_JoystickNumAxes(input->joystick) * sizeof(int));
        input->chapeaux = (int*) malloc(SDL_JoystickNumHats(input->joystick) * sizeof(int));
        input->trackballs = (InputTrackball*) malloc(SDL_JoystickNumBalls(input->joystick) * sizeof(InputTrackball));

        int i;
        for(i=0;i<SDL_JoystickNumButtons(input->joystick);i++)
            input->boutons[i] = 0;

        for(i=0;i<SDL_JoystickNumAxes(input->joystick);i++)
            input->axes[i] = 0;

        for(i=0;i<SDL_JoystickNumHats(input->joystick);i++)
            input->chapeaux[i] = SDL_HAT_CENTERED;

        for(i=0;i<SDL_JoystickNumBalls(input->joystick);i++)
        {
            input->trackballs[i].x = 0;
            input->trackballs[i].y = 0;

        }
    }
    else
    {
        input->joystick = NULL;
        input->boutons = NULL;
        input->axes = NULL;
        input->chapeaux = NULL;
        input->trackballs = NULL;
    }

}

void initMouse(Mouse *input){
    input->currentPos = makePoint(0,0);
    input->deltaPos = makePoint(0,0);
    int i;
    for(i=0;i<NBBUTTONS;i++) input->isPressed[i] =0;
    input->originPressedPos = makePoint(0,0);
}

void detruireJoystick(Joystick *input)
{

    if(input->joystick != NULL)
    {

        input->numero = 0;
        free(input->boutons);
        free(input->axes);
        free(input->chapeaux);
        free(input->trackballs);
        SDL_JoystickClose(input->joystick);
    }

}


int handleJoystickEvent(Joystick *inputs, const SDL_Event* event)
{
    switch(event->type)
    {
    case SDL_JOYBUTTONDOWN:
        if(inputs[event->jbutton.which].joystick!=NULL)
            inputs[event->jbutton.which].boutons[event->jbutton.button] = 1;
        return 1;

    case SDL_JOYBUTTONUP:
        if(inputs[event->jbutton.which].joystick!=NULL)
            inputs[event->jbutton.which].boutons[event->jbutton.button] = 0;
        return 1;

    case SDL_JOYAXISMOTION:
        if(inputs[event->jaxis.axis].joystick!=NULL)
            inputs[event->jaxis.axis].axes[event->jaxis.axis] = event->jaxis.value;
        return 1;

    case SDL_JOYHATMOTION:
        if(inputs[event->jhat.which].joystick!=NULL)
            inputs[event->jhat.which].boutons[event->jhat.hat] = event->jhat.value;
        return 1;

    case SDL_JOYBALLMOTION:
        if(inputs[event->jball.which].joystick!=NULL){
            inputs[event->jball.which].trackballs->x = event->jball.xrel;
            inputs[event->jball.which].trackballs->y = event->jball.yrel;
        }
        return 1;
    default:
        break;
    }
    return 0;
}

int handleMouseEvent(Mouse *input, const SDL_Event *event){
    switch(event->type)
    {
    case SDL_MOUSEMOTION:
        input->deltaPos = makeVectorAB(makePoint(event->motion.x,event->motion.y),input->currentPos);
        input->currentPos = makePoint(event->motion.x,event->motion.y);
        break;
    case SDL_MOUSEBUTTONUP:
        switch (event->button.which) {
        case SDL_BUTTON_LEFT:
            input->isPressed[LEFTBUTTON] = 0;
            break;
        case SDL_BUTTON_RIGHT:
            input->isPressed[RIGHTBUTTON] = 0;
            break;
        case SDL_BUTTON_WHEELDOWN:
            input->isPressed[WHEELDOWN] = 0;
            break;
        case SDL_BUTTON_WHEELUP:
            input->isPressed[WHEELUP] = 0;
            break;
        default:
            break;
        }
        break;
    case SDL_MOUSEBUTTONDOWN:
        switch (event->button.which) {
        case SDL_BUTTON_LEFT:
            input->isPressed[LEFTBUTTON] = 1;
            break;
        case SDL_BUTTON_RIGHT:
            input->isPressed[RIGHTBUTTON] = 1;
            break;
        case SDL_BUTTON_WHEELDOWN:
            input->isPressed[WHEELDOWN] = 1;
            break;
        case SDL_BUTTON_WHEELUP:
            input->isPressed[WHEELUP] = 1;
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
    return 0;
}

Point2D convertWindowPointToGLPoint(Point2D p){
    Point2D retour;
    retour.x = p.x * window.orthoGLX*2 / window.width -window.orthoGLX;
    retour.y = p.y * window.orthoGLY*-2 / window.height +window.orthoGLY;
    return retour;
}

SDL_Event* getControls(int i){
    if(i<0 || i>=NBCONTROLS)
        return controls[0];
    return controls[i];
}
