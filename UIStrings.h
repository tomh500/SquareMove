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
        wstring Menu_FastMode;
        wstring Menu_HideOrigin;
        wstring Menu_File;
        wstring Menu_Help;
        wstring Menu_About;
        wstring Menu_Exit;
		wstring MoveSuccess;
        wstring FastModeWarning;

    };


    extern Language g_currentLang;
    extern Strings g_strings;

    void LoadStrings(HINSTANCE hInstance);

}
