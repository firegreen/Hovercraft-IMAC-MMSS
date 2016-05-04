#include <stdlib.h>
#include "SDL_tools.h"
#include "game.h"
#include <time.h>

int main(int argc, char** argv){
  srand( (unsigned)time( NULL ) );
  initializeGame();
  game();
  return 0;
}
