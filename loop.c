#define _USE_MATH_DEFINES
#include <windows.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <tchar.h>
#include "loop.h"
#include "mymath.h"
#include "resman.h"
#include "menu.h"
#include "font.h"
#include "genmap.h"
#include "main.h"

extern void *screen;
extern int wndHeight;
extern int wndWidth;
extern int USERINPUT;
extern wchar_t FileName[16];
extern uint8_t KeyMap[256];
extern HWND hwnd;

GameState STATE = MENU;
int FOV = 600;
int toPlane = 500;
int mapWidth = 20;
int mapHeight = 10;
char *map = NULL;/*"11111111111111111111"
             "1..................1"
             "1..................1"
             "1..................1"
             "1..................1"
             "1..............1...1"
             "1..................1"
             "1..................1"
             "1..................1"
             "11111111111111111111";*/
MyImage mem;
MyImage img1;
MyImage enemyImg;
MyImage enbImg;
MyImage docImg;
MyImage titImg;
float *depthBuf;
int scount;
int score;
PlStat statArr[10];

void GameLaunch()
{
	srand(time(NULL));
    InitMyMath();
	InitMenu();
	LoadFont();
    LoadMyImage(_T("texture.bmp"), &mem);
	LoadMyImage(_T("sprite.bmp"), &img1);
	LoadMyImage(_T("enemy1.bmp"), &enemyImg);
	LoadMyImage(_T("enbullet.bmp"), &enbImg);
	LoadMyImage(_T("doctor.bmp"), &docImg);
	LoadMyImage(_T("title.bmp"), &titImg);
	depthBuf = (float*) malloc(wndWidth * sizeof(float));
}

void GameMain()
{
    static float posA = 788;
    static float posX = 8*64;
    static float posY = 8*64;
	static int health = 10;
	static MyList list;
	static int resetTime = 1;
	switch (STATE)
	{
		case ONGAME:
		{
			if (!map)
			{
				MessageBox(hwnd, L"Перед игрой необходимо сгенерировать карту", L"ACHTUNG!", MB_OK | MB_ICONINFORMATION);
				STATE = MENU;
				break;
			}
			if (resetTime)
			{
				if(rand()%2)
					PlaySound(L"Bender-06.wav", NULL, SND_FILENAME | SND_ASYNC);
				else 
					PlaySound(L"Bender-07.wav", NULL, SND_FILENAME | SND_ASYNC);
			}
			float timeRatio = GetTimeRatio(resetTime);
			resetTime = 0;
			ControlProc(&posA, &posX, &posY, &list, timeRatio, &STATE);
			ProcSprites(&list, timeRatio, &score, &health, posX, posY);
			DrawFrame((int)posA, (int)posX, (int)posY);
			DrawSprite((int)posX, (int)posY, (int)posA, &list);
			PrintNum(wndWidth/2, 5*wndHeight/6, score);
			PrintNum(0, 5 * wndHeight / 6, health);
			SpawnEnemy(&list, timeRatio, resetTime);
			if (health <= 0)STATE = DEATH;
		}break;
		case MENU:
		{
			resetTime = 1;
			for (int x = 0; x < wndWidth; x++)
				for (int y = 0; y < wndHeight; y++)
				{
					DrawPixel(x, y, titImg.imgBits[(y*titImg.width + x)*titImg.widthPix + 2],
						titImg.imgBits[(y*titImg.width + x)*titImg.widthPix + 1],
						titImg.imgBits[(y*titImg.width + x)*titImg.widthPix]);
				}
			MenuProc(&STATE);
		}break;
		case EXITGAME:
		{
			PostQuitMessage(0);
		}break;
		case SAVEGAME:
		{
			if (map == NULL)
			{
				MessageBox(hwnd, L"Нет карты для загрузки", L"Ошибка", MB_OK | MB_ICONERROR);
				STATE = MENU;
				break;
			}
			resetTime = 1;
			for (int x = 0; x < wndWidth; x++)
				for (int y = 0; y < wndHeight; y++)
					DrawPixel(x, y, 255, 100, 100);
			if (USERINPUT == NOINP)
			{
				USERINPUT = WAIT;
			}
			else if (USERINPUT == READY)
			{
				STATE = MENU;
				USERINPUT = NOINP;
				if (FileName[0] && (CreateDirectory(L"saves", NULL) || ERROR_ALREADY_EXISTS == GetLastError() ))
				{
					wchar_t fullname[32] = L"./saves/";
					wcscat(fullname, FileName);
					wcscat(fullname, L".dat");
					FILE *file = _wfopen(fullname, L"wb");
					fprintf(file, "%fx%fx%f\n", posA, posX, posY);
					fprintf(file, "%dx%d\n", mapHeight, mapWidth);
					fwrite(map, sizeof(char), mapHeight*mapWidth, file);
					fclose(file);
				}
			}
		}break;
		case GENMAP:
		{
			CleanList(&list);
			resetTime = 1;
			score = 0;
			if (USERINPUT == NOINP)
			{
				USERINPUT = WAIT;
			}
			else if(USERINPUT == READY)
			{
				USERINPUT = NOINP;
				STATE = MENU;
			}
			for (int x = 0; x < wndWidth; x++)
				for (int y = 0; y < wndHeight; y++)
					DrawPixel(x, y, 255, 0, 0);
			free(map);
			posA = 0;
			GenMap(60, 60, &map, &posX, &posY);
			mapWidth = 60;
			mapHeight = 60;
		}break;
		case LOADGAME:
		{
			CleanList(&list);
			resetTime = 1;
			score = 0;
			for (int x = 0; x < wndWidth; x++)
				for (int y = 0; y < wndHeight; y++)
					DrawPixel(x, y, 255, 100, 100);
			if (USERINPUT == NOINP)
			{
				USERINPUT = WAIT;
			}
			else if (USERINPUT == READY)
			{
				STATE = MENU;
				USERINPUT = NOINP;
				if (FileName[0])
				{
					wchar_t fullname[32] = L"./saves/";
					wcscat(fullname, FileName);
					wcscat(fullname, L".dat");
					FILE *file = _wfopen(fullname, L"rb");
					if (file == NULL)
					{
						MessageBox(hwnd, L"Файл не найден", L"Ошибка", MB_OK | MB_ICONERROR);
						break;
					}
					fscanf(file, "%fx%fx%f\n", &posA, &posX, &posY);
					fscanf(file, "%dx%d\n", &mapHeight, &mapWidth);
					char *newMap = (char*)malloc(mapHeight*mapWidth);
					fread(newMap, sizeof(char), mapHeight*mapWidth, file);
					free(map);
					map = newMap;
					fclose(file);
				}
			}
		}break;
		case DEATH:
		{
			CleanList(&list);
			resetTime = 1;
			for (int x = 0; x < wndWidth; x++)
				for (int y = 0; y < wndHeight; y++)
				{
					DrawPixel(x, y, docImg.imgBits[(y*docImg.width + x)*docImg.widthPix + 2],
						docImg.imgBits[(y*docImg.width + x)*docImg.widthPix + 1],
						docImg.imgBits[(y*docImg.width + x)*docImg.widthPix]);
				}
			if (USERINPUT == NOINP)
			{
				USERINPUT = WAIT;
			}
			else if (USERINPUT == READY)
			{
				if (FileName[0])
				{
					wchar_t fullname[32] = L"stat.dat";
					FILE *file = _wfopen(fullname, L"r+b");
					wchar_t tbuf[32];
					int i = 0;
					while (fgetws(tbuf, 32, file) != NULL && i<10)
					{
						wchar_t *tmp = NULL;
						statArr[i].score = wcstol(tbuf, &tmp, 10);
						if (tbuf[wcslen(tbuf) - 1] == L'\n')
						{
							tbuf[wcslen(tbuf) - 1] = 0;
						}
						wcscpy(statArr[i].name, tmp + 1);
						i++;
					}
					if (statArr[9].score < score)
					{
						statArr[9].score = score;
						wcscpy(statArr[9].name, FileName);
					}
					for (int i = 0; i < 9; i++)
					{
						for (int j = i+1; j < 10; j++)
						{
							if (statArr[i].score < statArr[j].score)
							{
								PlStat tmp = statArr[i];
								statArr[i] = statArr[j];
								statArr[j] = tmp;
							}
						}
					}
					i = 0;
					rewind(file);
					while (statArr[i].score > 0)
					{
						wchar_t tbuf[40];
						swprintf(tbuf, 40, L"%d %s\n", statArr[i].score, statArr[i].name);
						fwrite(tbuf, sizeof(wchar_t), wcslen(tbuf), file);
						i++;
					}
					fclose(file);
				}
				score = 0;
				health = 100;
				STATE = MENU;
				USERINPUT = NOINP;
			}
		}break;
		case SHOWST:
		{
			resetTime = 1;
			for (int x = 0; x < wndWidth; x++)
				for (int y = 0; y < wndHeight; y++)
					DrawPixel(x, y, 255, 100, 100);
			if (USERINPUT == NOINP)
			{
				PlaySound(L"wooa.wav", NULL, SND_FILENAME | SND_ASYNC);
				wchar_t fullname[32] = L"stat.dat";
				FILE *file = _wfopen(fullname, L"r+b");
				wchar_t tbuf[32];
				int i = 0;
				while (fgetws(tbuf, 32, file) != NULL)
				{
					wchar_t *tmp = NULL;
					statArr[i].score = wcstol(tbuf, &tmp, 10);
					if (tbuf[wcslen(tbuf) - 1] == L'\n')
					{
						tbuf[wcslen(tbuf) - 1] = 0;
					}
					wcscpy(statArr[i].name, tmp + 1);
					i++;
				}
				fclose(file);
				USERINPUT = WAIT;
			}
			else if (USERINPUT == READY)
			{
				STATE = MENU;
				USERINPUT = NOINP;
			}
		}break;
	}
}

void GameShutdown()
{
    MathShutdown();
	DelMenu();
	DeleteFont();
	free(depthBuf);
	free(mem.imgBits);
	free(img1.imgBits);
	free(enemyImg.imgBits);
	free(enbImg.imgBits);
	free(docImg.imgBits);
	free(titImg.imgBits);
	free(map);
}

void DrawPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b)
{
    if(x<0||x>=wndWidth||y<0||y>=wndHeight)
        return;

    uint32_t *mem = (uint32_t*)screen;
    mem[wndWidth*y+x]=(r<<16)|(g<<8)|b;
}

int DoesRayHit(float xcoord, float ycoord)
{
	int x = (int)xcoord;
	int y = (int)ycoord;
	if ((y >> 6) >= mapHeight || (y >> 6) < 0 || (x >> 6) >= mapWidth || (x >> 6) < 0)
	{
		return -1;
	}
    if (map[(y>>6)*mapWidth+(x>>6)]=='.')   //y>>6 == y/64
        return 0;
    else
        return 1;
}

void ControlProc(float *posA, float *posX, float *posY, MyList *list, float timeRatio, GameState *gs)
{
	float newX = *posX;
	float newY = *posY;
	int angle = (int)*posA;

	if (KEY_DOWN(0x57))
	{
		newX += (float)MyCos(angle)*30 * timeRatio;
		newY += (float)MySin(angle)*30 * timeRatio;
	}
	if (KEY_DOWN(0x53))
	{
		newX -= (float)MyCos(angle)*30* timeRatio;
		newY -= (float)MySin(angle)*30* timeRatio;
	}
	if (KEY_DOWN(0x41))
	{
		newX += (float)MyCos(angle + 900)*30* timeRatio;
		newY += (float)MySin(angle + 900)*30* timeRatio;
	}
	if (KEY_DOWN(0x44))
	{
		newX += (float)MyCos(angle - 900)*30* timeRatio;
		newY += (float)MySin(angle - 900)*30* timeRatio;
	}
	if (!DoesRayHit(newX, newY) && !DoesRayHit(newX + 4, newY)
		&& !DoesRayHit(newX - 4, newY) && !DoesRayHit(newX, newY + 4)
		&& !DoesRayHit(newX, newY - 4))
	{
		*posX = newX;
		*posY = newY;
	}
	if (KEY_DOWN(0x51))
	{
		*posA += 100* timeRatio;
	}
	if (KEY_DOWN(0x45))
	{
		*posA -= 100* timeRatio;
	}
	while (*posA >= 3600.f)
	{
		*posA -= 3600;
	}
	if (*posA < 0.f)
	{
		*posA += 3600;
	}
	if (KEY_UP(0x46))
	{
		KeyMap[0x46] = 0;
	}
	if (KEY_DOWN(0x46)&&(!KeyMap[0x46]))
	{
		if (rand() % 4 > 1)
		{
			PlaySound(L"blurp_x.wav", NULL, SND_FILENAME | SND_ASYNC);
		}
		else
		{
			PlaySound(L"bloop_x.wav", NULL, SND_FILENAME | SND_ASYNC);
		}
		Sprite sp;
		sp.dx = MyCos(angle)*100;
		sp.dy = MySin(angle)*100;
		sp.x = *posX;
		sp.y = *posY;
		sp.tex = img1;
		sp.health = 1;
		sp.type = BULLET;
		AddSpriteToList(list, sp);
		KeyMap[0x46] = 1;
	}
	if (KEY_UP(VK_ESCAPE))
	{
		KeyMap[VK_ESCAPE] = 0;
	}
	if (KEY_DOWN(VK_ESCAPE) && !KeyMap[VK_ESCAPE])
	{
		*gs = MENU;
		KeyMap[VK_ESCAPE] = 1;
	}
	if (KEY_UP(VK_RETURN))
	{
		KeyMap[VK_RETURN] = 0;
	}
}

void DrawFrame(int playerA, int playerX, int playerY)
{
    for(int x=0; x<wndWidth; x++)
    {
        int ang = playerA-(x/(double)wndWidth-0.5)*FOV;
        if(ang>=3600) ang%=3600;
        if(ang<0) ang+=3600;
        int rayTop =(ang<1800)?1:0;
        int rayRight = (ang<900 || ang>2700)?1:0;
        float dx, dy;
        float dstX=0, dstY=0;
        float aX, aY, bX, bY;//intersection with grid
        if(ang%900 != 0)
        {
            //for Y axis
            if(rayRight)
            {
                aX = (playerX/64+1) * 64;
                dx = 64;
            }
            else
            {
                aX = (playerX/64)*64 - 1;
                dx = -64;
            }
            if(!rayRight)
                aY = playerY+MyTan(ang)*(aX-playerX+1);
            else
                aY = playerY+MyTan(ang)*(aX-playerX);

            dy=MyTan(ang)*dx;

            while(!DoesRayHit(aX, aY))
            {
                aY+=dy;
                aX+=dx;
            }
            if(!rayRight) aX+=1;
            dstX = fabs((aX-playerX)/MyCos(ang));

            //for X axis
            if(rayTop)
            {
                bY=(playerY/64+1)*64;
                dy=64;
            }
            else
            {
                bY=(playerY/64)*64-1;
                dy=-64;
            }
            if(!rayTop)
                bX=playerX+(bY-playerY+1)/MyTan(ang);
            else
                bX=playerX+(bY-playerY)/MyTan(ang);

            dx=dy/MyTan(ang);

            while(!DoesRayHit(bX, bY))
            {
                bY+=dy;
                bX+=dx;
            }
            if(!rayTop) bY+=1;
            dstY = fabs((bY-playerY)/MySin(ang));
        }
        else    // if ang%900==0
        {
            float sX, sY;
            if(ang==0)
            {
                sX=(playerX/64+1)*64;
                dx=64;
                while(!DoesRayHit(sX, playerY)) sX+=dx;
                dstX=sX-playerX;
            }
            else if(ang==1800)
            {
                sX=(playerX/64)*64-1;
                dx=-64;
                while(!DoesRayHit(sX, playerY)) sX+=dx;
                dstX=playerX-sX-1;
            }
            else if(ang==900)
            {
                sY=(playerY/64+1)*64;
                dy=64;
                while(!DoesRayHit(playerX, sY)) sY+=dy;
                dstY=sY-playerY;
            }
            else if(ang==2700)
            {
                sY=(playerY/64)*64-1;
                dy=-64;
                while(!DoesRayHit(playerX, sY)) sY+=dy;
                dstY=playerY-sY-1;
            }
        }
        int shiftTexture;
        float dst;
        if(dstX==0)
        {
            dst=dstY;
            shiftTexture=playerX%64;
        }
        else if(dstY==0)
        {
            dst=dstX;
            shiftTexture=playerY%64;
        }
        else
        {
            if(dstX>dstY)
            {
                dst=dstY;
                shiftTexture=(int)bX%64;
            }
            else
            {
                dst=dstX;
                shiftTexture=(int)aY%64;
            }
        }
		float intesity = 0.25f;
		if (dst < 500) intesity = 1.f;
		else if (dst < 1000) intesity = 0.75f;
		else if (dst < 1500) intesity = 0.5f;

        dst *= MyCos(fabs(ang-playerA));
		depthBuf[x] = dst;
        int wallHeight = 64*toPlane/dst;
        int wallStart = (wndHeight-wallHeight)>>1;
        if(wallStart<0)
        {
            wallStart = -1;
        }
        int wallStop = wndHeight-wallStart;
		
        for(int i=0; i<wndHeight; i++)
        {
            if(i>wallStart&&i<wallStop)
            {
                int yTex;
                if(wallStart<0)
                {
                    yTex = 64*(i-wallStart-1+(wallHeight-wndHeight)/2)/wallHeight;
                }
                else
                {
					yTex = 64 * (i-wallStart-1) / wallHeight;
                }
				int point = yTex * 3 * 64 + shiftTexture * 3;
				uint8_t *imgMem = mem.imgBits;
                DrawPixel(x, i, imgMem[point+2]*intesity, imgMem[point+1]*intesity, imgMem[point]*intesity);
            }
            else if(i>=wallStop)
            {
				/*
				float front = (toPlane << 5) / (i - wndHeight / 2.f);
				float offset = front * MyTan(playerA - ang);
				int xFloor = playerX + MyCos(playerA)*front + MyCos(playerA - 900)*offset;
				int yFloor = playerY + MySin(playerA)*front + MySin(playerA - 900)*offset;
				xFloor %= 64; yFloor %= 64;
				int pixel = (yFloor * 3 << 6) + xFloor * 3;
				uint8_t *imgMem = mem.imgBits;
				DrawPixel(x, i, imgMem[pixel +2], imgMem[pixel + 1], imgMem[pixel]);*/
                DrawPixel(x, i, 20, 20, 20);
            }
			else
			{
				DrawPixel(x, i, 0, 150, 255);
			}
        }
    }
}

void DrawSprite(int playerX, int playerY, int playerA, MyList *list)
{
	int scount = list->count;
	ListIt *p = list->head;
	float *dist = (float*)malloc(scount * sizeof(float));
	for (int i = 0; i < scount; i++)
	{
		float dx = (p->data->x)- playerX;
		float dy = (p->data->y)- playerY;
		dist[i] = sqrt(dx * dx + dy * dy);
		p = p->next;
	}
	p = list->head;
	for (int i = 0; i < scount-1; i++)
	{
		ListIt *t = p->next;
		for (int j = i+1; j < scount; j++)
		{
			if (dist[i] < dist[j])
			{
				float tmp = dist[i];
				dist[i] = dist[j];
				dist[j] = tmp;
				SwapItems(list, p, t);
			}
			t = t->next;
		}
		p = p->next;
	}
	p = list->head;
	for (int k = 0; k < scount; k++)
	{
		if (dist[k] < 42.f)
		{
			p = p->next;
			continue;
		}
		float sAng = atan2(p->data->y-playerY, p->data->x-playerX) * 1800 / M_PI;
		if (sAng-playerA > 1800) sAng -= 3600;
		else if (sAng-playerA < -1800) sAng += 3600;
		int widthSpr = 64 * toPlane / dist[k];
		int xSpr = ((playerA + FOV / 2 - sAng) / FOV)*wndWidth-widthSpr/2;
		int ySpr = (wndHeight - widthSpr) / 2;
		for (int i = 0; i < widthSpr; i++)
		{
			if (xSpr >= 0 && xSpr < wndWidth && depthBuf[xSpr] < dist[k])
			{
				xSpr++;
				continue;
			}
			for (int j = 0; j < widthSpr; j++)
			{
				int y = (j * 64 /widthSpr);
				int texture = y*64+(i*64)/widthSpr;
				MyImage spoint = p->data->tex;
				uint8_t *tex = spoint.imgBits;
				if (!tex[texture * 3 + 2] && !tex[texture * 3 + 1] && !tex[texture * 3]) 
					continue;
				DrawPixel(xSpr, ySpr + j, tex[texture*3+2], tex[texture*3 + 1], tex[texture*3]);
			}
			xSpr++;
		}
		p = p->next;
	}
	free(dist);
}

void ProcSprites(MyList *list, float timeRatio, int *score, int *health, float playerX, float playerY)
{
	static float shootTime;
	char shooted = 0;
	Sprite player = { 0 };
	player.x = playerX;
	player.y = playerY;

	shootTime += timeRatio;

	ListIt *p = list->head;
	while (p != NULL)
	{
		p->data->x += p->data->dx * timeRatio;
		p->data->y += p->data->dy * timeRatio;
		ListIt *nxt = p->next;
		if (DoesRayHit(p->data->x, p->data->y))
		{
			DelItem(list, p);
			p = nxt;
			continue;
		}
		else if (p->isDead == 1)
		{
			if (p->data->type == ENEMY) (*score)++;
			DelItem(list, p);
			p = nxt;
			continue;
		}
		else if (p->data->type == ENBULLET)
		{
			if (GetDist(&player, p->data) <= 25.f)
			{
				PlaySound(L"blip.wav", NULL, SND_FILENAME | SND_ASYNC);
				(*health) -= 10;
				DelItem(list, p);
				p = nxt;
				continue;
			}
		}
		if (shootTime > 50.f && p->data->type == ENEMY)
		{
			Sprite bul = { 0 };
			bul.x = p->data->x;
			bul.y = p->data->y;
			bul.health = 1;
			bul.tex = enbImg;
			bul.dx = 20*(playerX - bul.x)/GetDist(&player, p->data);
			bul.dy = 20*(playerY - bul.y)/GetDist(&player, p->data);
			bul.type = ENBULLET;
			AddSpriteToList(list, bul);
			shooted = 1;
		}
		p = nxt;
	}
	if (shooted)
	{
		shootTime = 0;
	}
	p = list->head;
	if (p == NULL)return;
	while (p->next != NULL)
	{
		float r1 = 0.f;
		switch (p->data->type)
		{
		case BULLET:
			r1 = 5.f;
			break;
		case ENEMY:
			r1 = 30.f;
			break;
		default:
			p = p->next;
			continue;
		}
		ListIt *j = p->next;
		while (j != NULL)
		{
			float r2 = 0.f;
			switch (j->data->type)
			{
			case BULLET:
				r2 = 5.f;
				break;
			case ENEMY:
				r2 = 30.f;
				break;
			default:
				j = j->next;
				continue;
			}
			if (r1 + r2 >= GetDist(j->data, p->data) && !(p->data->type==ENEMY && j->data->type==ENEMY))
			{
				j->data->health -= 1;
				if (j->data->health < 1) j->isDead = 1;
				p->data->health -= 1;
				if (p->data->health < 1) p->isDead = 1;
			}
			j = j->next;
		}
		p = p->next;
	}
}

float GetTimeRatio(int reset)
{
	static uint32_t oldTime;

	uint32_t curTime = GetTickCount();
	uint32_t deltaTime = 0;
	if (reset)
	{
		deltaTime = 10;
	}
	else
	{
		deltaTime = curTime - oldTime;
	}
	oldTime = curTime;
	float timeRatio = deltaTime / 100.f;
	return timeRatio;
}

float GetDist(const Sprite *s1, const Sprite *s2)
{
	float x1 = s1->x;
	float x2 = s2->x;
	float y1 = s1->y;
	float y2 = s2->y;
	float dst = sqrtf((x1-x2) * (x1-x2) + (y1-y2) * (y1-y2));
	return dst;
}

void SpawnEnemy(MyList *list, float timeRatio, int reset)
{
	static float time;
	if (reset) time = 0.f;
	time += timeRatio;
	if (list->count > 20 || time < 10.f)
		return;
	time = 0.f;
	int blocks = mapHeight * mapWidth;
	char spawned = 0;
	int i = 0;
	while (!spawned)
	{
		i = 0;
		int r = rand() % blocks;
		Sprite sp = { 0 };
		while (r && i < blocks)
		{
			if (map[i] == '.')
			{
				r--;
			}
			i++;
		}
		if (i != blocks - 1)
		{
			spawned = 1;
		}
	}
	Sprite sp = { 0 };
	sp.y = (i / mapWidth)*64 + 32;
	sp.x = (i % mapWidth)*64 + 32;
	sp.health = 3;
	sp.tex = enemyImg;
	sp.type = ENEMY;
	AddSpriteToList(list, sp);
}