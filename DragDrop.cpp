#include "DragDrop.h"
#include <shellapi.h>

namespace App {

    void HandleDrop(HWND hWnd, HDROP hDrop, HWND hEdit1, HWND hEdit2) {
        UINT count = DragQueryFileW(hDrop, 0xFFFFFFFF, nullptr, 0);
        if (count > 0) {
            WCHAR path[MAX_PATH];
            DragQueryFileW(hDrop, 0, path, MAX_PATH);

            POINT pt;
            if (!DragQueryPoint(hDrop, &pt)) {
                SetWindowTextW(hEdit1, path);
            }
            else {
                HWND hTarget = ChildWindowFromPoint(hWnd, pt);
                if (hTarget == hEdit1) SetWindowTextW(hEdit1, path);
                else if (hTarget == hEdit2) SetWindowTextW(hEdit2, path);
            }
        }
        DragFinish(hDrop);
    }

}
