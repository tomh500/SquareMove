// Localization.h
#pragma once
#include <string>
#include <Windows.h> // 【修正】确保 HINSTANCE 和 LoadStringW 等 Windows API 类型可用
#include "Resource.h"

namespace Localization {

    enum Language { LANG_ZH_CN, LANG_EN };

    struct Strings {
        std::wstring title;
        std::wstring select_file_dir_btn;
        std::wstring select_dir_only_btn;
        std::wstring default_placeholder_src;
        std::wstring default_placeholder_dst;
    };

    // 唯一实例 (确保 GetStrings 是声明)
    const Strings& GetStrings();

    // 初始化并加载字符串 (确保 Initialize 是声明)
    void Initialize(HINSTANCE hInstance); // 【修正】确保参数类型正确声明

    // 内部函数
    Language DetectLanguage();

} // namespace Localization