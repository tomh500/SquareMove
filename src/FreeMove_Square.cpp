#include "framework.h"
#include "App.h"
#include "UIStrings.h"
#include "Resource.h"
#include <objbase.h>   
#include <CommCtrl.h> 
#include "Flag.h"
#include "FileTools.h"
#pragma comment(lib,"ole32.lib")
#pragma comment(lib,"comctl32.lib")

bool HideOrigin = false; 
bool FastMode = false;
int LocalVersion = 11000;

int APIENTRY wWinMain(HINSTANCE hInstance,
    HINSTANCE,
    LPWSTR,
    int nCmdShow)
{

    // --- 1. 命令行解析逻辑 ---
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);

    if (argc > 1) {
        // 如果有参数，尝试进入命令行模式
        // 约定格式：FreeMove.exe "源路径" "目标路径" [--fast] [--hide]
        std::wstring src, dst;
        bool cmdFast = false;
        bool cmdHide = false;

        for (int i = 1; i < argc; i++) {
            std::wstring arg = argv[i];
            if (arg == L"--fast") cmdFast = true;
            else if (arg == L"--hide") cmdHide = true;
            else if (src.empty()) src = arg;
            else if (dst.empty()) dst = arg;
        }

        if (!src.empty() && !dst.empty()) {
            // 附加到当前的控制台，以便能输出文字
            AttachConsole(ATTACH_PARENT_PROCESS);
            FILE* fp;
            freopen_s(&fp, "CONOUT$", "w", stdout);

            printf("\nStarting Move: %ls -> %ls\n", src.c_str(), dst.c_str());

            // 调用之前重构好的 FileTools 逻辑
            // 注意：这里需要设置全局变量 FastMode 等
            FastMode = cmdFast;
            auto res = FileTools::MoveWithLink(src, dst, cmdHide);

            if (res.success) {
                printf("Success!\n");
            }
            else {
                printf("Error: %ls\n", res.message.c_str());
            }

            LocalFree(argv);
            return 0; 
        }
    }
    LocalFree(argv);

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
