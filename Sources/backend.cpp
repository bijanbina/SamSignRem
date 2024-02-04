#include "backend.h"

// list all .img in current dir except recovery.img
vector<string> sr_findImgs(string dirname)
{
    vector<string> ret;
    string path = sr_getCurrentPath() + "\\" + dirname;
    DIR    *dir = opendir(path.c_str());

    dirent *file = readdir(dir);
    while( file )
    {
        string filename = file->d_name;
        if( isValidImage(filename) )
        {
            ret.push_back(filename);
        }
        file = readdir(dir);
    }
    closedir(dir);

    return ret;
}

string sr_getCurrentPath()
{
    char buffer[MAX_PATH];
    GetCurrentDirectoryA(MAX_PATH, buffer);

    string path(buffer);
    return path;
}

// return true if its a valid image format
int isValidImage(string name)
{
    if( name=="recovery.img" ||
        name=="recovery.bin" )
    {
        return 0;
    }

    int have_img = name.find(".img");
    int have_bin = name.find(".bin");
    if( have_img==-1 && have_bin==-1 )
    {
        return 0;
    }

    return 1;
}

void sr_mkdir(string path)
{
    string cmd = "mkdir " + path + " >NUL  2>NUL";
    system(cmd.c_str());
}

string sc_getLastDirectoryName(string address)
{
    size_t lastSeparator = address.find_last_of("\\");
    if( lastSeparator!=std::string::npos )
    {
        return address.substr(lastSeparator + 1);
    }
    else
    {
        return address;
    }
}
