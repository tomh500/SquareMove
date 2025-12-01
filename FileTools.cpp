#include "FileTools.h"
#include <filesystem>
#include <Windows.h>
#include "Flag.h"
#include "UIStrings.h" 
using namespace App;

int FileTools::blockmsg = 0;
namespace FileTools {

    namespace fs = std::filesystem;

    bool IsSubPathOrSame(const fs::path& base, const fs::path& target)
    {
        // 绝对路径
        fs::path absBase = fs::absolute(base);
        fs::path absTarget = fs::absolute(target);

        // 完全相等
        if (absBase == absTarget) return true;

        // 判断是否是子路径
        auto baseIt = absBase.begin();
        auto targetIt = absTarget.begin();

        while (baseIt != absBase.end() && targetIt != absTarget.end()) {
            if (*baseIt != *targetIt) return false;
            ++baseIt;
            ++targetIt;
        }

        return baseIt == absBase.end(); // base 完全匹配 target 的前缀
    }

    bool CopyItem(const std::wstring& src, const std::wstring& dstDir) {
        try {
            fs::path srcPath(src);
            fs::path dstPath = fs::path(dstDir) / srcPath.filename();

            if (fs::is_directory(srcPath))
                fs::copy(srcPath, dstPath, fs::copy_options::recursive | fs::copy_options::overwrite_existing);
            else
                fs::copy_file(srcPath, dstPath, fs::copy_options::overwrite_existing);

            return true;
        }
        catch (...) { return false; }
    }

    bool RemoveItem(const std::wstring& path) {
        try {
            fs::path p(path);
            if (fs::is_directory(p)) fs::remove_all(p);
            else fs::remove(p);
            return true;
        }
        catch (...) { return false; }
    }

    bool CreateLink(const std::wstring& linkPath, const std::wstring& targetPath, bool isDirectory, std::wstring& errMsg) {
        DWORD flags = isDirectory ? SYMBOLIC_LINK_FLAG_DIRECTORY : 0;
        if (!CreateSymbolicLinkW(linkPath.c_str(), targetPath.c_str(), flags)) {
            errMsg = L"CreateSymbolicLink failed, error code: " + std::to_wstring(GetLastError());
            return false;
        }
        return true;
    }

    bool SetHidden(const std::wstring& path) {
        DWORD attrs = GetFileAttributesW(path.c_str());
        if (attrs == INVALID_FILE_ATTRIBUTES) return false;
        attrs |= FILE_ATTRIBUTE_HIDDEN;
        return SetFileAttributesW(path.c_str(), attrs) != 0;
    }

    MoveResult MoveWithLink(const std::wstring& src, const std::wstring& dstDir, bool hideOrigin)
    {
        MoveResult res{ false, L"" };
        fs::path srcPath(src);
        fs::path dstPath = fs::path(dstDir) / srcPath.filename();

        // 1. 检查目标是否存在同名文件夹或文件
        if (fs::exists(dstPath)) {
            res.message = (g_currentLang == LANG_ZH_CN) ?
                L"目标路径已存在同名文件或目录" :
                L"Target path already exists";
            blockmsg = 1;
            return res;
        }

        // 2. 检查目标路径是否为源路径自身或子路径
        if (IsSubPathOrSame(srcPath, dstPath)) {
            res.message = (g_currentLang == LANG_ZH_CN) ?
                L"目标路径为源路径自身或其子目录" :
                L"Target path is inside source path or same as source";
            blockmsg = 1;
            return res;
        }

        // 3. 先复制
        if (!CopyItem(src, dstDir)) {
            res.message = (g_currentLang == LANG_ZH_CN) ? L"复制失败" : L"Copy failed";
            blockmsg = 0;
            return res;
        }

        // 4. 删除源文件/目录
        if (!RemoveItem(src)) {
            res.message = (g_currentLang == LANG_ZH_CN) ? L"删除源文件/目录失败" : L"Remove original failed";
            blockmsg = 0;
            return res;
        }

        // 5. 创建符号链接
        std::wstring errMsg;
        if (!CreateLink(src, dstPath, fs::is_directory(dstPath), errMsg)) {
            res.message = errMsg;
            // 撤销：安全复制回源目录
            fs::path safeRestore = srcPath.parent_path() / (srcPath.filename().wstring() + L"_restore");
            CopyItem(dstPath.wstring(), safeRestore.wstring());
            return res;
        }

        if (hideOrigin) SetHidden(src);

        res.success = true;
        return res;
    }


}
