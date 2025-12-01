#include "FileTools.h"
#include <filesystem>
#include <Windows.h>
#include "Flag.h"

namespace FileTools {

    namespace fs = std::filesystem;

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

    MoveResult MoveWithLink(const std::wstring& src, const std::wstring& dstDir, bool hideOrigin) {
        MoveResult res{ false, L"" };
        fs::path srcPath(src);
        fs::path dstPath = fs::path(dstDir) / srcPath.filename();

        if (!CopyItem(src, dstDir)) {
            res.message = L"Copy failed";
            return res;
        }

        if (!RemoveItem(src)) {
            res.message = L"Remove original failed";
            return res;
        }

        std::wstring errMsg;
        if (!CreateLink(src, dstPath, fs::is_directory(dstPath), errMsg)) {
            res.message = errMsg;
            CopyItem(dstPath.wstring(), srcPath.parent_path().wstring()); // ³·Ïú
            return res;
        }

        if (hideOrigin) SetHidden(src);

        res.success = true;
        return res;
    }

}
