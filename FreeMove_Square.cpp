#include "framework.h"
#include "App.h"
#include "UIStrings.h"
#include "Resource.h"
#include <objbase.h>   
#include <CommCtrl.h>   
#include "Flag.h"
#pragma comment(lib,"ole32.lib")
#pragma comment(lib,"comctl32.lib")

bool HideOrigin = false; 
bool FastMode = false;
int LocalVersion = 10000;

int APIENTRY wWinMain(HINSTANCE hInstance,
    HINSTANCE,
    LPWSTR,
    int nCmdShow)
{
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    INITCOMMONCONTROLSEX icc;
    icc.dwSize = sizeof(icc);
    icc.dwICC = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&icc);

    App::LoadStrings(hInstance);

    LoadStringW(hInstance, IDC_FREEMOVESQUARE,
        App::g_app.szWindowClass, 100);

    App::g_app.Register(hInstance,
        App::g_app.szWindowClass);

    App::g_app.Init(hInstance, nCmdShow);

    MSG msg{};
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    CoUninitialize();
    return (int)msg.wParam;
}
