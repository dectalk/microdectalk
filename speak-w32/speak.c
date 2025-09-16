#include <windows.h>

#include <stb_ds.h>
#include <miniaudio.h>

#include <epsonapi.h>

HINSTANCE hInst;
HWND* btns = NULL;
HBRUSH person_brush;
COLORREF person_color;

const char* people[] = {
	"PAUL",
	"BETTY",
	"HARRY",
	"FRANK",
	"DENNIS",
	"KIT",
	"URSULA",
	"RITA",
	"WENDY"
};

#define BTNSZ 70

void ShowPerson(HDC hdc, const char* name, RECT* rc){
	HBITMAP hBitmap = LoadBitmap(hInst, name);
	BITMAP bmp;
	HDC hmdc;

	GetObject(hBitmap, sizeof(bmp), &bmp);
	hmdc = CreateCompatibleDC(hdc);
	SelectObject(hmdc, hBitmap);

	StretchBlt(hdc, rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top, hmdc, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);

	DeleteDC(hmdc);
	DeleteObject(hBitmap);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	if(msg == WM_CLOSE){
		DestroyWindow(hWnd);
	}else if(msg == WM_DESTROY){
		PostQuitMessage(0);
	}else if(msg == WM_DRAWITEM){
		int i;
		LPDRAWITEMSTRUCT dis = (LPDRAWITEMSTRUCT)lp;
		for(i = 0; i < arrlen(btns); i++){
			if(btns[i] == dis->hwndItem){
				HDC dc = dis->hDC;
				RECT rc, brc;
				UINT fl = 0;

				GetClientRect(dis->hwndItem, &rc);

				brc.left = 0;
				brc.top = 0;
				brc.right = rc.right - rc.left;
				brc.bottom = rc.bottom - rc.top;

				SetBkColor(dc, person_color);
				FillRect(dc, &brc, person_brush);

				fl = (dis->itemState & ODS_SELECTED) ? DFCS_PUSHED : 0;
				DrawFrameControl(dc, &brc, DFC_BUTTON, DFCS_BUTTONPUSH | DFCS_ADJUSTRECT | DFCS_TRANSPARENT | fl);

				ShowPerson(dc, people[i], &brc);

				return 0;
			}
		}
		return DefWindowProc(hWnd, msg, wp, lp);
	}else{
		return DefWindowProc(hWnd, msg, wp, lp);
	}
	return 0;
}

BOOL InitApp(void) {
	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInst;
	wc.hIcon = LoadIcon(hInst, "DTC");
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = GetSysColorBrush(COLOR_MENU);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = "dectalk";
	wc.hIconSm = LoadIcon(hInst, "DTC");
	return RegisterClassEx(&wc);
}

BOOL InitWindow(int nCmdShow) {
	RECT rc;
	int padleft;
	HWND hWnd = CreateWindow("dectalk", "Speak", (WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME) ^ WS_MAXIMIZEBOX, CW_USEDEFAULT, CW_USEDEFAULT, 660, 440, NULL, 0, hInst, NULL);
	int i;

	if(!hWnd) {
		return FALSE;
	}

	GetClientRect(hWnd, &rc);

	padleft = ((rc.right - rc.left) - BTNSZ * 9) / 2;

	for(i = 0; i < 9; i++){
		HWND hBtn = CreateWindow("BUTTON", "", WS_VISIBLE | WS_CHILD | BS_OWNERDRAW, padleft + BTNSZ * i, 0, BTNSZ, BTNSZ, hWnd, 0, hInst, NULL);
		arrput(btns, hBtn);
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	return TRUE;
}


int WINAPI WinMain(HINSTANCE hCurInst, HINSTANCE hPrevInst, LPSTR lpsCmdLine, int nCmdShow){
	BOOL bret;
	MSG msg;

	hInst = hCurInst;

	person_color = RGB(0xb3, 0x35, 0x3f);
	person_brush = CreateSolidBrush(person_color);

	if(!InitApp()) return 0;
	if(!InitWindow(nCmdShow)) return 0;

	while((bret = GetMessage(&msg, NULL, 0, 0)) != 0) {
		if(bret == -1) {
			break;
		} else {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}
