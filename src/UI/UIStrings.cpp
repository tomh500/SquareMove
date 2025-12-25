#include "UIStrings.h"
#include "Resource.h"
#include <vector>
#include <algorithm>
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
                zw
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

        // 根据语言确定 Section 名字
        std::wstring sec = (g_currentLang == LANG_ZH_CN) ? L"ZH" : L"EN";
        g_strings.title = ParseIni(langData, sec, L"Title");
        g_strings.SelectPath_Button = ParseIni(langData, sec, L"SelectPathButton");
        g_strings.SelectFile_Button = ParseIni(langData, sec, L"SelectFileButton");
        g_strings.MoveButton = ParseIni(langData, sec, L"MoveButton");
        g_strings.MoveFailed = ParseIni(langData, sec, L"MoveFailed");
        g_strings.UndoConfirm = ParseIni(langData, sec, L"UndoConfirm");
        g_strings.RequireAdmin = ParseIni(langData, sec, L"RequireAdmin");
        g_strings.Menu_File = ParseIni(langData, sec, L"Menu_File");
        g_strings.Menu_Help = ParseIni(langData, sec, L"Menu_Help");
        g_strings.Menu_About = ParseIni(langData, sec, L"Menu_About");
        g_strings.Menu_Exit = ParseIni(langData, sec, L"Menu_Exit");
        g_strings.Menu_FastMode = ParseIni(langData, sec, L"Menu_FastMode");
        g_strings.Menu_HideOrigin = ParseIni(langData, sec, L"Menu_HideOrigin");
        g_strings.MoveSuccess = ParseIni(langData, sec, L"MoveSuccess");
        g_strings.FastModeWarning = ParseIni(langData, sec, L"FastModeWarning");
        g_strings.CreateQuick = ParseIni(langData, sec, L"CreateQuick");

        // 默认值兜底
        if (g_strings.title.empty()) {
            g_strings.title = (g_currentLang == LANG_ZH_CN) ? L"无损平方移动工具" : L"FreeMoveSQ";
            std::wstring warnMsg = (g_currentLang == LANG_ZH_CN) ? L"语言资源加载失败" : L"Failed to load language resources";
            MessageBoxW(NULL, warnMsg.c_str(), g_strings.title.c_str(), MB_OK | MB_ICONWARNING);
        }
    }
}