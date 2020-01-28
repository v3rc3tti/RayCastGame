#ifndef LOOP_H
#define LOOP_H

#include <windows.h>
#include "resman.h"

typedef enum InputState_ {
	WAIT,
	NOINP,
	READY
}InputState;

typedef enum GameState_{
	ONGAME,
	MENU,
	SAVEGAME,
	LOADGAME,
	GENMAP,
	DEATH,
	SHOWST,
	EXITGAME
}GameState;

void GameLaunch();
void GameMain();
void GameShutdown();
void DrawPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b);
int DoesRayHit(float xcoord, float ycoord);
void ControlProc(float *posA, float *posX, float *posY, MyList *list, float timeRatio, GameState *gs);
void DrawFrame(int playerA, int playerX, int playerY);
void DrawSprite(int playerX, int playerY, int playerA, MyList *list);
void ProcSprites(MyList *list, float timeRatio, int *score, int *health, float playerX, float playerY);
float GetTimeRatio(int reset);
float GetDist(const Sprite *s1, const Sprite *s2);
void SpawnEnemy(MyList *list, float timeRatio, int reset);

#define KEY_DOWN(x) (GetAsyncKeyState(x)&0x8000)
#define KEY_UP(x) (!(GetAsyncKeyState(x)&0x8000))

#endif // LOOP_H
