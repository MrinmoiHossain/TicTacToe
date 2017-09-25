// TicTacToe.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "TicTacToe.h"
#include <windowsx.h>

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_TICTACTOE, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TICTACTOE));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TICTACTOE));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)GetStockObject(DKGRAY_BRUSH);
    //wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);	->Default Background Color
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_TICTACTOE);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

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

//Board Size
const int CELL_SIZE = 100;
//Board Color
HBRUSH color1, color2;
//Player Variable
int playerTurn = 1;
int gameBoard[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };

//Game Board
BOOL GetGameBoardRect(HWND hwnd, RECT *pRect)
{
	RECT rc;
	if (GetClientRect(hwnd, &rc)) {
		int width = rc.right - rc.left;
		int height = rc.bottom - rc.top;


		pRect->left = (width - CELL_SIZE * 3) / 2;
		pRect->top = (height - CELL_SIZE * 3) / 2;
		pRect->right = pRect->left + CELL_SIZE * 3;
		pRect->bottom = pRect->top + CELL_SIZE * 3;

		return TRUE;
	}
	SetRectEmpty(pRect);
	return FALSE;
}

//Seperate Draw Line
void DrawLine(HDC hdc, int x1, int y1, int x2, int y2)
{
	MoveToEx(hdc, x1, y1, NULL);
	LineTo(hdc, x2, y2);
}

//Postion of Mouse Click
int GetCellNumberFromPoint(HWND hwnd, int x, int y)
{
	RECT rc;
	POINT pt = { x, y};

	if (GetGameBoardRect(hwnd, &rc)) {
		if (PtInRect(&rc, pt)) {
			//user clicked inside game board
			x = pt.x - rc.left;
			y = pt.y - rc.top;

			int column = x / CELL_SIZE;
			int row = y / CELL_SIZE;

			//conver to index: 0 to 8
			return column + row * 3;
		}
	}

	return -1;	//clicked outside game board or failure
}

BOOL GetCellRect(HWND hWnd, int index, RECT *pRect)
{
	RECT rcBoard;

	SetRectEmpty(pRect);
	if (index < 0 || index > 8) {
		return FALSE;
	}

	if (GetGameBoardRect(hWnd, &rcBoard)) {
		//conver index for 0 to 8 into (x, y) pair
		int r = index / 3;	//row number
		int c = index % 3;	//column number

		pRect->left = rcBoard.left + c * CELL_SIZE;
		pRect->top = rcBoard.top + r * CELL_SIZE;
		pRect->right = pRect->left + CELL_SIZE;
		pRect->bottom = pRect->top + CELL_SIZE;

		return TRUE;
	}

	return FALSE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
	case WM_CREATE:
		{
			color1 = CreateSolidBrush(RGB(0, 25, 51));
			color2 = CreateSolidBrush(RGB(0, 51, 51));
		}
		break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
	case WM_LBUTTONDOWN:
		{
			int xPos = GET_X_LPARAM(lParam);
			int yPos = GET_Y_LPARAM(lParam);

			int index = GetCellNumberFromPoint(hWnd, xPos, yPos);

			HDC hdc = GetDC(hWnd);

			if (NULL != hdc) {
				//WCHAR temp[100];

				//wsprintf(temp, L"Index = %d", index);
				//TextOut(hdc, xPos, yPos, temp, lstrlen(temp));

				//Get cell dimension from its index
				if (index != -1) {
					RECT rcCell;
					if ((gameBoard[index] == 0) && GetCellRect(hWnd, index, &rcCell)) {
						gameBoard[index] = playerTurn;
						FillRect(hdc, &rcCell, (playerTurn == 1) ? color1 : color2);
						playerTurn = (playerTurn == 1) ? 2 : 1;
					}
				}
				ReleaseDC(hWnd, hdc);
			}
		}
		break;
	case WM_GETMINMAXINFO:
		{
			MINMAXINFO *pMinMax = (MINMAXINFO *)lParam;

			pMinMax->ptMinTrackSize.x = CELL_SIZE * 5;
			pMinMax->ptMinTrackSize.y = CELL_SIZE * 5;
		}
		break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            //Game Main Start
			RECT rc;

			if (GetGameBoardRect(hWnd, &rc)) {
				FillRect(hdc, &rc, (HBRUSH)GetStockObject(LTGRAY_BRUSH));
				//Rectangle(hdc, rc.left, rc.top, rc.right, rc.bottom);
			}

			for (int i = 1; i < 3; i++) {
				//Draw Vertical Lines
				DrawLine(hdc, rc.left + CELL_SIZE * i, rc.top, rc.left + CELL_SIZE * i, rc.bottom);
				//Draw Horizontal Lines
				DrawLine(hdc, rc.left, rc.top + CELL_SIZE * i, rc.right, rc.top + CELL_SIZE * i);
			}
			
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
		DeleteObject(color1);
		DeleteObject(color2);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
