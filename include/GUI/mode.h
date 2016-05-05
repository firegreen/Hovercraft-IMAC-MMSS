#ifndef MODE_H
#define MODE_H

typedef enum Mode {MODE_MAINMENU, MODE_LEVELMENU, MODE_LEVEL, MODE_ONEPLAYER, MODE_TWOPLAYERS, MODE_INSTRUCTIONS, MODE_CREDITS} Mode;

void drawMode(Mode mode);
void updateMode(Mode mode);

#endif
