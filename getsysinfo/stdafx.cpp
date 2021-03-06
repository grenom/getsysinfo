// stdafx.cpp: исходный файл, содержащий только стандартные включаемые модули
// getsysinfo.pch будет использоваться в качестве предкомпилированного заголовка
// stdafx.obj будет содержать предварительно откомпилированные сведения о типе

#include "stdafx.h"

// TODO: Установите ссылки на любые требующиеся дополнительные заголовки в файле STDAFX.H
// , а не в данном файле




wql_res get_wql_result(string wminamespace, string instr)
{
	wstring w_instr = string_to_wstring(instr);
	wstring w_wminamespace = string_to_wstring(wminamespace);
	wql_res wout, error;
	map <string, string> e;
	e["error"] = "error";
	error.push_back(e);
	HRESULT hres;
	// Step 1: --------------------------------------------------
	// Initialize COM. ------------------------------------------
	hres = CoInitializeEx(0, COINIT_MULTITHREADED);
	if (FAILED(hres))
	{
		cout << "Failed to initialize COM library. Error code = 0x" << hex << hres << endl;
		return error;
	}
	// Step 2: --------------------------------------------------
	// Set general COM security levels --------------------------
	hres = CoInitializeSecurity(NULL,-1,NULL,NULL,RPC_C_AUTHN_LEVEL_DEFAULT,RPC_C_IMP_LEVEL_IMPERSONATE,NULL,EOAC_NONE,NULL);
	if (FAILED(hres))
	{
		cout << "Failed to initialize security. Error code = 0x"<< hex << hres << endl;
		CoUninitialize();
		return error;
	}
	// Step 3: ---------------------------------------------------
	// Obtain the initial locator to WMI -------------------------
	IWbemLocator *pLoc = NULL;
	hres = CoCreateInstance(CLSID_WbemLocator,0,CLSCTX_INPROC_SERVER,IID_IWbemLocator, (LPVOID *)&pLoc);
	if (FAILED(hres))
	{
		cout << "Failed to create IWbemLocator object."<< " Err code = 0x"<< hex << hres << endl;
		CoUninitialize();
		return error;
	}
	// Step 4: -----------------------------------------------------
	// Connect to WMI through the IWbemLocator::ConnectServer method
	IWbemServices *pSvc = NULL;
	// Connect to the root\cimv2 namespace with
	// the current user and obtain pointer pSvc
	// to make IWbemServices calls.
	//hres = pLoc->ConnectServer(_bstr_t(L"ROOT\\CIMV2"),NULL,NULL,0,NULL,0,0,&pSvc);
	hres = pLoc->ConnectServer(_bstr_t(w_wminamespace.data()), NULL, NULL, 0, NULL, 0, 0, &pSvc);
	if (FAILED(hres))
	{
		cout << "Could not connect. Error code = 0x" << hex << hres << endl;
		pLoc->Release();
		CoUninitialize();
		return error;
	}
	// Step 5: --------------------------------------------------
	// Set security levels on the proxy -------------------------
	hres = CoSetProxyBlanket(pSvc,RPC_C_AUTHN_WINNT,RPC_C_AUTHZ_NONE,NULL,RPC_C_AUTHN_LEVEL_CALL,RPC_C_IMP_LEVEL_IMPERSONATE,NULL,EOAC_NONE);
	if (FAILED(hres))
	{
		cout << "Could not set proxy blanket. Error code = 0x"<< hex << hres << endl;
		pSvc->Release();
		pLoc->Release();
		CoUninitialize();
		return error;
	}
	// Step 6: --------------------------------------------------
	// Use the IWbemServices pointer to make requests of WMI ----

	// For example, get the name of the operating system
	IEnumWbemClassObject* pEnumerator = NULL;
	hres = pSvc->ExecQuery(bstr_t("WQL"), SysAllocStringLen(w_instr.data(), w_instr.size()) ,WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,NULL,&pEnumerator);
	if (FAILED(hres))
	{
		cout << "Query for operating system name failed."<< " Error code = 0x"<< hex << hres << endl;
		pSvc->Release();
		pLoc->Release();
		CoUninitialize();
		return error;
	}

	// Step 7: -------------------------------------------------
	// Get the data from the query in step 6 -------------------
	IWbemClassObject *pclsObj = NULL;
	ULONG uReturn = 0;


	while (pEnumerator)
	{
		HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1,&pclsObj, &uReturn);
		if (0 == uReturn)
		{
			break;
		}
		//wout=L"$START_SECTION$\n";

		SAFEARRAY* pNames;
		pclsObj->GetNames(NULL, WBEM_FLAG_ALWAYS|WBEM_FLAG_NONSYSTEM_ONLY, NULL, &pNames);
		long lowerBound, upperBound;  // get array bounds
		SafeArrayGetLBound(pNames, 1, &lowerBound);
		SafeArrayGetUBound(pNames, 1, &upperBound);
		long lenNames = upperBound - lowerBound + 1;
		map <string, string> map_tmp;
			for (long i = 0; i <lenNames; i++)
			{
				BSTR bstrname;
				SafeArrayGetElement(pNames, &i, &bstrname);
				string name = wstring_to_string(bstrname);
				//wstring wname(bstrname, SysStringLen(bstrname));

				VARIANT vtProp;
				hr = pclsObj->Get(bstrname, 0, &vtProp, 0, 0);
				string property;
				if (vtProp.vt == VT_I4)
				{
					//wproperty = to_wstring(vtProp.intVal);
					if (name == "AdapterRAM")//video RAM не везде содержится в intVal
					{
						property = to_string(vtProp.llVal);
					}
					else
					{
						property = to_string(vtProp.intVal);
					}
				}
				else if (vtProp.vt == VT_BSTR)
				{
					property = wstring_to_string(vtProp.bstrVal);
					//property = wstring_to_string(vtProp.bstrVal);
					//property.assign(vtProp.bstrVal, SysStringLen(vtProp.bstrVal));
				}
				else if (vtProp.vt == VT_DISPATCH)
				{
					property = "test - VT_DISPATCH";
				}
				else if (vtProp.vt == VT_EMPTY)
				{
					property = "test - VT_EMPTY";
				}
				else if (vtProp.vt == VT_ARRAY+3)// VT_ARRAY type VT_I4
				{
					SAFEARRAY* prop_array = vtProp.parray;

					char buf0[128] = { 0 };
					char buf1[128] = { 0 };
					//char* pbuf2 = (char*)prop_array->pvData;
					memcpy_s(buf0, sizeof(buf0), prop_array->pvData, prop_array->rgsabound[0].cElements *3);//?prop_array->rgsabound[0].cElements
					int chn0 = 0, chn1 = 0;
					for (int chn0 = 0; chn0<sizeof(buf0); chn0++)
					{
						//if ((buf0[chn0] > 48 && buf0[chn0] < 126) || (buf0[chn0]>0 && buf0[chn0]<10))
						if (buf0[chn0] !=0)
						{
							buf1[chn1] = buf0[chn0];
							chn1++;
						}
						else
						{
							//buf1[chn1] = '';
							//chn1++;
						}
					}
					//buf1[chn1] = '\n';
					property.assign(buf1);
				}
				else if (vtProp.vt == VT_UI1)
				{
					property = to_string(vtProp.bVal);
				}
				else
				{
					property = "test uncnown type - " + to_string(vtProp.vt);
				}
				//if (name == "UserFriendlyName")
				//{
				//	property = "test uncnown type2 -" + to_string(vtProp.vt);
				//}
				VariantClear(&vtProp);
				//wcout << wname<< " - " << wproperty << endl;
				//wout += wname + wproperty + L"\n";
				//wout[1][wname] = wproperty;
				map_tmp[name] = property;
			}
		pclsObj->Release();
		wout.push_back(map_tmp);
	}
	//wout += L"$END_SECTION$\n";
	// Cleanup
	// ========
	pSvc->Release();
	pLoc->Release();
	pEnumerator->Release();
	CoUninitialize();
	return wout;
}



string clean_spaces(string in)
{
	//string str(in.begin(), in.end());
	string str = in;
	str += "   ";// чтобы регулярка ниже отрабатывала правильно для строк где нет вообще пробелов или табов
	string::const_iterator start = str.begin();
	string::const_iterator end = str.end();
	boost::smatch xResults;
	boost::regex r_regex_optionalsearch;
	r_regex_optionalsearch = "^(.+?)\\s+";
	string result;
	if (boost::regex_search(start, end, xResults, r_regex_optionalsearch))
	{
		result = xResults[1];
	}
	return result;
}

bool compare_instance(string in1, string in2)
{
	bool out = false;
	string::const_iterator start;
	string::const_iterator end;
	boost::regex r_regex("\\\\{2,2}");
	//cout << "in1 - " << in1 << ", in2 - " << in2 << endl;
	in1 = boost::regex_replace(in1, r_regex, "\\", boost::match_default | boost::format_all);
	//cout << "in1 result - " << in1 << endl;
	in2 = boost::regex_replace(in2, r_regex, "\\", boost::match_default | boost::format_all);
	//cout << "in2 result - " << in2 << endl;
	if (in1 == in2)
	{
		return true;
	}
	else
	{
		return false;
	}
	return false;
}


string wstring_to_string(wstring in)
{
	if (in.size() == 0)
	{
		return "";
	}
	string out;
	setlocale(LC_ALL, "");
	//const std::wstring ws = L"ħëłlö";
	locale locale("");
	typedef std::codecvt<wchar_t, char, std::mbstate_t> converter_type;
	const converter_type& converter = std::use_facet<converter_type>(locale);
	std::vector<char> to(in.length() * converter.max_length());
	std::mbstate_t state;
	const wchar_t* from_next;
	char* to_next;
	const converter_type::result result = converter.out(state, in.data(), in.data() + in.length(), from_next, &to[0], &to[0] + to.size(), to_next);
	if (result == converter_type::ok or result == converter_type::noconv) {
		const std::string s(&to[0], to_next);
		//std::cout << "std::string =     " << s << std::endl;
		out = s;
	}

	return out;
}

wstring string_to_wstring(string in)
{
	wstring_convert<codecvt_utf8_utf16<wchar_t>> converter;
	//string narrow = converter.to_bytes(wide_utf16_source_string);
	wstring wide = converter.from_bytes(in);
	return wide;
}

void dump_wql_res(wql_res in, bool debugmode)
{
	
	if (debugmode)
	{
		string out;

		cout << "Debug mode:" << endl;
		for (int i = 0; i < in.size(); i++)
		{
			map <string, string> s = in[i];
			map<string, string>::iterator it = s.begin();
			for (std::pair<string, string> element : s)
			{
				string name = element.first;
				string param = element.second;
				out += name + " - " + param + "\n";
			}
		}
		cout << out << endl;
	}
}