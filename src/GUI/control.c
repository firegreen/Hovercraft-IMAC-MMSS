#include <stdlib.h>
#include "GUI/control.h"
#include "SDL_tools.h"
#include "Object2D/hovercraft.h"

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


Point2D convertWindowPointToGLPoint(Point2D p){
    Point2D retour;
    retour.x = p.x * window.orthoGLX*2 / window.width -window.orthoGLX;
    retour.y = p.y * window.orthoGLY*-2 / window.height +window.orthoGLY;
    return retour;
}

