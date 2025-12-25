#include "MoveProgressWnd.h"
#include <CommCtrl.h>
#include <string>

#pragma comment(lib, "Comctl32.lib")

namespace FileTools {
    static ULONGLONG g_lastUpdate = 0;
    static HWND g_hWnd = nullptr;
    static HWND g_hProgress = nullptr;
    static HWND g_hText = nullptr;

    HWND ShowMoveProgress(HWND hParent, const std::wstring& title, size_t totalFiles)
    {
        INITCOMMONCONTROLSEX icex = { sizeof(INITCOMMONCONTROLSEX), ICC_PROGRESS_CLASS };
        InitCommonControlsEx(&icex);

        g_hWnd = CreateWindowEx(0, WC_DIALOG, title.c_str(),
            WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
            CW_USEDEFAULT, CW_USEDEFAULT, 550, 120,
            hParent, nullptr, nullptr, nullptr);

        g_hProgress = CreateWindowEx(0, PROGRESS_CLASS, nullptr,
            WS_CHILD | WS_VISIBLE,
            20, 20, 500, 30,
            g_hWnd, nullptr, nullptr, nullptr);

        g_hText = CreateWindowEx(0, L"STATIC", L"0/0",
            WS_CHILD | WS_VISIBLE | SS_CENTER,
            20, 60, 500, 20,
            g_hWnd, nullptr, nullptr, nullptr);

        SendMessage(g_hProgress, PBM_SETRANGE, 0, MAKELPARAM(0, 100));
        ShowWindow(g_hWnd, SW_SHOW);
        UpdateWindow(g_hWnd);
        return g_hWnd;
    }

    void UpdateMoveProgress(size_t done, size_t total)
    {
        ULONGLONG now = GetTickCount64();
        if (now - g_lastUpdate < 100 && done > 0 && done < total) return;
        g_lastUpdate = now;

        if (!g_hProgress || !g_hText) return;
        int percent = 0;
        if (total > 0) {
            percent = static_cast<int>(100.0 * done / total);
        }

        SendMessage(g_hProgress, PBM_SETPOS, percent, 0);
        std::wstring text = std::to_wstring(done) + L" / " + std::to_wstring(total);
        SetWindowTextW(g_hText, text.c_str());
        MSG msg;
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                PostQuitMessage((int)msg.wParam);
                break;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    void CloseMoveProgress()
    {
        if (g_hWnd) {
            DestroyWindow(g_hWnd);
            g_hWnd = nullptr;
            g_hProgress = nullptr;
            g_hText = nullptr;
        }
    }

}
