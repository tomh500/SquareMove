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

        HFONT CreatePreferredFont(int height)
        {
            LOGFONT lf{};
            lf.lfHeight = height;
            lf.lfWeight = FW_NORMAL;
            lf.lfCharSet = DEFAULT_CHARSET;
            lf.lfQuality = CLEARTYPE_QUALITY;

            std::wstring fonts[] = { L"等线", L"微软雅黑", L"宋体" };

            for (auto& f : fonts) {
                wcscpy_s(lf.lfFaceName, f.c_str());
                HFONT hFont = CreateFontIndirect(&lf);
                if (hFont) return hFont;
            }

            return (HFONT)GetStockObject(DEFAULT_GUI_FONT);
        }

        void SetCtrlFont(HWND hCtrl, int height = 16)
        {
            HFONT hFont = CreatePreferredFont(height);
            SendMessage(hCtrl, WM_SETFONT, (WPARAM)hFont, TRUE);
        }


        std::wstring LoadAboutText(HINSTANCE hInst)
        {
            //MessageBoxW(NULL, L"LoadAboutText called", L"DEBUG", 0);
            HRSRC hRes = FindResourceW(hInst, MAKEINTRESOURCE(IDR_ABOUTTXT), RT_RCDATA);
            if (!hRes) {
                MessageBoxW(NULL, L"FindResource FAILED", L"ERR", 0);
                return L"";
            }

            HGLOBAL hData = LoadResource(hInst, hRes);
            if (!hData) return L"";

            DWORD size = SizeofResource(hInst, hRes);
            if (!size) return L"";

            const char* pData = static_cast<const char*>(LockResource(hData));
            if (!pData) return L"";

            int wlen = MultiByteToWideChar(CP_UTF8, 0, pData, size, nullptr, 0);
            std::wstring text(wlen, 0);
            MultiByteToWideChar(CP_UTF8, 0, pData, size, &text[0], wlen);

            return text;
        }

        INT_PTR CALLBACK AboutDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
        {
            switch (message)
            {
            case WM_INITDIALOG:
            {
                std::wstring aboutText = LoadAboutText((HINSTANCE)GetWindowLongPtr(hDlg, GWLP_HINSTANCE));
                SetDlgItemTextW(hDlg, IDC_ABOUT_TEXT, aboutText.c_str());

                HWND hText = GetDlgItem(hDlg, IDC_ABOUT_TEXT);

                HFONT hFont = (HFONT)SendMessage(hText, WM_GETFONT, 0, 0);
                HDC hdc = GetDC(hDlg);
                SelectObject(hdc, hFont);

                const int ICON_LEFT = 14;
                const int ICON_WIDTH = 21;
                const int ICON_GAP = 10;

                const int TEXT_LEFT = ICON_LEFT + ICON_WIDTH + ICON_GAP + 40;

                RECT rc = { 0, 0, 500, 0 };
                DrawTextW(hdc, aboutText.c_str(), -1, &rc, DT_CALCRECT | DT_WORDBREAK);

                ReleaseDC(hDlg, hdc);

                // 移动文本框
                MoveWindow(hText,
                    TEXT_LEFT,
                    14,
                    rc.right + 10,
                    rc.bottom + 10,
                    TRUE);


                // Resize 窗口
                MoveWindow(hDlg,
                    0,
                    0,
                    TEXT_LEFT + rc.right + 20,
                    rc.bottom + 60,
                    TRUE);



                return TRUE;
            }

            case WM_COMMAND:
                if (LOWORD(wParam) == IDCANCEL)   // 右上角 X
                {
                    EndDialog(hDlg, 0);
                    return TRUE;
                }
                break;

            case WM_CLOSE:   // 防止系统绕过 WM_COMMAND
                EndDialog(hDlg, 0);
                return TRUE;
            }
            return FALSE;
        }



        BOOL CApp::Init(HINSTANCE h, int cmd)
        {
            hInst = h;
            RECT rc = { 0, 0, 700, 120 }; // 高度增加，容纳 Move 按钮
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
                CheckMenuItem(GetMenu(hWnd), IDM_FASTMODE, MF_BYCOMMAND | (FastMode ? MF_CHECKED : MF_UNCHECKED));
                CheckMenuItem(GetMenu(hWnd), IDM_HIDEORIGIN, MF_BYCOMMAND | (HideOrigin ? MF_CHECKED : MF_UNCHECKED));
                ModifyMenuW(GetMenu(hWnd), IDM_FASTMODE, MF_BYCOMMAND | MF_STRING, IDM_FASTMODE, g_strings.Menu_FastMode.c_str());
                ModifyMenuW(GetMenu(hWnd), IDM_HIDEORIGIN, MF_BYCOMMAND | MF_STRING, IDM_HIDEORIGIN, g_strings.Menu_HideOrigin.c_str());
                ModifyMenuW(GetMenu(hWnd), IDM_EXIT, MF_BYCOMMAND | MF_STRING, IDM_EXIT, g_strings.Menu_Exit.c_str());
                ModifyMenuW(GetMenu(hWnd), IDM_ABOUT, MF_BYCOMMAND | MF_STRING, IDM_ABOUT, g_strings.Menu_About.c_str());

                HMENU hMenu = GetMenu(hWnd);
                HMENU hFileMenu = GetSubMenu(hMenu, 0);
                HMENU hHelpMenu = GetSubMenu(hMenu, 1);
                ModifyMenuW(hMenu, 0, MF_BYPOSITION | MF_STRING, 0, g_strings.Menu_File.c_str());
                ModifyMenuW(hMenu, 1, MF_BYPOSITION | MF_STRING, 1, g_strings.Menu_Help.c_str());



                const int margin = 10;
                const int editHeight = 24;
                const int btnWidth = 90;
                const int editWidth = 600 - margin * 3 - btnWidth;
                const int moveBtnWidth = 100;
                const int warningIconSize = 48; 
                const int warningMarginX = 20; 

                g_app.hEdit1 = CreateWindowW(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
                    margin, margin, editWidth, editHeight, hWnd, (HMENU)ID_INPUT1_EDIT, g_app.hInst, nullptr);
                SetCtrlFont(g_app.hEdit1, 16);

                g_app.hBtn1A = CreateWindowW(L"BUTTON", g_strings.SelectPath_Button.c_str(),
                    WS_CHILD | WS_VISIBLE, margin + editWidth + margin, margin, btnWidth, editHeight,
                    hWnd, (HMENU)ID_SELECT1A_BUTTON, g_app.hInst, nullptr);
                SetCtrlFont(g_app.hBtn1A, 16);

                g_app.hBtn1B = CreateWindowW(L"BUTTON", g_strings.SelectFile_Button.c_str(),
                    WS_CHILD | WS_VISIBLE, margin + editWidth + margin + btnWidth, margin, btnWidth, editHeight,
                    hWnd, (HMENU)ID_SELECT1B_BUTTON, g_app.hInst, nullptr);
                SetCtrlFont(g_app.hBtn1B, 16);

                g_app.hEdit2 = CreateWindowW(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
                    margin, margin + editHeight + margin, editWidth, editHeight, hWnd, (HMENU)ID_INPUT2_EDIT, g_app.hInst, nullptr);
                SetCtrlFont(g_app.hEdit2, 16);

                g_app.hBtn2 = CreateWindowW(L"BUTTON", g_strings.SelectPath_Button.c_str(),
                    WS_CHILD | WS_VISIBLE, margin + editWidth + margin, margin + editHeight + margin, btnWidth, editHeight,
                    hWnd, (HMENU)ID_SELECT2_BUTTON, g_app.hInst, nullptr);
                SetCtrlFont(g_app.hBtn2, 16);

                g_app.hBtnMove = CreateWindowExW(0, L"BUTTON", g_strings.MoveButton.c_str(),
                    WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, margin, margin + 2 * (editHeight + margin),
                    moveBtnWidth, editHeight, hWnd, (HMENU)ID_MOVE_BUTTON, g_app.hInst, nullptr);
                SetCtrlFont(g_app.hBtnMove, 16);

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
                    
                case IDM_ABOUT:
                    DialogBox(g_app.hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, AboutDlgProc);
                    break;

                case IDM_EXIT:
                    DestroyWindow(hWnd);
                    break;
                case IDM_FASTMODE:
                    if (FastMode==false && MessageBoxW(nullptr, g_strings.FastModeWarning.c_str(), g_strings.title.c_str(), MB_OK | MB_ICONINFORMATION)) {
                        FastMode = !FastMode; // 切换状态
                        CheckMenuItem(GetMenu(hWnd), IDM_FASTMODE, MF_BYCOMMAND | (FastMode ? MF_CHECKED : MF_UNCHECKED));
                    }
                    else {
                        FastMode = !FastMode; // 切换状态
                        CheckMenuItem(GetMenu(hWnd), IDM_FASTMODE, MF_BYCOMMAND | (FastMode ? MF_CHECKED : MF_UNCHECKED));
                    }

                    break;
                case IDM_HIDEORIGIN:
                    HideOrigin = !HideOrigin;
                    CheckMenuItem(GetMenu(hWnd), IDM_HIDEORIGIN, MF_BYCOMMAND | (HideOrigin ? MF_CHECKED : MF_UNCHECKED));
                    break;

                case IDM_GITHUB: {
                    const std::string url = "https://www.github.com/tomh500/FreeMove_Square";
                    std::string command;
                    #ifdef _WIN32
                    command = "start \"\" \"" + url + "\"";
                    #elif __APPLE__
                    command = "open \"" + url + "\"";
                    #else
                    command = "xdg-open \"" + url + "\"";
                    #endif

                    // --- 执行命令 ---
                    int result = std::system(command.c_str());

                    break;
                }


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
