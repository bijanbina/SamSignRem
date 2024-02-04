#ifndef SR_TAR_H
#define SR_TAR_H

#include <windows.h>
#include "backend.h"

typedef struct MmApplication
{
    string shortcut_name;
    string exe_name;
    string exe_path;
    string exe_args;
    string working_dir;
    string win_title;
    DWORD pid = 0;
    string pname;
    int workspace;
    HWND hwnd = 0;
}MmApplication;

void sr_tar();

string mm_getLinkPath(string path);
string mm_getLinkPathUser(string path);
string mm_getLinkPathAll(string path);
HRESULT mm_ResolveIt(LPCSTR lnk_path, string *app);

#endif // SR_TAR_H
