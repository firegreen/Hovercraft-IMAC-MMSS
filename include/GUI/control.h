#ifdef __APPLE__
#include <SDL.h>
#else
#include <SDL/SDL.h>
#endif
#include "Object2D/geometry.h"
typedef enum { RIGHTBUTTON =0, LEFTBUTTON, WHEELUP, WHEELDOWN, NBBUTTONS } MouseButton;

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

void initJoystick(Joystick *input,int numeroJoystick);
void detruireJoystick(Joystick *input);

int handleJoystickEvent(Joystick *inputs, const SDL_Event* event);

Point2D convertWindowPointToGLPoint(Point2D p);
