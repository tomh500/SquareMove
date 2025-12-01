#pragma once
#include <windows.h>
#include <string>
using namespace std;
namespace App {


    enum Language { LANG_ZH_CN, LANG_EN };

   
    struct Strings {
        wstring title;
        wstring SelectPath_Button;
		wstring SelectFile_Button;
        wstring MoveButton;      
        wstring MoveFailed;      
        wstring UndoConfirm;     
        wstring RequireAdmin;   
    };


    extern Language g_currentLang;
    extern Strings g_strings;

    void LoadStrings(HINSTANCE hInstance);

}
