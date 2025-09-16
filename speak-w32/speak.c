#include <windows.h>

#include <stb_ds.h>
#include <miniaudio.h>

#include <epsonapi.h>

HINSTANCE hInst;
HWND* btns = NULL;
HWND text, start, stop;
HBRUSH person_brush, black_brush;
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
	}else if(msg == WM_SIZE){
		RECT rc;
		int i;
		int padleft;

		GetClientRect(hWnd, &rc);

		padleft = ((rc.right - rc.left) - (BTNSZ * 9)) / 2;

		for(i = 0; i < arrlen(btns); i++){
			SetWindowPos(btns[i], NULL, padleft + i * BTNSZ, 0, 0, 0, SWP_NOSIZE);
		}

		SetWindowPos(start, NULL, rc.right - 32 - 32 - 8, rc.bottom - 48 + 16 / 2, 0, 0, SWP_NOSIZE);
		SetWindowPos(stop, NULL, rc.right - 32 - 8, rc.bottom - 48 + 16 / 2, 0, 0, SWP_NOSIZE);

		SetWindowPos(text, NULL, 0, BTNSZ, rc.right - rc.left, rc.bottom - rc.top - BTNSZ - 48, 0);
	}else if(msg == WM_DRAWITEM){
		int i;
		LPDRAWITEMSTRUCT dis = (LPDRAWITEMSTRUCT)lp;

		if(dis->hwndItem == start || dis->hwndItem == stop){
			HDC dc = dis->hDC;
			RECT rc, brc;
			UINT fl = 0;

			GetClientRect(dis->hwndItem, &rc);

			brc.left = 0;
			brc.top = 0;
			brc.right = rc.right - rc.left;
			brc.bottom = rc.bottom - rc.top;

			fl = (dis->itemState & ODS_SELECTED) ? DFCS_PUSHED : 0;
			DrawFrameControl(dc, &brc, DFC_BUTTON, DFCS_BUTTONPUSH | DFCS_ADJUSTRECT | fl);

			if(dis->hwndItem == start){
				POINT pts[3];

				SelectObject(dc, black_brush);

				pts[0].x = brc.left + 3;
				pts[0].y = brc.top + 3;

				pts[1].x = brc.right - 3;
				pts[1].y = (brc.bottom - brc.top) / 2;

				pts[2].x = brc.left + 3;
				pts[2].y = brc.bottom - 3;
				
				Polygon(dc, pts, 3);
			}else{
				POINT pts[4];

				SelectObject(dc, black_brush);

				pts[0].x = brc.left + 3;
				pts[0].y = brc.top + 3;

				pts[1].x = brc.right - 3;
				pts[1].y = brc.top + 3;

				pts[2].x = brc.right - 3;
				pts[2].y = brc.bottom - 3;

				pts[3].x = brc.left + 3;
				pts[3].y = brc.bottom - 3;
				
				Polygon(dc, pts, 4);
			}

			return 0;
		}

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
	HWND hWnd = CreateWindow("dectalk", "Speak", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 660, 440, NULL, 0, hInst, NULL);
	int i;

	if(!hWnd) {
		return FALSE;
	}

	for(i = 0; i < 9; i++){
		HWND hBtn = CreateWindow("BUTTON", "", WS_VISIBLE | WS_CHILD | BS_OWNERDRAW, 0, 0, BTNSZ, BTNSZ, hWnd, (HMENU)(LONG_PTR)(i + 100), hInst, NULL);
		arrput(btns, hBtn);
	}

	start = CreateWindow("BUTTON", "", WS_VISIBLE | WS_CHILD | BS_OWNERDRAW, 0, 0, 32, 32, hWnd, (HMENU)200, hInst, NULL);
	stop = CreateWindow("BUTTON", "", WS_VISIBLE | WS_CHILD | BS_OWNERDRAW, 0, 0, 32, 32, hWnd, (HMENU)201, hInst, NULL);

	text = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | WS_VSCROLL | WS_HSCROLL | ES_MULTILINE, 0, 0, 0, 0, hWnd, 0, hInst, NULL);

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
	black_brush = CreateSolidBrush(RGB(0, 0, 0));

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
