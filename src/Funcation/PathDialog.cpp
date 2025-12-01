#include "PathDialog.h"
#include <ShObjIdl.h>
#include <filesystem>
#include <string>
#include <comdef.h> 

#pragma comment(lib, "Ole32.lib")
#pragma comment(lib, "Shlwapi.lib")
using namespace std;
namespace App {

    wstring OpenFile(HWND hWnd)
    {
        wstring result;

        IFileOpenDialog* pDialog = nullptr;
        if (SUCCEEDED(CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_ALL, IID_PPV_ARGS(&pDialog))))
        {
            DWORD dwOptions;
            if (SUCCEEDED(pDialog->GetOptions(&dwOptions)))
            {
                pDialog->SetOptions(dwOptions |
                    FOS_FORCEFILESYSTEM |
                    FOS_PATHMUSTEXIST |
                    FOS_FILEMUSTEXIST |
                    FOS_ALLOWMULTISELECT); // 允许选择文件和文件夹
            }

            if (SUCCEEDED(pDialog->Show(hWnd)))
            {
                IShellItem* pItem = nullptr;
                if (SUCCEEDED(pDialog->GetResult(&pItem)))
                {
                    PWSTR pszPath = nullptr;
                    if (SUCCEEDED(pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszPath)))
                    {
                        result = pszPath;
                        CoTaskMemFree(pszPath);
                    }
                    pItem->Release();
                }
            }

            pDialog->Release();
        }

        return result;
    }




    wstring OpenFolder(HWND hWnd)
    {
        wstring result;
        IFileOpenDialog* dlg = nullptr;
        CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_PPV_ARGS(&dlg));

        if (dlg)
        {
            DWORD options;
            dlg->GetOptions(&options);
            dlg->SetOptions(options | FOS_PICKFOLDERS);

            if (SUCCEEDED(dlg->Show(hWnd)))
            {
                IShellItem* item = nullptr;
                if (SUCCEEDED(dlg->GetResult(&item)))
                {
                    PWSTR path;
                    if (SUCCEEDED(item->GetDisplayName(SIGDN_FILESYSPATH, &path)))
                    {
                        result = path;
                        CoTaskMemFree(path);
                    }
                    item->Release();
                }
            }
            dlg->Release();
        }
        return result;
    }

}
