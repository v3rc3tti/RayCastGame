#include <tchar.h>
#include <stdint.h>
#include "menu.h"
#include "resman.h"
#include "loop.h"
#include "font.h"

extern uint8_t KeyMap[256];
extern int wndWidth;
extern int wndHeigth;
MyButton *buttons;
int bCount = M_EXIT+1;

void InitMenu()
{
	buttons = (MyButton*) malloc(bCount * sizeof(MyButton));

	LoadMyImage(L"continue.bmp", &buttons[M_CONTINUE].img);
	buttons[M_CONTINUE].state = 1;
	LoadMyImage(L"save.bmp", &buttons[M_SAVE].img);
	buttons[M_SAVE].state = 0;
	LoadMyImage(L"load.bmp", &buttons[M_LOAD].img);
	buttons[M_LOAD].state = 0;
	LoadMyImage(L"generate.bmp", &buttons[M_GEN].img);
	buttons[M_GEN].state = 0;
	LoadMyImage(L"stat.bmp", &buttons[M_STAT].img);
	buttons[M_STAT].state = 0;
	LoadMyImage(L"exit.bmp", &buttons[M_EXIT].img);
	buttons[M_EXIT].state = 0;

}

void MenuProc(enum GameState *gs)
{
	if (KEY_UP(VK_DOWN))
	{
		KeyMap[VK_DOWN] = 0;
	}
	if (KEY_DOWN(VK_DOWN) && !KeyMap[VK_DOWN])
	{
		PlaySound(L"button-17.wav", NULL, SND_ASYNC | SND_FILENAME);
		KeyMap[VK_DOWN] = 1;
		int nxti = 0;
		for (int i = 0; i < bCount; i++)
		{
			if (buttons[i].state)
			{
				nxti = i + 1;
				buttons[i].state = 0;
				break;
			}
		}
		if(nxti >= bCount)
		{
			nxti = 0;
		}
		buttons[nxti].state = 1;
	}
	if (KEY_UP(VK_UP))
	{
		KeyMap[VK_UP] = 0;
	}
	if (KEY_DOWN(VK_UP) && !KeyMap[VK_UP])
	{
		PlaySound(L"button-17.wav", NULL, SND_ASYNC | SND_FILENAME);
		KeyMap[VK_UP] = 1;
		int nxti = 0;
		for (int i = 0; i < bCount; i++)
		{
			if (buttons[i].state)
			{
				nxti = i - 1;
				buttons[i].state = 0;
				break;
			}
		}
		if (nxti < 0)
		{
			nxti = bCount-1;
		}
		buttons[nxti].state = 1;
	}
	if (KEY_UP(VK_ESCAPE))
	{
		KeyMap[VK_ESCAPE] = 0;
	}
	if (KEY_UP(VK_RETURN))
	{
		KeyMap[VK_RETURN] = 0;
	}
	if (KEY_DOWN(VK_RETURN) && !KeyMap[VK_RETURN])
	{
		PlaySound(L"metal_crunch.wav", NULL, SND_FILENAME|SND_ASYNC);
		KeyMap[VK_RETURN] = 1;
		int index = 0;
		for (int i = 0; i < bCount; i++)
		{
			if (buttons[i].state)
			{
				index = i;
				break;
			}
		}
		switch (index)
		{
			case M_CONTINUE:
				*gs = ONGAME;
			break;
			case M_SAVE:
				*gs = SAVEGAME;
			break;
			case M_LOAD:
				*gs = LOADGAME;
			break;
			case M_GEN:
				*gs = GENMAP;
			break;
			case M_STAT:
				*gs = SHOWST;
			break;
			case M_EXIT:
				*gs = EXITGAME;
			break;
		}
	}
	int x = (wndWidth - buttons[0].img.width-10);
	DrawButton(x, 1, &buttons[0]);
	DrawButton(x, buttons[0].img.height / 2 + 1, &buttons[1]);
	DrawButton(x, buttons[0].img.height, &buttons[2]);
	DrawButton(x, 3*buttons[0].img.height/2, &buttons[3]);
	DrawButton(x, 2*buttons[0].img.height, &buttons[4]);
	DrawButton(x, 5 * buttons[0].img.height/2, &buttons[5]);
}
void DrawButton(int x, int y, MyButton *button)
{
	int height = button->img.height;
	int width = button->img.width;
	int bPX = button->img.widthPix;
	uint8_t *bits = button->img.imgBits;
	int startY = 0;
	if(button->state)
	{
		startY = height >> 1;
	}
	height >>= 1;
	int point = startY * bPX * width;
	for (int iy = 0; iy < height; iy++)
	{
		for (int ix = 0; ix < width; ix++)
		{
			DrawPixel(ix + x, iy + y, bits[point + 2], bits[point + 1], bits[point]);
			point += bPX;
		}
	}
}

void DelMenu()
{
	for (int i = 0; i < bCount; i++)
	{
		free(buttons[i].img.imgBits);
	}
	free(buttons);
}