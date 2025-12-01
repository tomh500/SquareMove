#pragma once
#include <Windows.h>
#include <string>

namespace App {

    std::wstring OpenFile(HWND hWnd);
    std::wstring OpenFolder(HWND hWnd);

}
