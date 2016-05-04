#ifndef MODE_H
#define MODE_H

typedef enum Mode {MODE_MAINMENU, MODE_LEVELMENU, MODE_LEVEL} Mode;

void drawMode(Mode mode);
void updateMode(Mode mode);

#endif
