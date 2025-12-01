#pragma once
#include <string>

namespace FileTools {

    struct MoveResult {
        bool success;
        std::wstring message;
    };

    MoveResult MoveWithLink(const std::wstring& src, const std::wstring& dstDir, bool hideOrigin);

    bool CopyItem(const std::wstring& src, const std::wstring& dstDir);
    bool RemoveItem(const std::wstring& path);
    bool CreateLink(const std::wstring& linkPath, const std::wstring& targetPath, bool isDirectory, std::wstring& errMsg);
    bool SetHidden(const std::wstring& path);

}
