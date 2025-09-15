#include <windows.h>

#include <icons/pau203a.xpm>
#include <icons/bet203a.xpm>
#include <icons/har203a.xpm>
#include <icons/fra203a.xpm>
#include <icons/den203a.xpm>
#include <icons/kid203a.xpm>
#include <icons/urs203a.xpm>
#include <icons/rit203a.xpm>
#include <icons/wen203a.xpm>

#include <epsonapi.h>

static HINSTANCE hInst;

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	return DefWindowProc(hWnd, msg, wp, lp);
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
	HWND hWnd = CreateWindow("dectalk", "Speak", (WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME) ^ WS_MAXIMIZEBOX, CW_USEDEFAULT, CW_USEDEFAULT, 660, 440, NULL, 0, hInst, NULL);

	if(!hWnd) {
		return FALSE;
	}
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	return TRUE;
}


int WINAPI WinMain(HINSTANCE hCurInst, HINSTANCE hPrevInst, LPSTR lpsCmdLine, int nCmdShow){
	BOOL bret;
	MSG msg;

	hInst = hCurInst;
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
