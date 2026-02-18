#include <windows.h>
#include <stdio.h>
#include <tlhelp32.h>
#include "TCHAR.h"
#include "injector.h"

#define ID_RADIO1 1001
#define ID_RADIO2 1002
#define ID_RADIO3 1003



LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    SetPrivilege(SE_DEBUG_NAME, TRUE);

    DWORD PID = GetProcessID(L"Minesweeper.exe");

    switch (msg) {
    case WM_CREATE:
        CreateWindow(
            L"BUTTON", L"Timer Hack",
            WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON | WS_GROUP,
            20, 20, 150, 30,
            hwnd, (HMENU)ID_RADIO1, NULL, NULL
        );

        CreateWindow(
            L"BUTTON", L"Show Mine Hack",
            WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON,
            20, 60, 150, 30,
            hwnd, (HMENU)ID_RADIO2, NULL, NULL
        );

        CreateWindow(
            L"BUTTON", L"Infinite Life Hack",
            WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON,
            20, 100, 150, 30,
            hwnd, (HMENU)ID_RADIO3, NULL, NULL
        );

        break;


    case WM_COMMAND:

        switch (LOWORD(wParam)) {

        case ID_RADIO1:
            SendMessage(GetDlgItem(hwnd, ID_RADIO1), BM_SETCHECK, BST_CHECKED, 0);
            SendMessage(GetDlgItem(hwnd, ID_RADIO2), BM_SETCHECK, BST_UNCHECKED, 0);
            SendMessage(GetDlgItem(hwnd, ID_RADIO3), BM_SETCHECK, BST_UNCHECKED, 0);
            InjectDLL(PID, L"TimerHack.dll");
            break;

        case ID_RADIO2:
            SendMessage(GetDlgItem(hwnd, ID_RADIO1), BM_SETCHECK, BST_UNCHECKED, 0);
            SendMessage(GetDlgItem(hwnd, ID_RADIO2), BM_SETCHECK, BST_CHECKED, 0);
            SendMessage(GetDlgItem(hwnd, ID_RADIO3), BM_SETCHECK, BST_UNCHECKED, 0);
            InjectDLL(PID, L"ShowMineHack.dll");
            break;

        case ID_RADIO3:
            SendMessage(GetDlgItem(hwnd, ID_RADIO1), BM_SETCHECK, BST_UNCHECKED, 0);
            SendMessage(GetDlgItem(hwnd, ID_RADIO2), BM_SETCHECK, BST_UNCHECKED, 0);
            SendMessage(GetDlgItem(hwnd, ID_RADIO3), BM_SETCHECK, BST_CHECKED, 0);
            InjectDLL(PID, L"InfiniteLifeHack.dll");
            break;
        }
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

int WINAPI wWinMain(
    _In_ HINSTANCE hInst,
    _In_opt_ HINSTANCE,
    _In_ PWSTR,
    _In_ int nCmdShow
) {
    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInst;
    wc.lpszClassName = L"RadioExample";

    RegisterClass(&wc);

    HWND hwnd = CreateWindow(
        wc.lpszClassName, L"Minesweeper Hack",
        WS_OVERLAPPEDWINDOW,
        100, 100, 300, 200,
        NULL, NULL, hInst, NULL
    );

    ShowWindow(hwnd, nCmdShow);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}

