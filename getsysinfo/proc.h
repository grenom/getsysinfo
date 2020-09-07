#pragma once


#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <psapi.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>


#include <stdio.h>
#include <tchar.h>


/*#include <comdef.h>
#include <Wbemidl.h>
#include <unordered_map>
#pragma comment(lib, "wbemuuid.lib")*/
using namespace std;

string GetProcessName(DWORD);
vector<map<string, string>> get_all_process();
