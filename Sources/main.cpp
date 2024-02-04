#include "sr_processor.h"
#include "sr_lz4.h"
#include "sr_tar.h"

int main()
{
    cout << "Samsung Sign Remover V0.4\n";

    sr_lz4Decode();

    vector<string> imgs = sr_findImgs("lz4");
    int len = imgs.size();

    SrProcessor processor;
    for( int i=0 ; i<len ; i++ )
    {
        processor.processFile(imgs[i]);
    }

    sr_lz4Code();

    sr_tar();

    return 0;
}
