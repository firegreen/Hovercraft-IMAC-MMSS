#ifdef __APPLE__
#include <SDL.h>
#else
#include <SDL/SDL.h>
#endif

typedef struct InputTrackball
{

    int x; // valeur x
    int y; // valeur y

} InputTrackball;


typedef struct Joystick
{

    SDL_Joystick *joystick; // le joystick
    char *boutons; // les boutons
    int *axes; // les axes
    int *chapeaux; // les chapeaux
    InputTrackball *trackballs; // les trackballs
    int numero; // le numero

} Joystick;



void initialiserJoystick(Joystick *input,int numeroJoystick);

void detruireJoystick(Joystick *input);

int handleJoystickEvent(Joystick *inputs, const SDL_Event* event);
