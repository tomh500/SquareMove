// Controls.h
#pragma once

// --- UI 控件 ID ---
#define ID_INPUT1_EDIT      2001 // 源路径 (文件或目录)
#define ID_INPUT2_EDIT      2002 // 目标路径 (目录)
#define ID_SELECT1_BUTTON   2003
#define ID_SELECT2_BUTTON   2004

// --- 布局常量 ---
namespace UI {
    constexpr int WINDOW_WIDTH = 350;
    constexpr int WINDOW_HEIGHT = 200;
    constexpr int MARGIN = 10;
    constexpr int EDIT_HEIGHT = 26;
    constexpr int BUTTON_WIDTH = 100;
}