#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "genmap.h"

void GenMap(int genWidth, int genHeight, char **pmap, float *plX, float *plY)
{
	int minSize = 5, maxSize = 30;
	int mapArea = genWidth * genHeight;
	char *map = (char*)calloc(mapArea, sizeof(char));
	for (int i = 0; i < mapArea; i++)
	{
		map[i] = '1';
	}
	int count = 0;
	MyRect *rArr = NULL;
	int area = 0;
	while (area < mapArea / 3)
	{
		MyRect rect = { 0 };
		int s;
		do {
			s = GenRect(&rect, maxSize, minSize, genWidth, genHeight);
		} while (s >= mapArea / 10);

		int flag = 0;
		for (int i = 0; i < count; i++)
		{
			if (Intersect(&rect, &rArr[i]))
			{
				flag = 1;
				break;
			}
		}
		if (!flag)
		{
			count++;
			rArr = realloc(rArr, sizeof(MyRect)*count);
			rArr[count - 1] = rect;
			area += s;
			BurnRect(&rect, genWidth, map);
			*plX = rect.cx * 64.f;
			*plY = rect.cy * 64.f;
			if (count > 1)
			{
				int oldx = rArr[count - 2].cx;
				int oldy = rArr[count - 2].cy;
				int newx = rArr[count - 1].cx;
				int newy = rArr[count - 1].cy;
				if (1)
				{
					BurnHCor(oldx, newx, oldy, genWidth, map);
					BurnVCor(oldy, newy, newx, genWidth, map);
				}
				else
				{
					BurnVCor(oldy, newy, newx, genWidth, map);
					BurnHCor(oldx, newx, oldy, genWidth, map);
				}
			}
		}
	}
	free(rArr);
	*pmap = map;
}

int Intersect(MyRect *r1, MyRect *r2)
{
	int res = (r1->x1 <= r2->x2) && (r1->x2 >= r2->x1) &&
		(r1->y1 <= r2->y2) && (r1->y2 >= r2->y1);
	return res;
}

int GenRect(MyRect *rect, int maxSize, int minSize, int genWidth, int genHeight)
{
	int width = rand() % (maxSize - minSize) + minSize;
	int height = rand() % (maxSize - minSize) + minSize;
	rect->x1 = rand() % (genWidth - width - 1) + 1;
	rect->y1 = rand() % (genHeight - height - 1) + 1;
	rect->x2 = rect->x1 + width;
	rect->y2 = rect->y1 + height;
	rect->cx = rect->x1 + width / 2;
	rect->cy = rect->y1 + height / 2;

	int area = width * height;
	return area;
}

void BurnRect(MyRect *rect, int genWidth, char *map)
{
	int width = rect->x2 - rect->x1;
	int height = rect->y2 - rect->y1;
	int pstart = rect->y1*genWidth + rect->x1;
	for (int i = 0; i < height; i++)
	{
		int point = pstart;
		for (int j = 0; j < width; j++)
		{
			map[point] = '.';
			point++;
		}
		pstart += genWidth;
	}
}

void BurnVCor(int y1, int y2, int x, int genWidth, char *map)
{
	int point = 0, count = 0;
	if (y1 > y2)
	{
		point = y2 * genWidth + x;
		count = y1 - y2;
	}
	else
	{
		point = y1 * genWidth + x;
		count = y2 - y1;
	}
	for (int i = 0; i <= count; i++)
	{
		map[point] = '.';
		point += genWidth;
	}
}

void BurnHCor(int x1, int x2, int y, int genWidth, char *map)
{
	int point = 0, count = 0;
	if (x1 > x2)
	{
		point = y * genWidth + x2;
		count = x1 - x2;
	}
	else
	{
		point = y * genWidth + x1;
		count = x2 - x1;
	}
	for (int i = 0; i <= count; i++)
	{
		map[point] = '.';
		point++;
	}
}