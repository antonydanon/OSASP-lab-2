#include <Windows.h>

RECT workSpace = { 0, 0, 1440, 900 };

float windowHeight;
float windowWidth;

float columnWidth;
float rowHeight;

const int columnNumber = 5;
const int rowNumber = 3;

const CHAR* str = "Hello!";

void drawLine(HDC memDc, float startX, float startY, float finishX, float finishY) {
    MoveToEx(memDc, startX, startY, NULL);
    LineTo(memDc, finishX, finishY);
}

void drawColumnLines(HDC memDc, float startX, float startY) {
    for (int j = 0; j <= columnNumber; j++)
    {
        drawLine(memDc, startX, startY, startX, windowHeight);
        startX += columnWidth;
    }
}

void drawTopHorizontalLine(HDC memDc, float startX, float startY) {
    drawLine(memDc, startX, startY, windowWidth, startY);
}

void checkLastColumn(HDC memDc, int& columnCounter, float& startX, float& startY) {
    if (columnCounter == columnNumber)
    {
        startY += rowHeight;
        drawLine(memDc, 0, startY, windowWidth, startY);
        columnCounter = 0;
        startX = 0;
    }
}

int drawItem(HDC memDc, float startX, float startY) {
    RECT textRect;
    SetRect(&textRect, startX, startY + rowHeight/2.5, startX + columnWidth, startY + rowHeight);
    int textHeight = DrawText(memDc, (LPCSTR)str, strlen(str), &textRect, DT_CENTER | DT_EDITCONTROL | DT_WORDBREAK | DT_NOCLIP);
    return textHeight;
}

void checkTextHeight(float textHeight) {
    if (textHeight > rowHeight)
    {
        rowHeight = textHeight;
    }
}

void drawItemsAndVerticalLines(HDC memDc, float startX, float startY) {
    int columnCounter = 0;
    int i = 0;

    while (i < columnNumber * rowNumber)
    {
        checkLastColumn(memDc, columnCounter, startX, startY);
        int textHeight = drawItem(memDc, startX, startY);
        checkTextHeight(textHeight);

        columnCounter++;
        startX += columnWidth;
        i++;
    }
}

void drawTable(HWND hwnd)
{
    PAINTSTRUCT ps;
    HDC hdc;
    HDC memDc;
    HBITMAP memBmp;
    HBRUSH hbrBkgnd;

    GetClientRect(hwnd, &workSpace);
    windowHeight = workSpace.bottom;
    windowWidth = workSpace.right;

    hdc = BeginPaint(hwnd, &ps);
    memDc = CreateCompatibleDC(hdc);
    memBmp = CreateCompatibleBitmap(hdc, windowWidth, windowHeight);
    HBITMAP oldBmp = (HBITMAP)SelectObject(memDc, memBmp);
    hbrBkgnd = CreateSolidBrush(RGB(67, 215, 25));
    FillRect(memDc, &workSpace, hbrBkgnd);
    DeleteObject(hbrBkgnd);
    SetTextColor(memDc, RGB(215, 205, 234));
    SetBkMode(memDc, TRANSPARENT);

    int startX = 0, startY = 0;

    drawColumnLines(memDc, startX, startY);
    drawTopHorizontalLine(memDc, startX, startY);
    drawItemsAndVerticalLines(memDc,startX, startY);

    BitBlt(hdc, 0, 0, windowWidth, windowHeight, memDc, 0, 0, SRCCOPY);
    SelectObject(hdc, oldBmp);
    DeleteObject(memBmp);
    DeleteDC(memDc);
    EndPaint(hwnd, &ps);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
        case WM_DESTROY: {
            PostQuitMessage(0);
            break;
        }
        case WM_SIZE: {
            GetClientRect(hwnd, &workSpace);
            windowHeight = workSpace.bottom;
            windowWidth = workSpace.right;
            columnWidth = windowWidth / columnNumber;
            rowHeight = windowHeight / rowNumber;
            break;
        }
        case WM_ERASEBKGND: {
            return (LRESULT)1;
        }
        case WM_PAINT: {
            drawTable(hwnd);
            break;
        }
        default: return DefWindowProc(hwnd, message, wParam, lParam);
    }
    return 0;
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, LPSTR cmdLine, int nCmdShow)
{
	WNDCLASSEX windowClass;
	HWND winHandle;
	MSG message;

	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_DBLCLKS;
	windowClass.lpfnWndProc = WndProc;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = hInstance;
	windowClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	windowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	windowClass.lpszMenuName = NULL;
	windowClass.lpszClassName = "MainWindow";
	windowClass.hIconSm = windowClass.hIcon;

	RegisterClassEx(&windowClass);

	winHandle = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, "MainWindow", "Main Window", WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX,
		100, 100, (workSpace.right - workSpace.left) / 1.5, (workSpace.bottom - workSpace.top) / 1.5, NULL, NULL, hInstance, 0);

	ShowWindow(winHandle, nCmdShow);
	UpdateWindow(winHandle);

	while (GetMessage(&message, NULL, 0, 0))
	{
		TranslateMessage(&message);
		DispatchMessage(&message);

		InvalidateRect(winHandle, NULL, FALSE);
	}

	return (int)message.wParam;
}
