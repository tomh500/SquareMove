#include <windows.h>
#include <shlobj.h>
#include <shlguid.h>
#include <string>
#include <filesystem>

namespace ShortcutLogic {
    // 内部使用的辅助函数：获取快捷方式应有的完整路径
    static std::wstring GetTargetLnkPath() {
        WCHAR szPath[MAX_PATH];
        // CSIDL_PROGRAMS 指向当前用户的“开始菜单/程序”
        if (SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_PROGRAMS, NULL, 0, szPath))) {
            return std::wstring(szPath) + L"\\FreeMove_Square.lnk";
        }
        return L"";
    }

    // --- 逻辑 A：供初始化时判断是否显示的函数 ---
    bool IsAlreadyCreated() {
        std::wstring path = GetTargetLnkPath();
        if (path.empty()) return false;
        return std::filesystem::exists(path);
    }

    // --- 逻辑 B：供按钮 case 调用的“一键创建”函数 ---
    bool CreateStartMenuShortcut() {
        std::wstring shortcutPath = GetTargetLnkPath();
        if (shortcutPath.empty()) return false;

        // 获取当前 exe 的完整路径
        WCHAR szExePath[MAX_PATH];
        GetModuleFileNameW(NULL, szExePath, MAX_PATH);

        // 初始化 COM（确保在同一个线程内）
        HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
        bool success = false;

        IShellLinkW* psl;
        hr = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLinkW, (LPVOID*)&psl);
        if (SUCCEEDED(hr)) {
            psl->SetPath(szExePath);
            psl->SetDescription(L"FreeMove Square Tool");
            // 设置起始位置为 exe 所在目录
            std::filesystem::path p(szExePath);
            psl->SetWorkingDirectory(p.parent_path().c_str());

            IPersistFile* ppf;
            hr = psl->QueryInterface(IID_IPersistFile, (LPVOID*)&ppf);
            if (SUCCEEDED(hr)) {
                hr = ppf->Save(shortcutPath.c_str(), TRUE);
                if (SUCCEEDED(hr)) success = true;
                ppf->Release();
            }
            psl->Release();
        }

        // 如果之前已经初始化过 COM，这里调用 Uninitialize 不会破坏全局
        CoUninitialize();
        return success;
    }
}