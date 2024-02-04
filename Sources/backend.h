#ifndef BACKEND_H
#define BACKEND_H

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
string sc_getLastDirectoryName(string address);
vector<string> sr_findImgs(string dirname);
void sr_checkProjDir();
void sr_ask2Replace(vector<string> imgs);
void sr_mkdir(string path);
int  isValidImage(string name);

#endif // BACKEND_H
