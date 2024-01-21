#include "sr_backend.h"

int main()
{
    vector<string> imgs = sr_findImgs();
    long len = imgs.size();
    for( long i=0 ; i<len ; i++ )
    {
        sr_processFiles(imgs[i]);
    }

    string answer;
    cout << "Replace .img files? [Y|n]: ";
    cin >> answer;

    if( answer=="y" || answer.size()==0 || answer=="Y" )
    {
        for( long i=0 ; i<len ; i++ )
        {
//            raw2img(imgs[i]);
        }
    }

    return 0;
}

