#include "sr_lz4.h"

void sr_lz4Decode()
{
    sr_mkdir("lz4");

    vector<string> lz4s = sr_findlz4s();
    int lz4_len = lz4s.size();
    for( int i=0 ; i<lz4_len ; i++ )
    {
        string name_without_lz4 = lz4s[i].substr(0, lz4s[i].size()-4);
        string cmd = "lz4.exe " + lz4s[i] + " lz4\\" +
                name_without_lz4 + "\n";
        system(cmd.c_str());
    }
}

void sr_lz4Code()
{
    sr_mkdir("tar");

    vector<string> imgs = sr_findImgs("patched");
    int imgs_len = imgs.size();
    for( int i=0 ; i<imgs_len ; i++ )
    {
        string cmd = "lz4.exe patched\\" + imgs[i] + " tar\\" +
                imgs[i] + ".lz4\n";
        system(cmd.c_str());
    }
}

vector<string> sr_findlz4s()
{
    vector<string> ret;
    string path = sr_getCurrentPath();
    DIR    *dir = opendir(path.c_str());

    dirent *file = readdir(dir);
    while( file )
    {
        string filename = file->d_name;
        int have_lz4 = filename.find(".lz4");
        if( have_lz4!=-1 )
        {
            ret.push_back(filename);
        }
        file = readdir(dir);
    }
    closedir(dir);

    return ret;
}
