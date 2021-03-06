// stdafx.h: включаемый файл для стандартных системных включаемых файлов
// или включаемых файлов для конкретного проекта, которые часто используются, но
// не часто изменяются
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>


#include <comdef.h>
#include <Wbemidl.h>
#include <iostream>
#include <string>
#include <vector>
#include <locale>
#include <codecvt>
#include <unordered_map>
#include <map>
#pragma comment(lib, "wbemuuid.lib")
#include <boost/program_options.hpp>
#include <boost/regex.hpp>
#include <boost/format.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>


using namespace std;
typedef vector<map <string, string>> wql_res;
//vector<map <string, string>> get_wql_result(string);
wql_res get_wql_result(string,string);
string clean_spaces(string);
string wstring_to_string(wstring);
wstring string_to_wstring(string);
void dump_wql_res(wql_res,bool);
bool compare_instance(string,string);

// TODO: Установите здесь ссылки на дополнительные заголовки, требующиеся для программы
