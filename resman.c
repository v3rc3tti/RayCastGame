#include <windows.h>
#include <tchar.h>
#include "resman.h"

void LoadMyImage(TCHAR *file, MyImage *imgStruct)
{
    HBITMAP img = LoadImage(NULL, file, IMAGE_BITMAP, 0, 0,
                            LR_LOADFROMFILE|LR_CREATEDIBSECTION);
    if(!img)
    {
        MessageBox(NULL, file, _T("Can't load bitmap!"), MB_ICONERROR|MB_OK);
		PostQuitMessage(0);
		return;
    }
    BITMAP bm;
    GetObject(img, sizeof(BITMAP), &bm);
	imgStruct->width = bm.bmWidth;
	imgStruct->height = bm.bmHeight;
	imgStruct->widthPix = (bm.bmBitsPixel) >> 3;
	uint8_t *memImg = calloc(bm.bmWidth*bm.bmHeight, imgStruct->widthPix);
	GetBitmapBits(img, bm.bmWidth*bm.bmHeight*imgStruct->widthPix, memImg);
	imgStruct->imgBits = memImg;
	DeleteObject(img);
}

void AddSpriteToList(MyList *list, Sprite sp)
{
	ListIt *p = (ListIt*)malloc(sizeof(ListIt));
	Sprite *tmp = (Sprite*)malloc(sizeof(Sprite));
	*tmp = sp;
	p->data = tmp;
	p->isDead = 0;
	p->next = NULL;
	if (list->count > 0)
	{
		p->prev = list->tail;
		list->tail->next = p;
	}
	else
	{
		p->prev = NULL;
		list->head = p;
	}
	list->tail = p;
	list->count++;
}

void DelItem(MyList *list, ListIt *dit)
{
	if (list->count < 1) return;
	ListIt *pr = dit->prev;
	ListIt *nx = dit->next;
	if (pr != NULL)
	{
		pr->next = nx;
	}
	else
	{
		list->head = nx;
	}
	if (nx != NULL)
	{
		nx->prev = pr;
	}
	else
	{
		list->tail = pr;
	}
	free(dit->data);
	free(dit);
	list->count--;
}

void CleanList(MyList *list)
{
	ListIt *p = list->head;
	while (p!=NULL)
	{
		ListIt *tmp = p;
		p = p->next;
		free(tmp->data);
		free(tmp);
	}
	list->count = 0;
	list->head = NULL;
	list->tail = NULL;
}

void SwapItems(MyList *list, ListIt *first, ListIt* sec)
{
	Sprite *tmp = first->data;
	first->data = sec->data;
	sec->data = tmp;
	int t = first->isDead;
	first->isDead = sec->isDead;
	sec->isDead = t;
}