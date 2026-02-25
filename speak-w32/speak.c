#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <windows.h>
#include <commctrl.h>

#include <stb_ds.h>
#include <miniaudio.h>

#include <epsonapi.h>

HINSTANCE hInst;
HWND* btns = NULL;
HWND text, start, stop, ratebar;
HBRUSH person_brush, black_brush;
COLORREF person_color;
ma_device_config config;
ma_device device;
ma_mutex mutex;

typedef struct buffer {
	short* data;
	unsigned int length;
	unsigned int seek;
} buffer_t;

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

buffer_t* buffers = NULL;
unsigned int wavelen;
short* wave = NULL;
int rate = 200;

short* write_wav(short* iwave, long length, int phoneme){
	if(wave == NULL){
		wave = malloc(length * 2);
		memcpy(wave, iwave, length * 2);
	}else{
		short* old = wave;
		wave = malloc((wavelen + length) * 2);
		memcpy(wave, old, wavelen * 2);
		memcpy(wave + wavelen, iwave, length * 2);
		free(old);
	}

	wavelen += length;

	return NULL;
}

void data_callback(ma_device* dev, void* out, const void* in, ma_uint32 frame){
	unsigned int seek = 0;
	memset(out, 0, frame * 2);
	ma_mutex_lock(&mutex);
	while(frame > 0 && arrlen(buffers) > 0){
		unsigned int bsz = buffers[0].length - buffers[0].seek;
		unsigned int sz = bsz > frame ? frame : bsz;

		memcpy(((short*)out) + seek, buffers[0].data + buffers[0].seek, sz * 2);

		buffers[0].seek += sz;
		frame -= sz;
		seek += sz;

		if((buffers[0].length - buffers[0].seek) == 0){
			free(buffers[0].data);
			arrdel(buffers, 0);
		}
	}
	ma_mutex_unlock(&mutex);
}

void speak(const char* str){
	buffer_t buf;
	char ratebuf[32];
	char* sbuf;

	sprintf(ratebuf, "[:rate %d]", rate);
	sbuf = malloc(strlen(ratebuf) + strlen(str) + 1);

	strcpy(sbuf, ratebuf);
	strcat(sbuf, str);

	if(wave != NULL){
		free(wave);
		wave = NULL;
	}
	wavelen = 0;

	TextToSpeechStart(sbuf, NULL, WAVE_FORMAT_1M16);
	TextToSpeechSync();

	buf.data = malloc(wavelen * 2);
	memcpy(buf.data, wave, wavelen * 2);
	buf.length = wavelen;
	buf.seek = 0;

	ma_mutex_lock(&mutex);
	arrput(buffers, buf);
	ma_mutex_unlock(&mutex);

	free(sbuf);
}

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
	}else if(msg == WM_COMMAND){
		int m = LOWORD(wp);
		if(100 <= m && m < 109){
			char buf[128];

			sprintf(buf, "[:name %s]%s", people[m - 100], people[m - 100]);

			speak(buf);
		}else if(m == 200){
			int len = GetWindowTextLength(text);
			char* buf = malloc(len + 2);
			GetWindowText(text, buf, len + 1);

			buf[len] = 0;

			speak(buf);

			free(buf);
		}else if(m == 201){
			ma_mutex_lock(&mutex);
			if(arrlen(buffers) > 0){
				while(arrlen(buffers) > 0){
					free(buffers[0].data);
					arrdel(buffers, 0);
				}
			}
			ma_mutex_unlock(&mutex);
		}
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
		SetWindowPos(ratebar, NULL, 48 * 2, rc.bottom - 48 / 2 - 16 / 2, 0, 0, SWP_NOSIZE);

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
	}else if(msg == WM_PAINT){
		PAINTSTRUCT ps;
		HDC dc = BeginPaint(hWnd, &ps);
		RECT rc, frc;
		char buf[32];

		GetClientRect(hWnd, &rc);

		rc.right = rc.right - rc.left;
		rc.bottom = rc.bottom - rc.top;
		rc.left = 0;
		rc.top = 0;

		FillRect(dc, &rc, GetSysColorBrush(COLOR_MENU));

		SetTextAlign(dc, TA_CENTER | TA_TOP);
		SetBkMode(dc, TRANSPARENT);
		TextOut(dc, 48, rc.bottom - 48 / 2 - 16 / 2, "Speaking Rate", 13);

		frc.left = 48 * 2 + 128 + 32;
		frc.top = rc.bottom - 48 / 2 - 16 / 2;
		frc.right = frc.left + 48 * 2;
		frc.bottom = frc.top + 16;

		sprintf(buf, "%d WPM", rate);
		FrameRect(dc, &frc, black_brush);
		TextOut(dc, 48 * 2 + 128 + 32 + 48, rc.bottom - 48 / 2 - 16 / 2, buf, strlen(buf));

		EndPaint(hWnd, &ps);
	}else if(msg == WM_HSCROLL){
		if(ratebar == (HWND)lp){
			rate = SendMessage(ratebar, TBM_GETPOS, 0, 0);
			InvalidateRect(hWnd, NULL, FALSE);
		}
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

	InitCommonControls();

	start = CreateWindow("BUTTON", "", WS_VISIBLE | WS_CHILD | BS_OWNERDRAW, 0, 0, 32, 32, hWnd, (HMENU)200, hInst, NULL);
	stop = CreateWindow("BUTTON", "", WS_VISIBLE | WS_CHILD | BS_OWNERDRAW, 0, 0, 32, 32, hWnd, (HMENU)201, hInst, NULL);
	ratebar = CreateWindow(TRACKBAR_CLASS, "", WS_VISIBLE | WS_CHILD, 0, 0, 128 + 32, 16, hWnd, 0, hInst, NULL);
	SendMessage(ratebar, TBM_SETRANGE, TRUE, MAKELPARAM(75, 600));
	SendMessage(ratebar, TBM_SETPOS, TRUE, 200);

	text = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | WS_VSCROLL | WS_HSCROLL | ES_MULTILINE, 0, 0, 0, 0, hWnd, 0, hInst, NULL);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	return TRUE;
}


int WINAPI WinMain(HINSTANCE hCurInst, HINSTANCE hPrevInst, LPSTR lpsCmdLine, int nCmdShow){
	BOOL bret;
	MSG msg;

	config = ma_device_config_init(ma_device_type_playback);
	config.playback.format = ma_format_s16;
	config.playback.channels = 1;
	config.sampleRate = 11025;
	config.dataCallback = data_callback;

	ma_mutex_init(&mutex);

	if(ma_device_init(NULL, &config, &device) != MA_SUCCESS || ma_device_start(&device) != MA_SUCCESS){
		return 0;
	}

	TextToSpeechInit(write_wav, NULL);

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
