#ifndef SR_BACKEND_H
#define SR_BACKEND_H

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>
#include <dirent.h>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <windows.h>

#define SR_BUFFER_MARGIN 100
#define SR_BLOCK_SIZE    1e6
// Calc from -> "SignerVer02.....65165289R.......A346EXXU2AWE2...";
#define SR_REPLACE_SIZE  48

using namespace std;

string sr_getCurrentPath();
vector<string> sr_findImgs();
void sr_fillPositions();
void sr_processFiles(string base_name);
void sr_checkProjDir();
void sr_printAscii(int index);
void sr_printHex(int index);
void sr_rwUntilPosition(long position);
void sr_replaceBytes();
void sr_raw2img(string base_name);
void sr_findBlockPos(string *block);

#endif // SR_BACKEND_H
