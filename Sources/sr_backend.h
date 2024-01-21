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
// Calc from -> "SignerVer02....65165289R......A346EXXU2AWE2...";
#define SR_REPLACE_SIZE  46

using namespace std;

string sr_getCurrentPath();
vector<string> sr_findImgs();
vector<int>    sr_findPositions();
int  sr_getReadSize(vector<int> addresses, size_t nth, int curr_addr);
void sr_processFiles(string base_name);
void sr_checkProjDir();
void sr_replaceBytes(vector<int> positions);
void sr_raw2img(string base_name);
void sr_findBlockPos(string *block, vector<int> *positions,
                     int file_offset);

#endif // SR_BACKEND_H
