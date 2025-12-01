#pragma once
#include <filesystem>
#include <string>

namespace FreeMove::Core {

    enum class MoveResult {
        Ok,
        SourceMissing,
        CopyFailed,
        DeleteFailed
    };

    struct MoveOptions {
        bool verify = true;
    };

    bool CopyRecursive(const std::filesystem::path&,
        const std::filesystem::path&,
        std::wstring& error);

    MoveResult SafeMove(const std::filesystem::path&,
        const std::filesystem::path&,
        std::wstring& error,
        const MoveOptions& opt);

}
