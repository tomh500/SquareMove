#include "AboutDlg.h"
#include "Resource.h"

INT_PTR CALLBACK About(HWND hDlg, UINT msg, WPARAM w, LPARAM l) {
    UNREFERENCED_PARAMETER(l);

    if (msg == WM_COMMAND) {
        if (LOWORD(w) == IDOK || LOWORD(w) == IDCANCEL) {
            EndDialog(hDlg, LOWORD(w));
            return TRUE;
        }
    }
    return FALSE;
}
