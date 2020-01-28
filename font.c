#include <tchar.h>
#include "font.h"
#include "resman.h"
#include "loop.h"

extern void *screen;
MyImage font;
void LoadFont()
{
	LoadMyImage(L"font.bmp", &font);
}

void DrawLetter(int x, int y, int letter)
{
	float width = font.width / 10.f;
	int height = font.height;
	float startX = width * letter;
	for (int iy = 0; iy < height; iy++)
	{
		unsigned int point = (iy*font.width + startX) * font.widthPix;
		for (int ix = 0; ix < width; ix++)
		{
			if (font.imgBits[point + 2] || font.imgBits[point + 1] || font.imgBits[point])
			{
				DrawPixel(x + ix, y + iy, font.imgBits[point + 2], font.imgBits[point + 1], font.imgBits[point]);
			}
			point += font.widthPix;
		}
	}
}

void PrintNum(int x, int y, int num)
{
	x += 4*font.width/10;
	int i = 0;
	while(num && i<4)
	{
		DrawLetter(x, y, num % 10);
		x -= font.width / 10;
		num /= 10;
		i++;
	}
}

void DeleteFont()
{
	free(font.imgBits);
}