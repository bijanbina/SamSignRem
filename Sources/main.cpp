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
    cout << "Samsung Sign Remover V0.4\n";
    cout << "Confirm Replacing? [Y|n]: ";
    std::getline(std::cin, answer);

    if( answer=="y" || answer.size()==0 || answer=="Y" )
    {
        for( long i=0 ; i<len ; i++ )
        {
            sr_raw2img(imgs[i]);
        }
    }
    else
    {
        for( long i=0 ; i<len ; i++ )
        {
            sr_rmRaw(imgs[i]);
        }
    }

    return 0;
}
