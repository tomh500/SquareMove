    #include "App.h"
    #include "UIStrings.h"
    #include "DragDrop.h"
    #include "PathDialog.h"
    #include "AboutDlg.h"
    #include "Resource.h"
    #include "FileTools.h" 
    #include "Flag.h"
    namespace ft = FileTools;

    namespace App {

        CApp g_app;

        ATOM CApp::Register(HINSTANCE h, const WCHAR* cls)
        {
            WNDCLASSEXW wc{};
            wc.cbSize = sizeof(wc);
            wc.lpfnWndProc = WndProc;
            wc.hInstance = h;
            wc.lpszClassName = cls;
            wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
            wc.hIcon = LoadIcon(h, MAKEINTRESOURCE(IDI_FREEMOVESQUARE));
            wc.hIconSm = LoadIcon(h, MAKEINTRESOURCE(IDI_SMALL));
            wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
            wc.lpszMenuName = MAKEINTRESOURCE(IDC_FREEMOVESQUARE);
            return RegisterClassExW(&wc);
        }

        BOOL CApp::Init(HINSTANCE h, int cmd)
        {
            hInst = h;
            RECT rc = { 0, 0, 700, 180 }; // 高度增加，容纳 Move 按钮
            AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, TRUE);

            HWND hWnd = CreateWindowW(szWindowClass,
                g_strings.title.c_str(),
                WS_OVERLAPPEDWINDOW,
                CW_USEDEFAULT, 0,
                rc.right - rc.left,
                rc.bottom - rc.top,
                nullptr, nullptr, hInst, nullptr);

            ShowWindow(hWnd, cmd);
            UpdateWindow(hWnd);
            return TRUE;
        }

        LRESULT CALLBACK CApp::WndProc(HWND hWnd, UINT msg, WPARAM w, LPARAM l)
        {
            switch (msg)
            {
            case WM_CREATE:
            {
                DragAcceptFiles(hWnd, TRUE);
                DragAcceptFiles(hWnd, TRUE);

                const int margin = 10;
                const int editHeight = 24;
                const int btnWidth = 90;
                const int editWidth = 600 - margin * 3 - btnWidth;
                const int moveBtnWidth = 100;
                const int warningIconSize = 48; 
                const int warningMarginX = 20; 

                // 第一行: 文件或文件夹选择
                g_app.hEdit1 = CreateWindowW(L"EDIT", L"",
                    WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
                    margin, margin, editWidth, editHeight,
                    hWnd, (HMENU)ID_INPUT1_EDIT, g_app.hInst, nullptr);

                g_app.hBtn1A = CreateWindowW(L"BUTTON", g_strings.SelectPath_Button.c_str(),
                    WS_CHILD | WS_VISIBLE,
                    margin + editWidth + margin, margin, btnWidth, editHeight,
                    hWnd, (HMENU)ID_SELECT1A_BUTTON, g_app.hInst, nullptr);

                g_app.hBtn1B = CreateWindowW(L"BUTTON", g_strings.SelectFile_Button.c_str(),
                    WS_CHILD | WS_VISIBLE,
                    margin + editWidth + margin+ btnWidth, margin, btnWidth, editHeight,
                    hWnd, (HMENU)ID_SELECT1B_BUTTON, g_app.hInst, nullptr);

                g_app.hEdit2 = CreateWindowW(L"EDIT", L"",
                    WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
                    margin, margin + editHeight + margin, editWidth, editHeight,
                    hWnd, (HMENU)ID_INPUT2_EDIT, g_app.hInst, nullptr);

                g_app.hBtn2 = CreateWindowW(L"BUTTON", g_strings.SelectPath_Button.c_str(),
                    WS_CHILD | WS_VISIBLE,
                    margin + editWidth + margin, margin + editHeight + margin, btnWidth, editHeight,
                    hWnd, (HMENU)ID_SELECT2_BUTTON, g_app.hInst, nullptr);


                // Move 按钮
                g_app.hBtnMove = CreateWindowExW(
                    0,
                    L"BUTTON",
                    g_strings.MoveButton.c_str(),
                    WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                    margin, margin + 2 * (editHeight + margin),
                    moveBtnWidth, editHeight,
                    hWnd,
                    (HMENU)ID_MOVE_BUTTON,
                    g_app.hInst,
                    nullptr
                );

                // 检测管理员权限
                if (!IsUserAnAdmin()) {
                    // 警告按钮
                    g_app.hBtnAdminWarn = CreateWindowExW(
                        0,
                        L"BUTTON",
                        L"",
                        WS_CHILD | WS_VISIBLE | BS_ICON | BS_FLAT,
                        margin + moveBtnWidth + warningMarginX +10, //右侧偏移
                        margin + 2 * (editHeight + margin),
                        warningIconSize,
                        warningIconSize,
                        hWnd,
                        (HMENU)ID_ADMIN_WARN_BUTTON,
                        g_app.hInst,
                        nullptr
                    );

                    HICON hWarningIcon = LoadIconW(NULL, IDI_WARNING);
                    SendMessageW(g_app.hBtnAdminWarn, BM_SETIMAGE, IMAGE_ICON, (LPARAM)hWarningIcon);

                    // 警告文字
                    g_app.hTxtAdminWarn = CreateWindowExW(
                        0,
                        L"STATIC",
                        g_strings.RequireAdmin.c_str(),
                        WS_CHILD | WS_VISIBLE,
                        margin + moveBtnWidth + warningMarginX + warningIconSize + 20, // 向右偏移
                        margin + 2 * (editHeight + margin) + (warningIconSize - editHeight) / 2, // 垂直居中对齐
                        250, // 文字宽度增加
                        editHeight,
                        hWnd,
                        nullptr,
                        g_app.hInst,
                        nullptr
                    );
                }
        
            break;

           
            }

            case WM_DROPFILES:
                App::HandleDrop(hWnd, (HDROP)w, g_app.hEdit1, g_app.hEdit2);
                break;

            case WM_COMMAND:
            {
                int wmId = LOWORD(w);
                switch (wmId)
                {
                    // 顶部菜单
                case IDM_ABOUT:
                    DialogBox(g_app.hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, ::About);
                    break;
                case IDM_EXIT:
                    DestroyWindow(hWnd);
                    break;

                    // 按钮
                case ID_SELECT1A_BUTTON:
                {
                    SetWindowTextW(g_app.hEdit1, OpenFolder(hWnd).c_str());
                    break;
                }
                case ID_SELECT1B_BUTTON:
                {
                    SetWindowTextW(g_app.hEdit1, OpenFile(hWnd).c_str());
                    break;
                }
                case ID_SELECT2_BUTTON:
                {
                    SetWindowTextW(g_app.hEdit2, OpenFolder(hWnd).c_str());
                    break;
                }
                case ID_MOVE_BUTTON:
                {
                    WCHAR src[MAX_PATH], dst[MAX_PATH];
                    GetWindowTextW(g_app.hEdit1, src, MAX_PATH);
                    GetWindowTextW(g_app.hEdit2, dst, MAX_PATH);

                    if (wcslen(src) == 0 || wcslen(dst) == 0)
                        break;

                    auto res = ft::MoveWithLink(src, dst, HideOrigin);

                    if (!res.success) {
                        if(ft::blockmsg==0)
                        {
						
                        int ret = MessageBoxW(hWnd, g_strings.UndoConfirm.c_str(), res.message.c_str() ,  MB_YESNO | MB_ICONWARNING);
                        if (ret == IDYES) {
                            // 撤销操作：尝试复制回原位置
                            ft::CopyItem(std::wstring(dst) + L"\\" + std::filesystem::path(src).filename().wstring(), std::wstring(src));
                        }
                       }
                        else {
                            MessageBoxW(hWnd, res.message.c_str(), g_strings.MoveFailed.c_str(), MB_OK | MB_ICONERROR);
							ft::blockmsg = 0;
                        }
                    }
                    break;
                }

                case ID_ADMIN_WARN_BUTTON:
                {
                    WCHAR szPath[MAX_PATH];
                    GetModuleFileNameW(NULL, szPath, MAX_PATH);

                    SHELLEXECUTEINFOW sei{ sizeof(sei) };
                    sei.lpVerb = L"runas";
                    sei.lpFile = szPath;
                    sei.nShow = SW_SHOWNORMAL;

                    if (!ShellExecuteExW(&sei)) {
                        // 提权失败或用户选择了“否”
                        MessageBoxW(hWnd,
                            g_strings.RequireAdmin.c_str(),
                            g_strings.MoveFailed.c_str(),
                            MB_OK | MB_ICONERROR);
                    }
                    else {
                        // 成功提权，退出当前程序
                        PostQuitMessage(0);
                    }
                    break;
                }




                default:
                    return DefWindowProc(hWnd, msg, w, l);
                }
                break;
            }

            case WM_DESTROY:
                PostQuitMessage(0);
                break;
            }

            return DefWindowProc(hWnd, msg, w, l);
        }

    }
