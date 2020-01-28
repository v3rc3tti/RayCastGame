#ifndef GENMAP_H
#define GENMAP_H

typedef struct MyRect_ {
	int x1;
	int x2;
	int y1;
	int y2;
	int cx;
	int cy;
}MyRect;

void GenMap(int genWidth, int genHeight, char **pmap, float *plX, float *plY);
int GenRect(MyRect *rect, int maxSize, int minSize, int genWidth, int genHeight);
void BurnRect(MyRect *rect, int genWidth, char *map);
int Intersect(MyRect *r1, MyRect *r2);
void BurnVCor(int y1, int y2, int x, int genWidth, char *map);
void BurnHCor(int x1, int x2, int y, int genWidth, char *map);

#endif
