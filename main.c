#include <tchar.h>
#include <windows.h>
#include <stdint.h>
#include "loop.h"
#include "main.h"

LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
void СreateMemory();

const TCHAR *szClassName = _T("Game");
int wndHeight = 480; int wndWidth = 640;
int desHeight, desWidth;
void *screen = NULL; BITMAPINFO bmInfo;
int USERINPUT = NOINP;
wchar_t FileName[16];
uint8_t KeyMap[256];
HWND hwnd;
extern GameState STATE;
extern int score;
extern PlStat statArr[10];


int WINAPI WinMain (HINSTANCE hThisInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR lpszArgument,
                     int nCmdShow)
{
    MSG messages;
    WNDCLASSEX wincl;
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;
    wincl.style = CS_VREDRAW|CS_HREDRAW;
    wincl.cbSize = sizeof (WNDCLASSEX);
    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;
    wincl.cbClsExtra = 0;
    wincl.cbWndExtra = 0;
    wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND;

    if (!RegisterClassEx (&wincl))
        return 0;
	
    RECT wndRect = {.right=wndWidth, .bottom=wndHeight};
    AdjustWindowRect(&wndRect, WS_SYSMENU | WS_CAPTION, FALSE);
    int width = wndRect.right-wndRect.left;
    int height = wndRect.bottom-wndRect.top;
	desWidth = wndWidth;//GetSystemMetrics(SM_CXSCREEN);
	desHeight = wndHeight;//GetSystemMetrics(SM_CYSCREEN);
	//wndHeight = desHeight*wndWidth / desWidth;

    hwnd = CreateWindowEx (
           0,//WS_EX_TOPMOST,
           szClassName,
           _T("Wolfenstein"),
		   WS_SYSMENU|WS_CAPTION,//WS_POPUP,
           //WS_OVERLAPPEDWINDOW,
           CW_USEDEFAULT,
           CW_USEDEFAULT,
           desWidth,                 /* The programs width */
           desHeight,                 /* and height in pixels */
           NULL,
           NULL,
           hThisInstance,
           NULL
           );
	СreateMemory();
    GameLaunch();
    ShowWindow (hwnd, nCmdShow);
    UpdateWindow(hwnd);
    while (1)
    {
        if(PeekMessage(&messages, NULL, 0, 0, PM_REMOVE))
        {
            if(messages.message==WM_QUIT)
            {
                break;
            }
            TranslateMessage(&messages);
            DispatchMessage(&messages);
        }
        GameMain();
		InvalidateRect(hwnd, NULL, TRUE);
		UpdateWindow(hwnd);
    }
    GameShutdown();
	free(screen);
    return messages.wParam;
}

LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    PAINTSTRUCT ps;
	static int fileLen = 0;
    switch (message)
    {
        case WM_PAINT:
		{
			hdc = BeginPaint(hwnd, &ps);
			HBITMAP hBitD = CreateCompatibleBitmap(hdc, desWidth, desHeight);
			HDC hDob = CreateCompatibleDC(hdc);
			HBITMAP oldBit = SelectObject(hDob, hBitD);
			StretchDIBits(hDob, 0, 0, desWidth, desHeight, 0, 0, wndWidth, wndHeight,
				screen, &bmInfo, DIB_RGB_COLORS, SRCCOPY);
			if(USERINPUT == WAIT)
			{
				RECT rect;
				GetClientRect(hwnd, &rect);
				SetTextColor(hDob, NULL);
				HFONT hFont = CreateFont(50, 0, 0, 0, 0, 0, 0, 0,
					DEFAULT_CHARSET, 0, 0, 0, VARIABLE_PITCH, L"Arial Bold");
				HFONT original = SelectObject(hDob, hFont);
				SetBkMode(hDob, TRANSPARENT);
				switch (STATE) {
					case DEATH:
					{
						SetTextColor(hDob, RGB(255, 255, 255));
						wchar_t tbuf[16];
						swprintf(tbuf, 16, L"Ваш счет:%d", score);
						DrawText(hDob, (LPCWSTR)tbuf, wcslen(tbuf), &rect, DT_SINGLELINE | DT_CENTER | DT_TOP);
						TextOut(hDob, 10, wndHeight / 3, L"Ваше имя:", 9);
						DrawText(hDob, (LPCWSTR)FileName, fileLen, &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
					}break;
					case SAVEGAME:
					case LOADGAME:
					{
						DrawText(hDob, (LPCWSTR)FileName, fileLen, &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
					}break;
					case SHOWST:
					{
						int y = 0;
						int dy = desHeight / 10;
						int i = 0;
						wchar_t str[40] = { 0 };
						while (statArr[i].score > 0)
						{
							swprintf(str, 40, L"%s: %d", statArr[i].name, statArr[i].score);
							TextOutW(hDob, 0, y, str, wcslen(str));
							y += dy;
							i++;
						}
					}break;
					case GENMAP:
					{
						DrawText(hDob, L"Карта сгенерирована!", 20, &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
					}break;
				}
				SelectObject(hdc, original);
				DeleteObject(hFont);
			}
			BitBlt(hdc, 0, 0, desWidth, desHeight, hDob, 0, 0, SRCCOPY);
			SelectObject(hDob, oldBit);
			DeleteObject(hBitD);
			DeleteDC(hDob);
			EndPaint(hwnd, &ps);
			return 0;
		}
		case WM_CLOSE:
		{
			int res = MessageBox(hwnd, L"Вы действительно желаете выйти?", L"Подтверждение", 
				MB_YESNO | MB_ICONQUESTION);
			if (res == IDYES)
			{
				DestroyWindow(hwnd);
			}
			return 0;
		}
        case WM_DESTROY:
            PostQuitMessage (0);
			return 0;
		case WM_ERASEBKGND:
			return 1;
		case WM_KEYUP:
			if (USERINPUT != WAIT)
				break;
			switch (wParam)
			{
			case VK_ESCAPE:
				KeyMap[VK_ESCAPE] = 0;
				return 0;
			case VK_RETURN:
				KeyMap[VK_RETURN] = 0;
				return 0;
			default:
				break;
			}
			break;
		case WM_CHAR:
			if (USERINPUT != WAIT)
			{
				break;
			}
			switch (wParam)
			{
			case 0x0A:
			case 0x09:
				break;
			case VK_BACK:
				if(fileLen)
					fileLen--;
				return 0;
			case VK_ESCAPE:
				if (!KeyMap[VK_ESCAPE])
				{
					fileLen = 0;
					FileName[0] = 0;
					USERINPUT = READY;
					KeyMap[VK_ESCAPE] = 1;
				}
				return 0;
			case VK_RETURN:
				if (!KeyMap[VK_RETURN])
				{
					FileName[fileLen] = 0;
					fileLen = 0;
					USERINPUT = READY;
					KeyMap[VK_RETURN] = 1;
				}
				return 0;
			default:
				if (STATE != GENMAP && fileLen<15) {
					FileName[fileLen] = (wchar_t)wParam;
					fileLen++;
				}
				return 0;
			}
        default:
			break;
    }
	return DefWindowProc(hwnd, message, wParam, lParam);
}

void СreateMemory()
{
    screen = calloc(wndHeight*wndWidth, sizeof(uint32_t));
    bmInfo.bmiHeader.biSize = sizeof(bmInfo.bmiHeader);
    bmInfo.bmiHeader.biWidth = wndWidth;
    bmInfo.bmiHeader.biHeight = -wndHeight;
    bmInfo.bmiHeader.biPlanes = 1;
    bmInfo.bmiHeader.biBitCount = 32;
    bmInfo.bmiHeader.biCompression = BI_RGB;

    if(!screen)
    {
        MessageBox(NULL, _T("Memory Allocation Error!"), _T("Memory Error"), MB_ICONERROR|MB_OK);
    }
}
