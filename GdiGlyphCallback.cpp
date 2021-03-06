// GdiGlyphCallback.cpp : Defines the entry point for the application.
//

#include <windows.h>
#include "IGlyphCallback.h"
#include "RegBigFontShxParser.h"
#include <vector>

class GdiGlyphCallback : public IGlyphCallback
{
public:
    GdiGlyphCallback(HDC hdc, double bottom)
    {
        _hdc = hdc;
        _bottom = bottom;
    }
    void glBegin(int mode)
    {
        //assert(mode == GL_LINE_STRIP);
        _points.clear();
    }

    void glVertex2d(double x, double y)
    {
        // mirror at _bottom so that text isnt upside down.
        y = _bottom - (y - _bottom);
        _points.push_back({ int(x), int(y) });
    }

    void glEnd()
    {
        if (_points.size() > 1)
            Polyline(_hdc, _points.data(), _points.size());
    }
private:
    HDC _hdc;
    double _bottom;
    std::vector<POINT> _points;
};

// Global Variables:
HINSTANCE hInst;
const WCHAR* szTitle = L"GdiGlyphCallback";         // The title bar text
const WCHAR* szWindowClass = L"GDIGLYPHCALLBACK";    // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

int main()
{
    // Initialize global strings
    MyRegisterClass(NULL);

    // Perform application initialization:
    if (!InitInstance (NULL, SW_SHOW))
    {
        return FALSE;
    }

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = NULL;
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = NULL;
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = NULL;

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);

            CRegBigFontShxParser shxParser;
            shxParser.Init("txt.shx", "hztxt.shx");
            shxParser.SetTextHeight(50);
            double bottom = 200;
            GdiGlyphCallback gdiCallback(hdc, bottom);
#undef DrawText
            shxParser.DrawText(&gdiCallback, L"Hello, 中国", 100, bottom);

            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
