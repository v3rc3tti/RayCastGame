#ifndef MENU_H

#include "resman.h"

typedef enum MenuIt_{
	M_CONTINUE,
	M_SAVE,
	M_LOAD,
	M_GEN,
	M_STAT,
	M_EXIT
}MenuIt;

typedef struct MyButton_ {
	MyImage img;
	int state;
}MyButton;

void InitMenu();
void MenuProc(enum GameState *gs);
void DrawButton(int x, int y, MyButton *button);
void DelMenu();

#endif // !MENU_H
