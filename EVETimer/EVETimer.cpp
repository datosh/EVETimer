// EVETimer.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "EVETimer.h"

#define MAX_LOADSTRING 100

struct Timer
{
	int hours;
	int minutes;
	int seconds;
	BOOL active;
	char* description;
};

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
WCHAR szNewTimerWindowClass[MAX_LOADSTRING] = TEXT("NEWEVETIMER");	// the new timer window class name
std::vector<Timer> timers;
HWND mainHwnd = nullptr;
HWND newTimerHWnd = nullptr;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	NewTimerWndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
DWORD WINAPI		UpdateThreadFnc(LPVOID);



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
    LoadStringW(hInstance, IDC_EVETIMER, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_EVETIMER));

	DWORD threadId;
	HANDLE updateThread = CreateThread(NULL, 0, UpdateThreadFnc, NULL, 0, &threadId);

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

DWORD WINAPI UpdateThreadFnc(LPVOID lpParam)
{
	while (true)
	{
		Sleep(1000);
		PostMessage(mainHwnd, WM_UPDATE, 0, 0);
	}
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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_EVETIMER));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_EVETIMER);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	WNDCLASSEXW wcex_sub;

	wcex_sub.cbSize = sizeof(WNDCLASSEX);

	wcex_sub.style = CS_HREDRAW | CS_VREDRAW;
	wcex_sub.lpfnWndProc = NewTimerWndProc;
	wcex_sub.cbClsExtra = 0;
	wcex_sub.cbWndExtra = 0;
	wcex_sub.hInstance = hInstance;
	wcex_sub.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_EVETIMER));
	wcex_sub.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex_sub.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex_sub.lpszMenuName = NULL;
	wcex_sub.lpszClassName = szNewTimerWindowClass;
	wcex_sub.hIconSm = LoadIcon(wcex_sub.hInstance, MAKEINTRESOURCE(IDI_SMALL));


	return RegisterClassExW(&wcex) && RegisterClassExW(&wcex_sub);
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

   mainHwnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, CW_USEDEFAULT, 400, 400, nullptr, nullptr, hInstance, nullptr);

   if (!mainHwnd)
   {
      return FALSE;
   }

   ShowWindow(mainHwnd, nCmdShow);
   UpdateWindow(mainHwnd);

   return TRUE;
}

BOOL GetIntFromDlgItem(HWND hWnd, int& destination, int dlgID)
{
	BOOL success;
	destination = GetDlgItemInt(hWnd, dlgID, &success, false);
	if (!success)
	{
		MessageBox(hWnd, TEXT("Int could not be retrieved from dlg"), TEXT("ERROR"), MB_OK);
	}
	return success;
}

BOOL AddTimer(HWND hWnd)
{
	int hour, min, sec;

	if (!GetIntFromDlgItem(hWnd, hour, IDC_NEW_HOUR))
	{
		return false;
	}
	if (!GetIntFromDlgItem(hWnd, min, IDC_NEW_MINUTE))
	{
		return false;
	}
	if (!GetIntFromDlgItem(hWnd, sec, IDC_NEW_SECOND))
	{
		return false;
	}

	char description[256];

	GetDlgItemText(hWnd, IDC_NEW_DESCRIPTION, (LPWSTR)description, 256);
	
	Timer next = {};
	next.hours = hour;
	next.minutes = min;
	next.seconds = sec;
	next.active = true;
	next.description = (char*)malloc(sizeof(next.description) * 256);
	memcpy(next.description, description, 256);
	timers.push_back(next);

	return true;
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
	int numTimers = 0;
    switch (message)
    {
	case WM_ADD_TIMER_OVERVIEW:

		numTimers = timers.size();
		Timer current = timers[timers.size() - 1];
		WCHAR toDisplay[MAX_LOADSTRING];
		swprintf(toDisplay, sizeof(toDisplay), L"%d:%d:%d\t%s", current.hours, current.minutes, current.seconds, current.description);

		CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("STATIC"), toDisplay,
			WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | WS_TABSTOP,
			20, numTimers * 35 - 25, 250, 25, hWnd, (HMENU)(IDC_TIMER_ONE + numTimers - 1), GetModuleHandle(NULL), NULL);

		break;
	case WM_UPDATE:
		for (int i = 0; i < timers.size(); ++i)
		{
			if (!timers[i].active)
			{
				continue;
			}

			Timer current = timers[i];
			timers[i].seconds -= 1;
			if (timers[i].seconds == -1)
			{
				timers[i].seconds = 60;
				timers[i].minutes -= 1;
			}
			if (timers[i].minutes == -1)
			{
				timers[i].minutes = 60;
				timers[i].hours -= 1;
			}
			if (timers[i].hours == -1)
			{
				timers[i].hours = 0;
				timers[i].active = false;
			}
			WCHAR toDisplay[MAX_LOADSTRING];
			swprintf(toDisplay, sizeof(toDisplay), L"%d:%d:%d\t%s", current.hours, current.minutes, current.seconds, current.description);

			SetDlgItemText(hWnd, IDC_TIMER_ONE + i, toDisplay);
		}
		break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
			case IDM_NEW_TIMER:
				newTimerHWnd = CreateWindowEx(WS_EX_NOINHERITLAYOUT, szNewTimerWindowClass, TEXT("New Timer"),
					WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
					CW_USEDEFAULT, CW_USEDEFAULT, 200, 170, hWnd, NULL, NULL, NULL);
				ShowWindow(newTimerHWnd, SW_SHOW);
				UpdateWindow(newTimerHWnd);
				SetForegroundWindow(newTimerHWnd);

				CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT("Hours"),
					WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
					20, 10, 85, 25, newTimerHWnd, (HMENU)IDC_NEW_HOUR, GetModuleHandle(NULL), NULL);
				CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT("Minutes"),
					WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
					20, 40, 85, 25, newTimerHWnd, (HMENU)IDC_NEW_MINUTE, GetModuleHandle(NULL), NULL);
				CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT("Seconds"),
					WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
					20, 70, 85, 25, newTimerHWnd, (HMENU)IDC_NEW_SECOND, GetModuleHandle(NULL), NULL);

				CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT("Description"),
					WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
					20, 100, 150, 25, newTimerHWnd, (HMENU)IDC_NEW_DESCRIPTION, GetModuleHandle(NULL), NULL);

				CreateWindowEx(NULL, TEXT("BUTTON"), TEXT("ADD"),
					WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
					115, 40, 50, 25, newTimerHWnd, (HMENU)IDC_NEW_GO, GetModuleHandle(NULL), NULL);
				
				break;
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
			case IDC_NEW_GO:
				if (newTimerHWnd && AddTimer(newTimerHWnd))
				{
					DestroyWindow(newTimerHWnd);
				}
				break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
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

LRESULT CALLBACK NewTimerWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDC_NEW_GO:
			if (newTimerHWnd && AddTimer(newTimerHWnd))
			{
				PostMessage(mainHwnd, WM_ADD_TIMER_OVERVIEW, 0, 0);
				DestroyWindow(newTimerHWnd);
			}
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code that uses hdc here...
		EndPaint(hWnd, &ps);
	}
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
