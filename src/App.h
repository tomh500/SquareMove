#pragma once
#include <Windows.h>
#include <shellapi.h> 
#include <ShlObj.h>   
#include <filesystem>
#include <string>
namespace fs = std::filesystem;

namespace App {

    class CApp {
    public:
        HINSTANCE hInst{};
        WCHAR szWindowClass[100]{};
        HWND hBtnMove = nullptr; 
        HWND hBtnAdmin = nullptr;
        HWND hStaticAdmin = nullptr;
        HWND hBtnAdminWarn = nullptr;
        HWND hTxtAdminWarn = nullptr;
        HWND hEdit1{}, hEdit2{};
        HWND hBtn1A{}, hBtn1B{}, hBtn2{};

        static ATOM Register(HINSTANCE, const WCHAR*);
        BOOL Init(HINSTANCE, int);
        static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
    };

    extern CApp g_app;

}
