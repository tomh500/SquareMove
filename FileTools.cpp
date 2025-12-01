#include "FileTools.h"
#include <filesystem>
#include <Windows.h>
#include <thread>
#include <vector>
#include <fstream>
#include <functional>

#include "Flag.h"
#include "UIStrings.h" 
#include "MoveProgressWnd.h"
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



    // 递归复制目录，调用回调更新进度
    bool CopyItemWithProgress(const std::wstring& src, const std::wstring& dstDir,
        std::function<void(size_t done, size_t total)> progressCallback)
    {
        namespace fs = std::filesystem;
        try {
            fs::path srcPath(src);
            fs::path dstPath = fs::path(dstDir) / srcPath.filename();

            std::vector<fs::path> allFiles;

            // 递归统计文件
            for (auto& entry : fs::recursive_directory_iterator(srcPath)) {
                if (fs::is_regular_file(entry.path()))
                    allFiles.push_back(entry.path());
            }

            size_t total = allFiles.size();
            size_t done = 0;

            for (auto& file : allFiles) {
                fs::path relative = file.lexically_relative(srcPath);
                fs::path target = dstPath / relative;
                fs::create_directories(target.parent_path());

                // 分块复制文件，每块 1MB
                std::ifstream ifs(file, std::ios::binary);
                std::ofstream ofs(target, std::ios::binary);
                constexpr size_t bufSize = 1024 * 1024;
                std::unique_ptr<char[]> buffer(new char[bufSize]);

                while (ifs.read(buffer.get(), bufSize) || ifs.gcount() > 0) {
                    ofs.write(buffer.get(), ifs.gcount());
                    progressCallback(done, total); // 高频更新
                }


                done++;
                progressCallback(done, total); // 文件完成一次
            }

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

        // 1. 目标存在
        if (fs::exists(dstPath)) {
            res.message = (g_currentLang == LANG_ZH_CN) ?
                L"目标路径已存在同名文件或目录" :
                L"Target path already exists";
            blockmsg = 1;
            return res;
        }

        // 2. 目标是子路径
        if (IsSubPathOrSame(srcPath, dstPath)) {
            res.message = (g_currentLang == LANG_ZH_CN) ?
                L"目标路径为源路径自身或其子目录" :
                L"Target path is inside source path or same as source";
            blockmsg = 1;
            return res;
        }

        bool isFast = FastMode;

        // 3. 执行操作
        if (isFast) {
            // 快速模式：直接剪切
            try {
                fs::rename(srcPath, dstPath);
            }
            catch (...) {
                res.message = (g_currentLang == LANG_ZH_CN) ? L"剪切失败" : L"Move failed";
                return res;
            }
        }
        else {
            // 普通模式：完整复制 + 进度
            size_t totalFiles = 0;
            for (auto& entry : fs::recursive_directory_iterator(srcPath))
                if (fs::is_regular_file(entry.path())) totalFiles++;

            HWND hProgressWnd = ShowMoveProgress(nullptr, L"移动中...", totalFiles);
            size_t done = 0;

            if (!CopyItemWithProgress(src, dstDir, [&](size_t fileDone, size_t fileTotal) {
                done = fileDone;
                UpdateMoveProgress(done, totalFiles);
                })) {
                CloseMoveProgress();
                res.message = (g_currentLang == LANG_ZH_CN) ? L"复制失败" : L"Copy failed";
                return res;
            }
            CloseMoveProgress();
        }

        // 4. 删除原目录（只复制模式需要）
        if (!isFast) {
            if (!RemoveItem(src)) {
                res.message = (g_currentLang == LANG_ZH_CN) ? L"删除源文件/目录失败" : L"Remove original failed";
                blockmsg = 0;
                return res;
            }
        }

        // 5. 创建符号链接
        std::wstring errMsg;
        if (!CreateLink(src, dstPath.wstring(), fs::is_directory(dstPath), errMsg)) {
            res.message = errMsg;
            if (!isFast) {
                fs::path safeRestore = srcPath.parent_path() / (srcPath.filename().wstring() + L"_restore");
                CopyItem(dstPath.wstring(), safeRestore.wstring());
            }
            return res;
        }

        if (hideOrigin) SetHidden(src);

        res.success = true;

        // 6. 成功提示
        MessageBoxW(nullptr,
            (g_currentLang == LANG_ZH_CN) ? L"操作完成" : L"Operation completed",
            (g_currentLang == LANG_ZH_CN) ? L"完成" : L"Done",
            MB_OK | MB_ICONINFORMATION);

        return res;
    }




}
