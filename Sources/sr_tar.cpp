#include "sr_tar.h"
#include <shobjidl.h>
#include <shlguid.h>
#include <psapi.h>
#include <locale>
#include <codecvt>

void sr_tar()
{
    CoInitializeEx(NULL, COINIT_MULTITHREADED);

    string exe_path = mm_getLinkPath("7-Zip\\7-Zip File Manager.lnk");
    int path_len = exe_path.size();
    string path_7z = exe_path.substr(0, path_len-6);
    path_7z += exe_path.substr(path_len-4, 4);

    string path = sr_getCurrentPath();
    string dir = sc_getLastDirectoryName(path);
    path += "\\tar";
    SetCurrentDirectoryA(path.c_str());
    string cmd = "\"" + path_7z + "\" a -- " + dir + ".tar *.lz4";
    system(cmd.c_str());
}

string mm_getLinkPath(string path)
{
    string ret;
    ret = mm_getLinkPathUser(path);
    if( ret.size()==0 )
    {
        ret = mm_getLinkPathAll(path);
        if( ret.size()==0 )
        {
            cout << "Error 24: cannot find shortcut "
                 << path << endl;
            return ret;
        }
    }
    return ret;
}

string mm_getLinkPathUser(string path)
{
    string ret;
    string lnk = getenv("APPDATA");
    lnk += "\\Microsoft\\Windows\\Start Menu\\Programs\\";
    lnk += path;

    fstream img_file(lnk);
    if( !img_file.good() )
    {
        return ret;
    }

    mm_ResolveIt(lnk.c_str(), &ret);
    return ret;
}

//retreive link from ProgramData instead of user account
string mm_getLinkPathAll(string path)
{
    string ret;

    string lnk = getenv("PROGRAMDATA");
    lnk += "\\Microsoft\\Windows\\Start Menu\\Programs\\";
    lnk += path;

    mm_ResolveIt(lnk.c_str(), &ret);
    return ret;
}

HRESULT mm_ResolveIt(LPCSTR lnk_path, string *app)
{
    HRESULT hr;
    IShellLink* psl;
    WCHAR szTargetPath[MAX_PATH];
    WCHAR szArgs[MAX_PATH];
    WCHAR szDirPath[MAX_PATH];
    WCHAR szDescription[MAX_PATH];
    WIN32_FIND_DATA wfd;

    *app = ""; // Assume failure

    // Get a pointer to the IShellLink interface. It is assumed that CoInitialize
    // has already been called.
    hr = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER,
                            IID_IShellLink, (LPVOID*)&psl);
    if( hr )
    {
        cout << "IID_IShellLink Failed " << hr << endl;
        return hr;
    }

    IPersistFile* ppf;

    // Get a pointer to the IPersistFile interface.
    hr = psl->QueryInterface(IID_IPersistFile, (void**)&ppf);
    if( hr )
    {
        cout << "IID_IPersistFile Failed " << hr << endl;
        return hr;
    }

    WCHAR wsz[MAX_PATH];

    // Ensure that the string is Unicode.
    MultiByteToWideChar(CP_ACP, 0, lnk_path, -1, wsz, MAX_PATH);

    // Add code here to check return value from MultiByteWideChar
    // for success.

    // Load the shortcut.
    hr = ppf->Load(wsz, STGM_READ);
    if( hr )
    {
        cout << "ppf->Load Failed " << hr << endl;
        return hr;
    }

    // Resolve the link.
    HWND hwnd = GetActiveWindow();
    hr = psl->Resolve(hwnd, 0);

    if( SUCCEEDED(hr) )
    {
        // Get the path to the link target.
        hr = psl->GetPath(szTargetPath, MAX_PATH, (WIN32_FIND_DATA*)&wfd, SLGP_SHORTPATH);
        hr = psl->GetArguments(szArgs, MAX_PATH);
        hr = psl->GetWorkingDirectory(szDirPath, MAX_PATH);

        // Get the description of the target.
        hr = psl->GetDescription(szDescription, MAX_PATH);

        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;

        *app    = converter.to_bytes(szTargetPath);
    }

    // Release the pointer to the IPersistFile interface.
    ppf->Release();

    // Release the pointer to the IShellLink interface.
    psl->Release();

    return hr;
}
