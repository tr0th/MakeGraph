// MakeGraph.cpp : Определяет точку входа для приложения.
//

#include "framework.h"
#include "MakeGraph.h"
#include <cwchar>
#include "math.h"
#include "zmouse.h"

#ifndef WM_MOUSEWHEEL
#define WM_MOUSEWHEEL WM_MOUSELAST+1
#endif

int windowPositionX = 200;
int windowPositionY = 200;

#define WINDOW_WIDTH  640
#define WINDOW_HEIGHT 480

UINT uMSH_MOUSEWHEEL = 0;

#define Pi 3.14159265
#define MAX_LOADSTRING 100
#define IDM_MYMENURESOURCE   4
#define IDB_Button1 1
#define IDB_ButtonStop 2
#define IDB_ButtonStart 3

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна

// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

//тестовые переменные
bool mousePressed;
int mouseOnWindowX;
int mouseOnWindowY;
int mouseX;
int mouseY;

int StrToInt(TCHAR *s)
{
    int temp = 0; // число
    int i = 0;
    int sign = 0; // знак числа 0- положительное, 1 - отрицательное
    if (s[i] == '-')
    {
        sign = 1;
        i++;
    }
    while (s[i] >= 0x30 && s[i] <= 0x39)
    {
        temp = temp + (s[i] & 0x0F);
        temp = temp * 10;
        i++;
    }
    temp = temp / 10;
    if (sign == 1)
        temp = -temp;
    return(temp);
}

char* IntToStr(int n)
{
    char s[40], t, * temp;
    int i, k;
    int sign = 0;
    i = 0;
    k = n;
    if (k < 0)
    {
        sign = 1;
        k = -k;
    }
    do {
        t = k % 10;
        k = k / 10;
        s[i] = t | 0x30;
        i++;
    } while (k > 0);
    if (sign == 1)
    {
        s[i] = '-';
        i++;
    }
    temp = new char[i];
    k = 0;
    i--;
    while (i >= 0) {
        temp[k] = s[i];
        i--; k++;
    }
    temp[k] = '\0';
    return(temp);
}


void DrawDiagram(HWND hwnd, HDC hdc, int offset, int Amplitude, int Herz);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Разместите код здесь.

    // Инициализация глобальных строк
    MSG msg;
    HACCEL hAccelTable;

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_MAKEGRAPH, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Выполнить инициализацию приложения:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    uMSH_MOUSEWHEEL = RegisterWindowMessage(MSH_MOUSEWHEEL);

    if (!uMSH_MOUSEWHEEL)
    {
        GetMessage(&msg, nullptr, 0, 0);
        MessageBoxA(NULL, "RegisterWindowMessage Failed!", "Error", MB_OK);
        return msg.wParam;
    }

    hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MAKEGRAPH));

    // Цикл основного сообщения:
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
//  ФУНКЦИЯ: MyRegisterClass()
//
//  ЦЕЛЬ: Регистрирует класс окна.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_MAKEGRAPH);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}


//
//   ФУНКЦИЯ: InitInstance(HINSTANCE, int)
//
//   ЦЕЛЬ: Сохраняет маркер экземпляра и создает главное окно
//
//   КОММЕНТАРИИ:
//
//        В этой функции маркер экземпляра сохраняется в глобальной переменной, а также
//        создается и выводится главное окно программы.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Сохранить маркер экземпляра в глобальной переменной

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_MAXIMIZE | WS_OVERLAPPEDWINDOW,
      windowPositionX, windowPositionY, WS_MAXIMIZE, WS_MAXIMIZE, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  ФУНКЦИЯ: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ЦЕЛЬ: Обрабатывает сообщения в главном окне.
//
//  WM_COMMAND  - обработать меню приложения
//  WM_PAINT    - Отрисовка главного окна
//  WM_DESTROY  - отправить сообщение о выходе и вернуться
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;
    HDC hdcWrap;
    HDC graphic;
    RECT rect;
    static HWND graphWrap;
    static HWND graph;
    static HWND hEdt;
    static HWND hEdtHerz;
    static HWND hBtnApply;
    static HWND hBtnStart;
    static HWND hBtnStop;
    static int offset = 0;
    static int Amplit = 300;
    static int Herz = 20;
    static int Anim = 100;
    static double zoom = 1;
    int inPut;
    TCHAR StrA[20];

    SetTimer(hWnd, 100, Anim, NULL);


    switch (message)
    {
    case WM_CREATE:
        GetClientRect(hWnd, &rect);
        GetWindowRect(hWnd, &rect);
        graphWrap = CreateWindowW(L"static", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_RIGHT, 500, 200, 500, 400, hWnd, 0, hInst, NULL);
        ShowWindow(graphWrap, SW_SHOWNORMAL);

        GetClientRect(graphWrap, &rect);

        graph = CreateWindowW(L"static", NULL, WS_CHILD | WS_VISIBLE | ES_RIGHT, rect.right / 100, rect.bottom / 2.5, rect.right / 0.36, rect.bottom / 1.3, graphWrap, 0, hInst, NULL);
        ShowWindow(graph, SW_SHOWNORMAL);
        hEdt = CreateWindowW(L"edit", L"300", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_RIGHT, rect.right / 18, rect.top / 20, 60, 20, hWnd, 0, hInst, NULL);
        ShowWindow(hEdt, SW_SHOWNORMAL);
        hEdtHerz = CreateWindowW(L"edit", L"20", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_RIGHT, 500, 80, 60, 20, hWnd, 0, hInst, NULL);
        ShowWindow(hEdtHerz, SW_SHOWNORMAL);
        hBtnApply = CreateWindowW(L"button", L"Принять", WS_CHILD | WS_VISIBLE | WS_BORDER, 500, 120, 60, 30, hWnd, (HMENU)IDB_Button1, hInst, NULL);
        ShowWindow(hBtnApply, SW_SHOWNORMAL);
        hBtnStart = CreateWindowW(L"button", L"Запустить", WS_CHILD | WS_VISIBLE | WS_BORDER, 500, 160, 85, 30, hWnd, (HMENU)IDB_ButtonStart, hInst, NULL);
        ShowWindow(hBtnStart, SW_SHOWNORMAL);
        hBtnStop = CreateWindowW(L"button", L"Остановить", WS_CHILD | WS_VISIBLE | WS_BORDER, 500, 200, 85, 30, hWnd, (HMENU)IDB_ButtonStop, hInst, NULL);
        ShowWindow(hBtnStop, SW_SHOWNORMAL);

        break;
    case WM_COMMAND:
    {
        if (wParam == IDB_Button1)
        {
            inPut = GetWindowText(hEdt, StrA, 20);
            Amplit = StrToInt(StrA);

            inPut = GetWindowText(hEdtHerz, StrA, 20);
            Herz = StrToInt(StrA);
            InvalidateRect(hWnd, 0, true);
            UpdateWindow(hWnd);
        }

        if (wParam == IDB_ButtonStop)
        {
            Anim = -1;
        }
        else if (wParam == IDB_ButtonStart)
        {
            Anim = 100;
        }

        int wmId = LOWORD(wParam);
        // Разобрать выбор в меню:
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
    case WM_SIZE:
        GetClientRect(hWnd, &rect);
        GetWindowRect(hWnd, &rect);
        SetWindowPos(hWnd, nullptr, 0, 0, rect.right, rect.bottom, SWP_NOZORDER);
        SetWindowPos(graphWrap, nullptr, rect.right / 1000, rect.bottom / 20, rect.right * 1, rect.bottom * 0.9, SWP_NOZORDER);
        SetWindowPos(hEdt, nullptr, rect.right / 18, rect.top / 20, rect.right / 14, rect.bottom / 30, SWP_NOZORDER);
        SetWindowPos(hEdtHerz, nullptr, rect.right / 5, rect.top / 20, rect.right / 14, rect.bottom / 30, SWP_NOZORDER);
        SetWindowPos(hBtnApply, nullptr, rect.right / 3.5, rect.top / 20, rect.right / 14, rect.bottom / 30, SWP_NOZORDER);
        SetWindowPos(hBtnStart, nullptr, rect.right / 2.7, rect.top / 20, rect.right / 14, rect.bottom / 30, SWP_NOZORDER);
        SetWindowPos(hBtnStop, nullptr, rect.right / 2.2, rect.top / 20, rect.right / 14, rect.bottom / 30, SWP_NOZORDER);

        //GetClientRect(graphWrap, &rect);
        //GetWindowRect(graphWrap, &rect);

        //SetWindowPos(graph, nullptr, rect.right / 200, rect.bottom / 200, rect.right / 0.3, rect.bottom / 0.2, SWP_NOZORDER);

        return 0;
        break;
    case WM_TIMER:
    {
        GetClientRect(hWnd, &rect);
        InvalidateRect(hWnd, &rect, true);
        UpdateWindow(hWnd);
        ++offset;
        if (Amplit > 1000)
        {
            Amplit = 300;
            SetWindowText(hEdt, L"300");
            MessageBox(GetActiveWindow(), L"Введено число выше допустимого значения!", L"Недопустимое значение", MB_ICONASTERISK);
        }
        else if (Amplit < 0)
        {
            Amplit = 300;
            SetWindowText(hEdt, L"300");
            MessageBox(GetActiveWindow(), L"Введено число ниже допустимого значения!", L"Недопустимое значение", MB_ICONASTERISK);
        }

        if (Herz > 100)
        {
            Herz = 20;
            SetWindowText(hEdtHerz, L"20");
            MessageBox(GetActiveWindow(), L"Введено число выше допустимого значения!", L"Недопустимое значение", MB_ICONASTERISK);
        }
        else if (Herz < 0)
        {
            Herz = 20;
            SetWindowText(hEdt, L"20");
            MessageBox(GetActiveWindow(), L"Введено число ниже допустимого значения!", L"Недопустимое значение", MB_ICONASTERISK);
        }

        break;
    }
    case WM_PAINT:
    {
        hdc = BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);

        graphic = BeginPaint(graph, &ps);
        DrawDiagram(graph, graphic, offset, Amplit, Herz);
        EndPaint(graph, &ps);

        hdcWrap = BeginPaint(graphWrap, &ps);
        EndPaint(graphWrap, &ps);

    }
    break;
    case WM_LBUTTONDOWN:
    {
        mousePressed = true;
        POINT p;
        GetCursorPos(&p);
        mouseOnWindowX = p.x - windowPositionX;
        mouseOnWindowY = p.y - windowPositionY;
        SetWindowPos(graph, HWND_TOP, windowPositionX, windowPositionY, 0, 0, SWP_NOSIZE);
        break;
    }
    case WM_LBUTTONUP:
    {
        mousePressed = false;
        break;
    }
    case WM_MOUSEMOVE:
    {

        if (mousePressed)
        {
            POINT p;
            GetCursorPos(&p);
            windowPositionX = p.x - mouseOnWindowX;
            windowPositionY = p.y - mouseOnWindowY;
            SetWindowPos(graph, HWND_TOP, windowPositionX, windowPositionY, 0, 0, SWP_NOSIZE);
            InvalidateRect(hWnd, 0, true);
            UpdateWindow(hWnd);
        }
        break;
    }
    case WM_MOUSEWHEEL:

        RECT rc;

        //rect.right / 100, rect.bottom / 100

        GetWindowRect(graph, &rc);
        GetClientRect(graphWrap, &rect);

        if (((short)HIWORD(wParam)) < 0)
        {
            zoom += 5;
            SetWindowPos(graph, nullptr, rect.right / 100, rect.bottom / 100, (rect.right / 1.1) - (zoom * 4.9), (rect.bottom / 1.4) - (zoom * 2),/*Height, Width*/ SWP_NOZORDER);
            InvalidateRect(hWnd, 0, true);
            UpdateWindow(hWnd);
        }
        else
        {
            zoom -= 5;
            SetWindowPos(graph, nullptr, rect.right / 100, rect.bottom / 100, (rect.right / 1.1) - (zoom * 4.9), (rect.bottom / 1.4) - (zoom * 2),/*Height + zoom, Width + zoom */ SWP_NOZORDER);
            InvalidateRect(hWnd, 0, true);
            UpdateWindow(hWnd);
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

void DrawDiagram(HWND graph, HDC hdc, int offset, int Amplitude, int Herz)
{
    RECT rect;
    GetClientRect(graph, &rect);
    const int xVE = rect.right - rect.left;
    const int yVE = rect.bottom - rect.top;
    const int xWE = xVE;
    const int yWE = yVE;
    double nPixPerVolt = yVE / 1000.0;
    double nPixPerMs = xVE / 60.0;

    SetMapMode(hdc, MM_ISOTROPIC);
    SetWindowExtEx(hdc, xWE, yWE, NULL);
    SetViewportExtEx(hdc, xVE, -yVE, NULL);
    SetViewportOrgEx(hdc, 4 * nPixPerMs, yVE / 2, NULL);

    const int tMin = 0;
    const int tMax = 50;
    const int uMin = -1000;
    const int uMax = 1000;
    const int tGridStep = 5;
    const int uGridStep = 100;
    int x, y;

    const wchar_t *xMark[] = { L"0", L"10", L"20", L"30", L"40", L"50", L"60", L"70", L"80", L"90", L"100" };
    const wchar_t *yMark[] = { L"-1000",L"-900",L"-800",L"-700",L"-600",L"-500",L"-400", L"-300", L"-200", L"-100", L"0", L"100", L"200", L"300", L"400", L"500", L"600", L"700", L"800", L"900", L"1000"};

    HPEN hPen0 = CreatePen(PS_SOLID, 1, RGB(0, 160, 0));
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen0);

    int u = uMin;
    int xMin = tMin * nPixPerMs;
    int xMax = tMax * nPixPerMs;

    for (int i = 0; i < 21; ++i) {
        y = u * nPixPerVolt;
        MoveToEx(hdc, xMin, y, NULL);
        LineTo(hdc, xMax, y);
        TextOutW(hdc, xMin - 35, y + 4, (LPCWSTR)yMark[i], wcslen(yMark[i]));
        u += uGridStep;
    }

    int t = tMin;
    int yMin = uMin * nPixPerVolt;
    int yMax = uMax * nPixPerVolt;

    for (int a = 0; a < 11; ++a) {
        x = t * nPixPerMs;
        MoveToEx(hdc, x, yMin, NULL);
        LineTo(hdc, x, yMax); 
        TextOut(hdc, x, yMin - 10, (LPCWSTR)xMark[a], wcslen(xMark[a]));
        t += tGridStep;
    }

    HPEN hPen1 = CreatePen(PS_SOLID, 3, RGB(0, 0, 0));
    SelectObject(hdc, hPen1);

    MoveToEx(hdc, 0, 0, NULL); LineTo(hdc, xMax, 0);
    MoveToEx(hdc, 0, yMin, NULL); LineTo(hdc, 0, yMax);

    HPEN hPen2 = CreatePen(PS_SOLID, 5, RGB(25, 25, 255));
    SelectObject(hdc, hPen2);
    int tStep = 1;
    double radianPerx = 2 * (Pi / Herz);
    const double uAmplit = Amplitude;
    t = tMin * 200;
    MoveToEx(hdc, 0, ((uAmplit * sin(t * radianPerx - offset)) * nPixPerVolt ), NULL);
    while (t <= tMax) { 
        u = uAmplit * sin(t * radianPerx - offset); 
        LineTo(hdc, t * nPixPerMs, u * nPixPerVolt);
        t += tStep;
    }
    SelectObject(hdc, hOldPen);
}

// Обработчик сообщений для окна "О программе".
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
