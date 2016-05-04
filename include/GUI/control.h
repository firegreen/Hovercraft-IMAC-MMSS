#ifdef __APPLE__
#include <SDL.h>
#else
#include <SDL/SDL.h>
#endif
#include "Object2D/geometry.h"
typedef enum { CONTROLKEYBOARD1=0, CONTROLKEYBOARD2, CONTROLKEYBOARD3,
               CONTROLMOUSE, CONTROLJOYSTICK ,NBCONTROLS} ControlID;
typedef enum { RIGHTBUTTON =0, LEFTBUTTON, WHEELUP, WHEELDOWN, NBBUTTONS } MouseButton;


typedef struct Mouse{
    unsigned char isPressed[NBBUTTONS];
    Point2D originPressedPos;
    Point2D currentPos;
    Point2D deltaPos;
} Mouse;


typedef struct Point2D InputTrackball;


typedef struct Joystick
{

    SDL_Joystick *joystick; // le joystick
    char *boutons; // les boutons
    int *axes; // les axes
    int *chapeaux; // les chapeaux
    InputTrackball *trackballs; // les trackballs
    int numero; // le numero

} Joystick;

void initControls();
SDL_Event* getControls(int i);
void initJoystick(Joystick *input,int numeroJoystick);
void initMouse(Mouse *input);

void detruireJoystick(Joystick *input);

int handleJoystickEvent(Joystick *inputs, const SDL_Event* event);
int handleMouseEvent(Mouse *input, const SDL_Event* event);

Point2D convertWindowPointToGLPoint(Point2D p);
