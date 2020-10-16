#define RECT_ID 1
#define IMG_ID 2
#define IDB_BITMAP1 101

#include <windows.h>
#include <tchar.h>
#include <corecrt_math.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
TCHAR WinName[] = _T("MainFrame");

int APIENTRY _tWinMain(HINSTANCE This, HINSTANCE Prev, LPTSTR cmd, int mode) {
	HWND hWnd;
	MSG msg;
	WNDCLASS wc;
	wc.hInstance = This;
	wc.lpszClassName = WinName;
	wc.lpfnWndProc = WndProc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.lpszMenuName = NULL;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	if (!RegisterClass(&wc)) return 0;
	hWnd = CreateWindow(WinName, _T("Лабораторная работа 1"), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
		500, 500, HWND_DESKTOP, NULL, This, NULL);
	ShowWindow(hWnd, mode);
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}

void drawImage(HDC hdc, int x, int y, HBITMAP hBitmap) {
	HBITMAP hNewBitmap;

	HDC compDc;
	BITMAP bitmap;
	int bitmapWidth, bitmapHeight;

	compDc = CreateCompatibleDC(hdc);

	hNewBitmap = (HBITMAP)SelectObject(compDc, hBitmap);

	if (hNewBitmap) {
		SetMapMode(compDc, GetMapMode(hdc));
		GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&bitmap);

		bitmapWidth = bitmap.bmWidth;
		bitmapHeight = bitmap.bmHeight;

		POINT bmSize;
		bmSize.x = bitmapWidth;
		bmSize.y = bitmapHeight;

		DPtoLP(hdc, &bmSize, 1);

		POINT point;
		point.x = 0;
		point.y = 0;

		DPtoLP(compDc, &point, 1);
		BitBlt(hdc, x, y, bitmapWidth, bitmapHeight, compDc, point.x, point.y, SRCCOPY);
		SelectObject(compDc, hNewBitmap);
	}

	DeleteDC(compDc);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	static int id = RECT_ID;
	PAINTSTRUCT ps;
	HDC hdc, memDC;
	RECT clientRect;
	static HBRUSH hbrush;
	static HBITMAP myBmp;
	static int x = 200;
	static int y = 200;
	static int size = 100;
	static int vx, vy, alpha;
	static int moveStatus = 0;
	int mouseOffset;

	switch (message) {
	case WM_CREATE: {
		hbrush = CreateSolidBrush(RGB(0, 0, 0));
		myBmp = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP1));
		break;
	}
	case WM_DESTROY: {
		if (moveStatus) {
			KillTimer(hWnd, 1);
		}
		DeleteObject(hbrush);
		DeleteObject(myBmp);
		PostQuitMessage(0);
		break;
	}
	case WM_RBUTTONDOWN: {
		if (id == RECT_ID)
			id = IMG_ID;
		else
			id = RECT_ID;
		break;
	}
	case WM_LBUTTONDOWN: {
		if (!moveStatus) {
			x = LOWORD(lParam);
			y = HIWORD(lParam);
			alpha = round((rand() % 360 - 180) * 3.14 / 180.0);
			vx = 10.0 * cos(alpha);
			vy = 10.0 * sin(alpha);
			SetTimer(hWnd, 1, 25, NULL);
			moveStatus = 1;
		}
		else {
			KillTimer(hWnd, 1);
			moveStatus = 0;
		}
		break;
	}
	case WM_TIMER:
	{
		x += vx;
		y += vy;
		GetClientRect(hWnd, &clientRect);
		if ((x + size) >= clientRect.right)
			vx = -abs(vx);
		if (x < clientRect.left)
			vx = abs(vx);
		if ((y + size) > clientRect.bottom)
			vy = -abs(vy);
		if (y < clientRect.top)
			vy = abs(vy);
		x--;
		y--;
		InvalidateRect(hWnd, NULL, TRUE);
		break;
	}
	case WM_PAINT: {
		if (id == RECT_ID) {
			hdc = BeginPaint(hWnd, &ps);
			SelectObject(hdc, hbrush);
			Rectangle(hdc, x, y, x + size, y + size);
			EndPaint(hWnd, &ps);
		}
		else {
			hdc = BeginPaint(hWnd, &ps);
			drawImage(hdc, x, y, myBmp);
			EndPaint(hWnd, &ps);
		}
		break;
	}
	case WM_KEYDOWN:
	{
		if (!moveStatus) {
			GetClientRect(hWnd, &clientRect);
			switch (wParam) {
			case VK_LEFT: {
				x -= 10;
				if (x < clientRect.left)
					x += 20;
				break;
			}
			case VK_RIGHT: {
				x += 10;
				if ((x + size) > clientRect.right)
					x -= 20;
				break;
			}
			case VK_UP: {
				y -= 10;
				if (y < clientRect.top)
					y += 20;
				break;
			}
			case VK_DOWN: {
				y += 10;
				if ((y + size) > clientRect.bottom)
					y -= 20;
				break;
			}
			}
			InvalidateRect(hWnd, NULL, TRUE);
		}
		break;
	}
	case WM_MOUSEWHEEL: {
		if (!moveStatus) {
			GetClientRect(hWnd, &clientRect);
			mouseOffset = GET_WHEEL_DELTA_WPARAM(wParam);
			if (LOWORD(wParam) == MK_SHIFT) {
				if (mouseOffset > 0) {
					x += 10;
					if ((x + size) > clientRect.right)
						x -= 20;
				}
				else {
					x -= 10;
					if (x < clientRect.left)
						x += 20;
				}
			}
			else {
				if (mouseOffset > 0) {
					y += 10;
					if ((y + size) > clientRect.bottom)
						y -= 20;
				}
				else {
					y -= 10;
					if (y < clientRect.top)
						y += 20;
				}
			}
			InvalidateRect(hWnd, NULL, TRUE);
		}
		break;
	}
	case WM_MOUSEMOVE: {
		if (!moveStatus) {
			int oldX = x;
			int oldY = y;
			GetClientRect(hWnd, &clientRect);
			x = LOWORD(lParam);
			y = HIWORD(lParam);
			if (x < clientRect.left)
				x = oldX;
			if ((x + size) > clientRect.right)
				x = oldX;
			if (y < clientRect.top)
				y = oldY;
			if ((y + size) > clientRect.bottom)
				y = oldY;
			InvalidateRect(hWnd, NULL, TRUE);
		}
		break;
	}
	default: return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}