#pragma once
#include <Windows.h>
#include <string>

namespace FileTools {

    HWND ShowMoveProgress(HWND hParent, const std::wstring& title, size_t totalFiles);
    void UpdateMoveProgress(size_t done, size_t total);
    void CloseMoveProgress();

}
