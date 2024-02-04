#ifndef SR_PROCESSOR_H
#define SR_PROCESSOR_H

#include "backend.h"

class SrProcessor
{
public:
    SrProcessor();

    void processFile(string base_name);
    void fillPositions(string pattern);
    void replaceBytes(string replacement);
    void printAscii(int index, int len);
    void printHex(int index, int len);
    void rwUntilPosition(int64_t position);
    void findPattInBlock(string *block, string pattern);

    FILE *img_file = NULL;
    FILE *raw_file = NULL;
    char *f_buffer;
    int64_t  curr_pos;
    vector<int64_t> positions;
};

#endif // SR_PROCESSOR_H
