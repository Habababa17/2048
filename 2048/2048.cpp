// 2048.cpp : Defines the entry point for the application.
//
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include "framework.h"
#include "resource.h"
#include "2048.h"
#include <chrono>
#include <thread>
using namespace std;
#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
WCHAR chWindowClass[MAX_LOADSTRING];
HWND* children = new HWND[16];
HWND counter = NULL;
int state[16] = { 0 };
HWND* children2 = new HWND[16];
HWND counter2 = NULL;
HWND hWnd1 = NULL;
HWND hWnd2 = NULL;
HWND overlay = NULL;
HWND overlay2 = NULL;
int score;
int goal = 8;
int won = 0;
// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
ATOM                MyRegisterClassChild(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int, ATOM&);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);


void ReDrawChildren();
void Add2Empty();
void UpdateAll();
bool CanMoveUp();
void MoveUp();
bool CanMoveLeft();
void MoveLeft();
bool CanMoveDown();
void MoveDown();
bool CanMoveRight();
void MoveRight();
void DrawSquare(HWND hwnd, PAINTSTRUCT& ps, int index);
void SaveState();
void LoadState();
void UpdateScore(HWND hwnd);
void CheckGoal(int v);


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
    LoadStringW(hInstance, IDC_MY2048, szWindowClass, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_MY2048, chWindowClass, MAX_LOADSTRING);

    
    ATOM parent = MyRegisterClass(hInstance);
    //UnregisterClass((LPCWSTR)parent, hInstance);
    //MyRegisterClassChild(hInstance);

    // Perform application initialization:
    if (!InitInstance(hInstance, nCmdShow, parent))
    {
        return FALSE;
    }
    UpdateWindow(hWnd1);
    UpdateWindow(hWnd2);
    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MY2048));

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

    return (int)msg.wParam;
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

    wcex.style = 0;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MY2048));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = CreateSolidBrush(RGB(250, 247, 238));
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_MY2048);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));


    return RegisterClassExW(&wcex);
}

ATOM MyRegisterClassChild(HINSTANCE childInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = 0;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = childInstance;
    wcex.hIcon = LoadIcon(childInstance, MAKEINTRESOURCE(IDI_MY2048));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = CreateSolidBrush(RGB(0, 0, 0));
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_MY2048);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

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



BOOL InitInstance(HINSTANCE hInstance, int nCmdShow, ATOM& parent)
{
    hInst = hInstance; // Store instance handle in our global variable
    LoadState();
    hWnd1 = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW| CS_PARENTDC,
        CW_USEDEFAULT, 0, 80*4+20, 80*4+20+60, nullptr, nullptr, hInstance, nullptr);

    SetWindowText(hWnd1, L"2048");
    if (!hWnd1)
    {
        return FALSE;
    }
    ShowWindow(hWnd1, nCmdShow);
    UpdateWindow(hWnd1);
    RECT rc;
    GetClientRect(hWnd1, &rc);

    const int margin = 60;
    const int offset = 20;
    const int squares = 4;

    hWnd2 = CreateWindowW(szWindowClass, szTitle,
        CS_PARENTDC | CS_OWNDC | WS_OVERLAPPEDWINDOW, 0, 0, 500, 500, hWnd1, nullptr, hInstance, nullptr);



    if (!hWnd2)
    {
        return FALSE;
    }
    ShowWindow(hWnd2, nCmdShow);
    UpdateWindow(hWnd2);

    PAINTSTRUCT ps;


    for (int i = 0; i < squares; i++) {
        for (int j = 0; j < squares; j++) {
            children[i * squares + j] = CreateWindowW(szWindowClass, szTitle, WS_CHILD | WS_OVERLAPPED  | WS_VISIBLE | CS_PARENTDC |
                CS_NOCLOSE, (margin + offset) * i + offset, (margin + offset) * j + offset, margin, margin, hWnd1, nullptr, hInstance, nullptr);
            

            if (!children[i * squares + j])
            {
                return FALSE;
            }
         
           
            ShowWindow(children[i * squares + j], nCmdShow);
            UpdateWindow(children[i * squares + j]);
        }
    }

    for (int i = 0; i < squares; i++) {
        for (int j = 0; j < squares; j++) {
            children2[i * squares + j] = CreateWindowW(szWindowClass, szTitle, WS_CHILD | WS_OVERLAPPED | WS_VISIBLE | CS_PARENTDC | 
                CS_NOCLOSE, (margin + offset) * i + offset, (margin + offset) * j + offset, margin, margin, hWnd2, nullptr, hInstance, nullptr);
            

            if (!children2[i * squares + j])
            {
                return FALSE;
            }
            
            // SetClassLongPtr(children2[i * squares + j], 0, RGB(rand() % 256 , 0, 0));
            
            ShowWindow(children2[i * squares + j], nCmdShow);
            UpdateWindow(children2[i * squares + j]);
        }
    }

    counter = CreateWindowW(szWindowClass, szTitle, WS_CHILD | WS_OVERLAPPED | WS_VISIBLE | CS_PARENTDC |
        CS_NOCLOSE, offset, (margin + offset) * 4 + offset, (margin + offset) * 4 - offset, margin, hWnd1, nullptr, hInstance, nullptr);
    if (!counter)
    {
        return FALSE;
    }
    ShowWindow(counter, nCmdShow);
    UpdateWindow(counter);

    counter2 = CreateWindowW(szWindowClass, szTitle, WS_CHILD | WS_OVERLAPPED | WS_VISIBLE | CS_PARENTDC |
        CS_NOCLOSE, offset, (margin + offset) * 4 + offset, (margin + offset) * 4 - offset, margin, hWnd2, nullptr, hInstance, nullptr);
    if (!counter2)
    {
        return FALSE;
    }
    ShowWindow(counter2, nCmdShow);
    UpdateWindow(counter2);

    UpdateScore(counter);
    UpdateScore(counter2);
    
  
    
    


    for (int i = 0; i < 16; i++) {
        if (state[i]) return TRUE;
    }
    Add2Empty();
    
    return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static HDC offDC = NULL;
    static HBITMAP offOldBitmap = NULL;
    static HBITMAP offBitmap = NULL;

    switch (message)
    {


        case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                //DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                
                break;
            case ID_HELP_8:
                CheckMenuItem(GetMenu(hWnd), ID_HELP_8 , MF_BYCOMMAND|MF_CHECKED);
                CheckMenuItem(GetMenu(hWnd), ID_HELP_16, MF_BYCOMMAND | MF_UNCHECKED);
                CheckMenuItem(GetMenu(hWnd), ID_HELP_64, MF_BYCOMMAND | MF_UNCHECKED);
                CheckMenuItem(GetMenu(hWnd), ID_HELP_2048, MF_BYCOMMAND | MF_UNCHECKED);

                goal = 8;
                won = 0;
                break;
            case ID_HELP_16:
                CheckMenuItem(GetMenu(hWnd), ID_HELP_8, MF_BYCOMMAND | MF_UNCHECKED);
                CheckMenuItem(GetMenu(hWnd), ID_HELP_16, MF_BYCOMMAND | MF_CHECKED);
                CheckMenuItem(GetMenu(hWnd), ID_HELP_64, MF_BYCOMMAND | MF_UNCHECKED);
                CheckMenuItem(GetMenu(hWnd), ID_HELP_2048, MF_BYCOMMAND | MF_UNCHECKED);
                goal = 16;
                won = 0;
                break;
            case ID_HELP_64:
                CheckMenuItem(GetMenu(hWnd), ID_HELP_8, MF_BYCOMMAND | MF_UNCHECKED);
                CheckMenuItem(GetMenu(hWnd), ID_HELP_16, MF_BYCOMMAND | MF_UNCHECKED);
                CheckMenuItem(GetMenu(hWnd), ID_HELP_64, MF_BYCOMMAND | MF_CHECKED);
                CheckMenuItem(GetMenu(hWnd), ID_HELP_2048, MF_BYCOMMAND | MF_UNCHECKED);
                goal = 64;
                won = 0;
                break;
            case ID_HELP_2048:
                CheckMenuItem(GetMenu(hWnd), ID_HELP_8, MF_BYCOMMAND | MF_UNCHECKED);
                CheckMenuItem(GetMenu(hWnd), ID_HELP_16, MF_BYCOMMAND | MF_UNCHECKED);
                CheckMenuItem(GetMenu(hWnd), ID_HELP_64, MF_BYCOMMAND | MF_UNCHECKED);
                CheckMenuItem(GetMenu(hWnd), ID_HELP_2048, MF_BYCOMMAND | MF_CHECKED);
                goal = 2048;
                won = 0;
                break;
            case IDM_EXIT:
                //SaveState();
                //DestroyWindow(hWnd);
                score = 0;
                won = 0;
                for (int i = 0; i < 16; i++) state[i] = 0;
                UpdateAll();
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;

        case WM_MOVE:
        {
            int screenx = GetSystemMetrics(SM_CXSCREEN) / 2;
            int screeny = GetSystemMetrics(SM_CYSCREEN) / 2;

            RECT rc;
            GetWindowRect(hWnd, &rc);



            if (hWnd == hWnd1) {
                MoveWindow(hWnd2, (2 * screenx - rc.right), 2 * screeny - rc.bottom, 500, 500, FALSE);
            }
            if (hWnd == hWnd2) {
                MoveWindow(hWnd1, (2 * screenx - rc.right), 2 * screeny - rc.bottom, 500, 500, FALSE);
            }

            if (rc.left < screenx && screenx < rc.right && rc.bottom > screeny && screeny > rc.top) {

                //SetWindowLong(hWnd, GWL_EXSTYLE,
                //    GetWindowLong(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
                SetLayeredWindowAttributes(hWnd, 0, (255 * 50) / 100, LWA_ALPHA);

                //UpdateWindow(hWnd);
            }
            else {
                SetLayeredWindowAttributes(hWnd, 0, 255, LWA_ALPHA);

            }

            UpdateWindow(hWnd);
            


        }
        break;



        case WM_GETMINMAXINFO:
        {
            RECT a;
            a.left = a.top = 0;
            a.right = 345;
            a.bottom = 485;
            AdjustWindowRect(&a, WS_OVERLAPPEDWINDOW | CS_PARENTDC , TRUE);

            if (hWnd == hWnd1 || hWnd == hWnd2) {
                MINMAXINFO* minMaxInfo = (MINMAXINFO*)lParam;
                minMaxInfo->ptMaxSize.x = minMaxInfo->ptMaxTrackSize.x = a.right;
                minMaxInfo->ptMaxSize.y = minMaxInfo->ptMaxTrackSize.y = a.bottom;
                minMaxInfo->ptMinTrackSize.x = a.right;
                minMaxInfo->ptMinTrackSize.y = a.bottom;
            }

        }
        break;

        case WM_CREATE:
        {

            HDC hdc = GetDC(hWnd);
            offDC = CreateCompatibleDC(hdc);
            ReleaseDC(hWnd, hdc);
            SetWindowLong(hWnd, GWL_EXSTYLE,
                GetWindowLong(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
            
        }
        break;
        case WM_SIZE:
        {
            int clientWidth = LOWORD(lParam);
            int clientHeight = HIWORD(lParam);
            HDC hdc = GetDC(hWnd);
            if (offOldBitmap != NULL) {
                SelectObject(offDC, offOldBitmap);

            }
            if (offBitmap != NULL) {
                DeleteObject(offBitmap);

            }
            offBitmap = CreateCompatibleBitmap(hdc, clientWidth, clientHeight);
            offOldBitmap = (HBITMAP)SelectObject(offDC, offBitmap);
            ReleaseDC(hWnd, hdc);
        }
        break;

       

        case WM_KEYDOWN:
            if (won) break;
            switch ((int)wParam)
            {
            case (int)'W':
                if (!CanMoveUp()) break;
                CheckGoal(8);
                MoveUp();
                UpdateAll();
                break;

            case (int)'A':
                if (!CanMoveLeft()) break;
                MoveLeft();
                UpdateAll();
                break;

            case (int)'S':
                if (!CanMoveDown()) break;
                MoveDown();
                UpdateAll();
                break;

            case (int)'D':
                if (!CanMoveRight()) break;
                MoveRight();
                UpdateAll();
                break;     
            }
            

            break;
        case WM_SETFOCUS:
        {
            ReDrawChildren();
        }
        break;
    case WM_PAINT:
    {
        //if (hWnd == hWnd2) break;
       
        //ReDrawChildren();
        PAINTSTRUCT ps;
        RECT rc;
        GetWindowRect(hWnd, &rc);
        if (rc.right - rc.left > 100) break;
        
        
        const int margin = 60;
        const int offset = 10;
        const int squares = 4;


        //Rectangle(hdc, 0, 0, margin, margin);

        UpdateScore(counter);
        UpdateScore(counter2);

        for (int i = 0; i < squares * squares; i++) {
            DrawSquare(children[i], ps, i);
            DrawSquare(children2[i], ps, i);
           
        }
       // DrawSquare(overlay, ps, 4);
      
        
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

bool CanMoveUp() 
{
    
    for (int j = 0; j < 4; j++)
        for (int i = 1; i < 4; i++) 
        {
            if ((state[i - 1 + j * 4] == 0 && state[i + 4 * j] != 0) || (state[i - 1 + j * 4] == state[i + j * 4] && state[i + 4 * j] != 0)) return true;
        }
    return false;
}
void MoveUp() {
 
    for (int j = 0; j < 4; j++) {
        int mer = 0;
        for (int i = 1; i < 4; i++) {
            int curi = i - 1;
            int curj = j;
            if (state[i + 4 * j] == 0) continue;
            while (curi > 0) {
                if (state[curi + 4 * j] == 0) curi--;
                else break;
            }
            curi += mer;
            mer = 0;
            //if (curi < 0) continue;
            if (state[curi + 4 * j] == state[i + 4 * j]) {
                if (pow(2, ++state[curi + 4 * j]) == goal) won = 1;
                mer = 1;
                score += pow(2, state[curi + 4 * j]);

                state[i + 4 * j] = 0;
                continue;
            }
            if (state[curi + 4 * j] == 0) {
                state[curi + 4 * j] = state[i + 4 * j];
                state[i + 4 * j] = 0;
                continue;
            }
            int temp = state[i + 4 * j];
            state[i + 4 * j] = 0;
            state[curi + 1 + 4 * j] = temp;
        }

    }
    
}
bool CanMoveLeft()
{

    for (int j = 1; j < 4; j++)
        for (int i = 0; i < 4; i++)
        {
            if ((state[i + (j-1) * 4] == 0 && state[i + 4 * j] != 0) || (state[i + (j - 1) * 4] == state[i + j * 4] && state[i + 4 * j] != 0)) return true;
        }
    return false;
}
void MoveLeft()
{
    for (int i = 0; i < 4; i++)
    {
        int mer = 0;
        for (int j = 1; j < 4; j++)
        {
            int curi = i;
            int curj = j - 1;
            if (state[i + 4 * j] == 0) continue;
            while (curj > 0) {
                if (state[i + 4 * curj] == 0) curj--;
                else break;
            }
            curj += mer;
            mer = 0;
            //if (curi < 0) continue;
            if (state[i + 4 * curj] == state[i + 4 * j]) {
                if (pow(2, ++state[i + 4 * curj]) == goal) won = 1;
                score += pow(2, state[i + 4 * curj]);
                mer = 1;
                state[i + 4 * j] = 0;
                continue;
            }
            if (state[i + 4 * curj] == 0) {
                state[i + 4 * curj] = state[i + 4 * j];
                state[i + 4 * j] = 0;
                continue;
            }
            int temp = state[i + 4 * j];
            state[i + 4 * j] = 0;
            state[i + 4 * (curj + 1)] = temp;
        }
    }

}
bool CanMoveDown()
{
    for (int j = 0; j < 4; j++)
        for (int i = 0; i < 3; i++)
        {
            if ((state[i + 1 + j * 4] == 0 && state[i + 4 * j] != 0) || (state[i + 1 + j * 4] == state[i + j * 4] && state[i + 4 * j] != 0)) return true;
        }
    return false;
}
void MoveDown()
{
    for (int j = 0; j < 4; j++) {
        int mer = 0;
        for (int i = 2; i >= 0; i--) {
            int curi = i + 1;
            int curj = j;
            if (state[i + 4 * j] == 0) continue;
            while (curi < 3) {
                if (state[curi + 4 * j] == 0) curi++;
                else break;
            }
            curi -= mer;
            mer = 0;
            if (state[curi + 4 * j] == state[i + 4 * j]) {
                if (pow(2, ++state[curi + 4 * j]) == goal) won = 1;
                score += pow(2, state[curi + 4 * j]);
                mer = 1;
                state[i + 4 * j] = 0;
                continue;
            }
            if (state[curi + 4 * j] == 0) {
                state[curi + 4 * j] = state[i + 4 * j];
                state[i + 4 * j] = 0;
                continue;
            }
            int temp = state[i + 4 * j];
            state[i + 4 * j] = 0;
            state[curi - 1 + 4 * j] = temp;
        }
    }
}
bool CanMoveRight()
{
    for (int j = 0; j < 3; j++)
        for (int i = 0; i < 4; i++)
        {
            if (state[i + 4 * j] == 0) continue;
            if (state[i + (j + 1) * 4] == 0   || state[i + (j + 1) * 4] == state[i + j * 4] ) return true;
        }
    return false;
}
void MoveRight()
{
    for (int i = 0; i < 4; i++) {
        int mer = 0;
        for (int j = 2; j >= 0; j--)
        {
            int curi = i;
            int curj = j + 1;
            if (state[i + 4 * j] == 0) continue;
            while (curj < 3) {
                if (state[i + 4 * curj] == 0) curj++;
                else break;
            }
            curj -= mer;
            mer = 0;
            if (state[i + 4 * curj] == state[i + 4 * j]) {
                if (pow(2, ++state[i + 4 * curj]) == goal)won = 1;
                score += pow(2, state[i + 4 * curj]);
                mer = 1;
                state[i + 4 * j] = 0;
                continue;
            }
            if (state[i + 4 * curj] == 0) {
                state[i + 4 * curj] = state[i + 4 * j];
                state[i + 4 * j] = 0;
                continue;
            }
            int temp = state[i + 4 * j];
            state[i + 4 * j] = 0;
            state[i + 4 * (curj - 1)] = temp;
        }
    }
}
void Add2Empty() {
    int zerocount = 0;
    srand((int)time(NULL));
    for (int i = 0; i < 16; i++) if (state[i] == 0) zerocount++;
    if (zerocount) {
        int place = rand() % zerocount + 1;
        int i = -1;
        while (place != 0) {
            i++;
            if (state[i] == 0) place--;
            
        }
        
        state[i] = 12;
        ReDrawChildren();
        std::this_thread::sleep_for(std::chrono::nanoseconds(10000000));
        state[i] = 13;
        ReDrawChildren();
        std::this_thread::sleep_for(std::chrono::nanoseconds(10000000));
        state[i] = 1;
        ReDrawChildren();
    }
}
void UpdateAll() {

    Add2Empty();
    SaveState();
   // ReDrawChildren();
   // UpdateScore(counter);
   // UpdateScore(counter2);
}
void ReDrawChildren() {
    InvalidateRect(hWnd1, NULL, NULL);
    UpdateWindow(hWnd1);
    InvalidateRect(hWnd2, NULL, NULL);
    UpdateWindow(hWnd2);
}
void UpdateScore(HWND hwnd)
{
    PAINTSTRUCT ps;
    COLORREF color = RGB(204, 192, 174);
    HDC hdc = BeginPaint(hwnd, &ps);
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, CreateSolidBrush(color));
    Rectangle(hdc, 0, 0, 300, 60);
   
    RECT rect;
    HDC hdc2 = GetWindowDC(hwnd);
    GetWindowRect(hwnd, &rect);
    SetTextColor(hdc2, 0x00000000);
    SetBkMode(hdc2, TRANSPARENT);
    rect.left = 20;
    rect.top = 20;
    string a = to_string(score);
    TextOutA(hdc2, 150 , 15, a.c_str(), a.length());
    DeleteDC(hdc2);
    EndPaint(hwnd, &ps);
}
void DrawSquare(HWND hwnd, PAINTSTRUCT& ps, int index) {
    COLORREF colors[] =
    {
        RGB(204, 192, 174), //empty 0
        RGB(238, 228, 198), //dla 2 1
        RGB(239, 225, 218), //dla 4 2
        RGB(243, 179, 124), //dla 8 3
        RGB(246, 153, 100), //dla 16 4
        RGB(246, 125, 98), //dla 32 5
        RGB(247, 93, 60), //dla 64 6
        RGB(237, 206, 116), //dla 128 7
        RGB(239, 204, 98), //dla 256 8
        RGB(243, 201, 85), //dla 512 9
        RGB(238, 200, 72), //dla 1024 10
        RGB(239, 192, 47), //dla 2048 11

    };
    LPCWSTR numbers[] =
    {
        L"", //empty
        L"2", //2
        L"4", //4
        L"8", //8
        L"16", //16
        L"32", //32
        L"64", //64
        L"128", //128
        L"256", //256
        L"512", //512
        L"1024", //1024
        L"2048", //2048
    };
    int num = state[index] % 12;
    bool isanimation = state[index] > 11;
    int margin = 60;
    RECT rc;
    GetWindowRect(hwnd, &rc);
    HDC hdc = BeginPaint(hwnd, &ps);
    HGDIOBJ newbrush = CreateSolidBrush(colors[num]);
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc,newbrush );
    if(isanimation)
    {
        if (num == 0) {
            RoundRect(hdc, 15, 15, margin - 15, margin - 15, margin - 10, margin - 10);
        }
        if (num == 1) {
            RoundRect(hdc, 7, 7, margin - 7, margin - 7, margin, margin);
        }
        if (num > 1) {
            RoundRect(hdc, -15,-15 , margin+15 , margin +15, margin+10, margin+10);
            state[index] -= 12;
        }

    }
    else
    {
        RoundRect(hdc, -5, -5, margin + 5, margin + 5, margin + 20, margin + 20);
        RECT rect;
        HDC hdc2 = GetWindowDC(hwnd);
        GetWindowRect(hwnd, &rect);
        SetTextColor(hdc2, 0x00000000);
        SetBkMode(hdc2, TRANSPARENT);
        rect.left = 20;
        rect.top = 20;
        DrawText(hdc2, numbers[num], -1, &rect, DT_SINGLELINE | DT_NOCLIP);
        DeleteDC(hdc2);
    }
    DeleteDC(hdc);
    EndPaint(hwnd, &ps);
    DeleteObject(newbrush);
    SelectObject(hdc, oldBrush);
    
    
}

void SaveState() {
    ofstream stream;
    stream.open("C://Users/10710/source/repos/2048/Debug/dane.txt");
    for (int i = 0; i < 16; i++)
        stream << state[i] << " ";
    stream << score << " ";
    stream << goal << " ";
    stream << won;
    stream.close();
}
void LoadState() {
    ifstream stream;
    stream.open("C://Users/10710/source/repos/2048/Debug/dane.txt");
    for (int i = 0; i < 16; i++)
        if (stream.is_open())
        {
            for (int i = 0; i < 16; i++)
            {
                stream >> state[i];
            }
            stream >> score;
            stream >> goal;
            stream >> won;
            stream.close(); // CLose input file
        }


}
void CheckGoal(int v) {
    //if (v == goal) 
    {
        
    }
}