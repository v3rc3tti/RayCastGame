#ifndef RESMAN_H
#define RESMAN_H
#include <stdint.h>

typedef struct PlStat_ {
	wchar_t name[30];
	int score;
} PlStat;

typedef enum SprType_ {
	BULLET,
	ENEMY,
	ENBULLET
} SprType;

typedef struct MyImage_ {
	uint8_t *imgBits;
	int width;
	int height;
	int widthPix;
} MyImage;

typedef struct Sprite_ {
	SprType type;
	int health;
	float x;
	float y;
	float dx;
	float dy;
	MyImage tex;
} Sprite;

typedef struct ListIt_ {
	Sprite *data;
	uint8_t isDead;
	struct ListIt_ *next;
	struct ListIt_ *prev;
}ListIt;

typedef struct MyList_ {
	int count;
	ListIt *head;
	ListIt *tail;
}MyList;

void LoadMyImage(TCHAR *file, MyImage *imgStruct);

void AddSpriteToList(MyList *list, Sprite sp);
void DelItem(MyList *list, ListIt *dit);
void CleanList(MyList *list);
void SwapItems(MyList *list, ListIt *first, ListIt* sec);
#endif // RESMAN_H
