#include "proc.h"

string GetProcessName(DWORD processID)
{
	CHAR szProcessName[MAX_PATH] = "<unknown>";
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processID);//PROCESS_QUERY_INFORMATION | PROCESS_VM_READ
	string out;

	if (NULL != hProcess)
	{
		HMODULE hMod;
		DWORD cbNeeded;

		if (EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded))
		{
			GetModuleBaseNameA(hProcess, hMod, szProcessName, sizeof(szProcessName) / sizeof(TCHAR));
			out = szProcessName;
		}
		else
		{
			out = "<error get process name>";
		}
	}
	else
	{
		out = "<error open process>";
	}
	//_tprintf(TEXT("%s  (PID: %u)\n"), szProcessName, processID);
	CloseHandle(hProcess);
	return out;
}


vector<map<string, string>> get_all_process()
{
	DWORD aProcesses[1024], cbNeeded, cProcesses;
	unsigned int i;
	vector<map<string, string>> out;
	map <string, string> e;
	e["error"] = "error";
	if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded))
	{
		out.push_back(e);
		return out;
	}

	cProcesses = cbNeeded / sizeof(DWORD);
	//std:map <string, int> e;
	for (i = 0; i < cProcesses; i++)
	{
		if (aProcesses[i] != 0)
		{
			
			
			map<string, string> p;
			p["name"] = GetProcessName(aProcesses[i]);
			p["pid"] = to_string(aProcesses[i]);
			out.push_back(p);
		}
	}
	return out;
}