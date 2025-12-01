#include "UIStrings.h"
#include "Resource.h"
#include <vector>
#define zw return LANG_ZH_CN;
#define zw1 return LANG_EN;

namespace App {

    Language g_currentLang = LANG_EN;
    Strings g_strings;

    // 判断系统语言
    Language DetectLanguage() {
        WCHAR localeName[LOCALE_NAME_MAX_LENGTH] = { 0 };
        if (::GetUserDefaultLocaleName(localeName, LOCALE_NAME_MAX_LENGTH)) {
            std::wstring locale(localeName);
            if (locale.find(L"zh-CN") == 0 ||
                locale.find(L"zh-HK") == 0 ||
                locale.find(L"zh-TW") == 0 ||
                locale.find(L"zh-MO") == 0) {
                zw1
            }
        }
        return LANG_EN;
    }

    // 读取资源 ini 到内存
    std::wstring LoadLangResource(HINSTANCE hInst) {
        HRSRC hRes = FindResourceW(hInst, MAKEINTRESOURCE(IDR_LANG_INI), RT_RCDATA);
        if (!hRes) return L"";

        HGLOBAL hData = LoadResource(hInst, hRes);
        if (!hData) return L"";

        DWORD size = SizeofResource(hInst, hRes);
        if (!size) return L"";

        const char* pData = static_cast<const char*>(LockResource(hData));
        if (!pData) return L"";

        // 简单转换成 wstring (UTF-8 -> UTF-16)
        int wlen = MultiByteToWideChar(CP_UTF8, 0, pData, size, nullptr, 0);
        std::wstring result(wlen, 0);
        MultiByteToWideChar(CP_UTF8, 0, pData, size, &result[0], wlen);

        return result;
    }

    // 解析简单 ini 格式
    std::wstring ParseIni(const std::wstring& data, const std::wstring& section, const std::wstring& key) {
        std::wstring searchSection = L"[" + section + L"]";
        size_t secPos = data.find(searchSection);
        if (secPos == std::wstring::npos) return L"";

        size_t nextSec = data.find(L"[", secPos + 1);
        size_t endPos = (nextSec == std::wstring::npos) ? data.size() : nextSec;

        std::wstring sectionData = data.substr(secPos, endPos - secPos);

        std::wstring searchKey = key + L"=";
        size_t keyPos = sectionData.find(searchKey);
        if (keyPos == std::wstring::npos) return L"";

        size_t lineEnd = sectionData.find(L"\n", keyPos);
        if (lineEnd == std::wstring::npos) lineEnd = sectionData.size();

        std::wstring value = sectionData.substr(keyPos + searchKey.size(), lineEnd - keyPos - searchKey.size());
        value.erase(std::remove(value.begin(), value.end(), L'\r'), value.end());
        value.erase(0, value.find_first_not_of(L" \t"));
        value.erase(value.find_last_not_of(L" \t") + 1);

        return value;
    }

    void LoadStrings(HINSTANCE hInstance) {
        g_currentLang = DetectLanguage();
        std::wstring langData = LoadLangResource(hInstance);

        //新增了字符串 需要在这里修改

        if (g_currentLang == LANG_ZH_CN) {
            g_strings.title = ParseIni(langData, L"ZH", L"Title");
            g_strings.SelectPath_Button = ParseIni(langData, L"ZH", L"SelectPathButton");
            g_strings.SelectFile_Button = ParseIni(langData, L"ZH", L"SelectFileButton");
            g_strings.MoveButton = ParseIni(langData, L"ZH", L"MoveButton");
            g_strings.MoveFailed = ParseIni(langData, L"ZH", L"MoveFailed");
            g_strings.UndoConfirm = ParseIni(langData, L"ZH", L"UndoConfirm");
            g_strings.RequireAdmin = ParseIni(langData, L"ZH", L"RequireAdmin");
            g_strings.Menu_FastMode = ParseIni(langData, L"ZH", L"FastMode");
            g_strings.Menu_HideOrigin = ParseIni(langData, L"ZH", L"HideOrigin");
            g_strings.Menu_File = ParseIni(langData, L"ZH", L"Menu_File");
            g_strings.Menu_Help = ParseIni(langData, L"ZH", L"Menu_Help");
            g_strings.Menu_About = ParseIni(langData, L"ZH", L"Menu_About");
            g_strings.Menu_Exit = ParseIni(langData, L"ZH", L"Menu_Exit");
            g_strings.Menu_FastMode = ParseIni(langData, L"ZH", L"Menu_FastMode");
            g_strings.Menu_HideOrigin = ParseIni(langData, L"ZH", L"Menu_HideOrigin");
        }
        else {
            g_strings.title = ParseIni(langData, L"EN", L"Title");
            g_strings.SelectPath_Button = ParseIni(langData, L"EN", L"SelectPathButton");
            g_strings.SelectFile_Button = ParseIni(langData, L"EN", L"SelectFileButton");
            g_strings.MoveButton = ParseIni(langData, L"EN", L"MoveButton");
            g_strings.MoveFailed = ParseIni(langData, L"EN", L"MoveFailed");
            g_strings.UndoConfirm = ParseIni(langData, L"EN", L"UndoConfirm");
            g_strings.RequireAdmin = ParseIni(langData, L"EN", L"RequireAdmin");
            g_strings.Menu_FastMode = ParseIni(langData, L"EN", L"FastMode");
            g_strings.Menu_HideOrigin = ParseIni(langData, L"EN", L"HideOrigin");
            g_strings.Menu_File = ParseIni(langData, L"EN", L"Menu_File");
            g_strings.Menu_Help = ParseIni(langData, L"EN", L"Menu_Help");
            g_strings.Menu_About = ParseIni(langData, L"EN", L"Menu_About");
            g_strings.Menu_Exit = ParseIni(langData, L"EN", L"Menu_Exit");
            g_strings.Menu_FastMode = ParseIni(langData, L"EN", L"Menu_FastMode");
            g_strings.Menu_HideOrigin = ParseIni(langData, L"EN", L"Menu_HideOrigin");
        }


        // 如果解析失败，提供默认值
        if (g_strings.title.empty())
        {
             g_strings.title = (g_currentLang == LANG_ZH_CN) ? L"无损平方移动工具（解析失败）" : L"FreeMoveSQ";
			 MessageBoxW(NULL, (g_currentLang == LANG_ZH_CN) ? L"语言资源加载失败。" : L"Failed to load language resources, using default strings.", g_strings.title.c_str(), MB_OK | MB_ICONWARNING);
        }
           
        /*
        if (g_strings.SelectPath_Button.empty())
            g_strings.SelectPath_Button = (g_currentLang == LANG_ZH_CN) ? L"选择路径" : L"Select Path";

        if (g_strings.SelectFile_Button.empty())
            g_strings.SelectFile_Button = (g_currentLang == LANG_ZH_CN) ? L"选择文件" : L"Select File";

        if (g_strings.MoveButton.empty())
            g_strings.MoveButton = (g_currentLang == LANG_ZH_CN) ? L"移动" : L"Move";

        if (g_strings.MoveFailed.empty())
            g_strings.MoveFailed = (g_currentLang == LANG_ZH_CN) ? L"移动失败" : L"Move Failed";

        if (g_strings.UndoConfirm.empty())
            g_strings.UndoConfirm = (g_currentLang == LANG_ZH_CN) ? L"是否撤销更改？" : L"Undo changes?";

        if (g_strings.RequireAdmin.empty())
            g_strings.RequireAdmin = (g_currentLang == LANG_ZH_CN) ? L"点击以管理员身份运行" : L"Require Admin Permission";

        if (g_strings.Menu_FastMode.empty())
            g_strings.Menu_FastMode = (g_currentLang == LANG_ZH_CN) ? L"快速模式" : L"Fast Mode";
        if (g_strings.Menu_HideOrigin.empty())
            g_strings.Menu_HideOrigin = (g_currentLang == LANG_ZH_CN) ? L"将原先目录隐藏" : L"Set original path to hidden";

            */
    }
}
