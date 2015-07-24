#include "stdafx.h"

#define MAX_LOADSTRING 100

#define SAVE_FILE_NAME L"EVETIMER.save"

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
WCHAR szNewTimerWindowClass[MAX_LOADSTRING] = TEXT("NEWEVETIMER");	// the new timer window class name
std::vector<Timer> timers;
HWND mainHwnd = nullptr;
HWND newTimerHWnd = nullptr;
wchar_t saveFilePath[MAX_STRING];
int successfull_loads = 0;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	NewTimerWndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
DWORD WINAPI		UpdateThreadFnc(LPVOID);

void loadAndSetup(void)
{
	// Get the path to %APPDATA%
	PWSTR roamingAppDataPath;
	SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, NULL, &roamingAppDataPath);

	// Test if the Folder already exists and create
	wchar_t saveFolderPath[MAX_STRING];
	swprintf_s(saveFolderPath, MAX_STRING, L"%s\\EVETIMER", roamingAppDataPath);
	if (CreateDirectory(saveFolderPath, NULL))
	{
		MessageBox(NULL, TEXT("You are using EVETimer for the first time. A folder to save timers was created!"), TEXT("Welcome"), MB_OK);
	}

	// Loaded save file path
	swprintf_s(saveFilePath, MAX_STRING, L"%s\\%s", saveFolderPath, SAVE_FILE_NAME);

	// Open file
	std::wfstream infile(saveFilePath);
	if (!infile.good())
	{
		MessageBox(NULL, TEXT("Something went wrong opening the save file. Timer probably can't be saved!"), TEXT("ERROR!"), MB_OK);
	}

	// Parse input
	std::wstring line;
	std::wstring delimiter = L":::";
	while (std::getline(infile, line))
	{
		// Check if the save string is correctly formatted
		auto pos = line.find(delimiter);
		if (pos == std::wstring::npos)
		{
			MessageBox(NULL, TEXT("Can't find the delimiter in the save string. Timer probably can't be loaded!"), TEXT("ERROR!"), MB_OK);
			continue;
		}

		// Load end_time and description
		std::wstring token = line.substr(0, pos);
		auto end_time = std::stol(token);
		std::wstring description = line.substr(pos + 3, std::wstring::npos);
		Timer t(end_time);
		t.setDescr(&description[0]);
		timers.push_back(t);
		
		

		successfull_loads += 1;
	}
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

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
		std::this_thread::sleep_for(std::chrono::seconds(1));
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

   // Create text field for display
   for (int i = 0; i < timers.size(); ++i)
   {
	   CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("STATIC"), timers[i].getDisplayText(),
		   WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | WS_TABSTOP,
		   20, (i+1) * 35 - 25, 250, 25, mainHwnd, (HMENU)(IDC_TIMER_ONE + i), GetModuleHandle(NULL), NULL);
   }

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

	wchar_t description[256];

	GetDlgItemText(hWnd, IDC_NEW_DESCRIPTION, description, 256);
	
	Timer next(0, hour, min, sec);
	next.setDescr(description);
	timers.push_back(next);

	std::wofstream outfile(saveFilePath, std::wofstream::app);
	outfile << next.getSaveString() << std::endl;

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
	int numTimers = 0, i = 0;
	Timer current;

    switch (message)
    {
	case WM_CREATE:
		loadAndSetup();
		break;
	case WM_ADD_TIMER_OVERVIEW:

		numTimers = timers.size();
		current = timers[timers.size() - 1];

		CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("STATIC"), current.getDisplayText(),
			WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | WS_TABSTOP,
			20, numTimers * 35 - 25, 250, 25, hWnd, (HMENU)(IDC_TIMER_ONE + numTimers - 1), GetModuleHandle(NULL), NULL);

		break;
	case WM_UPDATE:
		i = 0;
		for (auto &t : timers)
		{
			if (t.isActive())
			{
				t.tick();
				SetDlgItemText(hWnd, IDC_TIMER_ONE + i, t.getDisplayText());
			}
			i += 1;
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
