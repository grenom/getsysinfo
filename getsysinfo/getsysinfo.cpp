// getsysinfo.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
//#include "proc.h"





std::string version = "0.88";

int main(int argc, char* argv[])
{
	using namespace std;
	setlocale(LC_CTYPE, "");
	//SetConsoleOutputCP(65001);

	string in_type_hw;
	string in_type_discovery, in_instance;
	bool debugmode=false;
	namespace po = boost::program_options;
	po::options_description desc((string)"This prog write VDV (vdv@oduv.so-ups.ru). Version: " + version + ". Date compile: " + (string)__DATE__);
	string CPU_MODEL("cpu");
	string RAM_FULL("ramex");
	string RAM_BRIEF("ram");
	string SOFT("po");
	string GPU("gpu");
	string BIOS("bios");
	string SN("sn");
	string MODEL("model");
	string MONITOR("monitor");
	string PROCESS("proc");
	string INSTALLDATE("installdate");
	//string DZAB_MONITOR("monitors");
	string typehelp;
	typehelp = "hw type: " + CPU_MODEL + ", " + RAM_FULL + ", " + RAM_BRIEF+", " + SOFT+", "+GPU+", "+BIOS+", "+SN+", "+MODEL+", "+MONITOR+", " + INSTALLDATE;
	string discoveryhelp;
	discoveryhelp = "discovery type for ZABBIX: " + MONITOR + ", " +PROCESS;
	desc.add_options()
		("help,h", "this help")
		("type,t", po::value<string>(&in_type_hw), typehelp.data())
		("discovery,c", po::value<string>(&in_type_discovery), discoveryhelp.data())
		("instance,i", po::value<string>(&in_instance), "instance for --type (it uses with discovery templates in zabbix)")
		("debug,d", po::bool_switch(&debugmode), "debug mode");
	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);
#if _DEBUG
	cout << "DEBUG. Param:" << endl;
	cout << "hw - " << in_type_hw << endl;
#endif


	if (vm.count("help"))
	{
		cout << desc << endl;
#ifdef _DEBUG
		string temp1;
		cin >> temp1;
#endif
		return -1;
	}
	if (in_type_hw.empty() && !in_type_discovery.empty())
	{
		
	}
	else if (!in_type_hw.empty() && in_type_discovery.empty())
	{

	}
	else if(!in_type_hw.empty() && in_type_discovery.empty())
	{
		cout << "Void input param. Use --help" << endl;
		return -1;
	}

	if (!(in_type_hw == CPU_MODEL || in_type_hw == RAM_BRIEF || in_type_hw == RAM_FULL || in_type_hw == SOFT || in_type_hw == GPU \
		|| in_type_hw == BIOS || in_type_hw == SN || in_type_hw == MODEL || in_type_hw == MONITOR || in_type_discovery == MONITOR \
		|| in_type_discovery == PROCESS || in_type_hw == INSTALLDATE))
	{
			cout << "Error input param. Use --help" << endl;
			return -1;
	}




	if (in_type_hw == RAM_BRIEF || in_type_hw == RAM_FULL)
	{
		string out;
		wql_res sss = get_wql_result("ROOT\\CIMV2", "SELECT * FROM win32_physicalmemory");
		dump_wql_res(sss, debugmode);
		long long n_class = sss.size();
		if (in_type_hw == RAM_BRIEF)
		{
			out += to_string(n_class) + " ram socket(s). ";
		}
		for (int i = 0; i < n_class; i++)
		{
			map <string, string> s = sss[i];
			map<string, string>::iterator it = s.begin();

			//int capacity = stoi(s[L"Capacity"]);
			long long capacity = stoll(s["Capacity"]);
			capacity /= 1024 * 1024 * 1024;
			string partnumber = clean_spaces(s["PartNumber"]);
			string serialnumber = clean_spaces(s["SerialNumber"]);
			string	speed = s["Speed"];
			string manufacturer = clean_spaces(s["Manufacturer"]);
			string devicelocator = s["DeviceLocator"];
			if (in_type_hw == RAM_BRIEF)
			{
				out +=to_string(capacity) + "GB + ";
			}
			else if (in_type_hw == RAM_FULL)
			{
				out += to_string(i+1) + ". " + to_string(capacity) + "GB (" + speed + "MHz, p/n " + partnumber + ", s/n " + serialnumber + ", vendor " + manufacturer + ", " + devicelocator + ")  \n";
			}
		}
		out = out.substr(0, out.size() - 3);
		cout << out << endl;
	}
	else if (in_type_hw == SOFT)
	{
		//UINT oldcodepage = GetConsoleOutputCP();
		//SetConsoleOutputCP(65001);
		//string out;
		wql_res results_wql = get_wql_result("ROOT\\CIMV2", "SELECT * FROM win32_product");
		dump_wql_res(results_wql, debugmode);
		map <string, string> nameparam;
		string caption;
		string description;
		string installdate;
		string version;
		int count_po = 0;
		for (int i = 0; i < results_wql.size(); i++)
		{
			nameparam = results_wql[i];
			caption = nameparam["Caption"];
			description = nameparam["Description"];
			installdate = nameparam["InstallDate"];
			version = nameparam["Version"];
			if (!description.empty() && !caption.empty())
			{
				count_po++;
				cout << count_po << ". " << caption << " (install - " << installdate << ", version - " << version << ")" << endl;
			}
		}
	}
	else if (in_type_hw == CPU_MODEL)
	{
		//string out;
		wql_res results_wql = get_wql_result("ROOT\\CIMV2", "select * from win32_processor");
		dump_wql_res(results_wql, debugmode);
		map <string, string> nameparam;
		string name;
		string socket;
		int cores;
		int threads;
		int l2;
		int l3;
		for (int i = 0; i < results_wql.size(); i++)
		{
			nameparam = results_wql[i];
			name = nameparam["Name"];
			socket = nameparam["SocketDesignation"];
			cores = stoi(nameparam["NumberOfCores"]);
			threads = stoi(nameparam["NumberOfLogicalProcessors"]);
			l2 = stoi(nameparam["L2CacheSize"]);
			l3 = stoi(nameparam["L3CacheSize"]);
			cout << to_string(i + 1) << ". " << name << " (" << socket << ", cores-" << cores << ", threads-" << threads << ", L2-" << l2 << ", L3-" << l3 << ")" << endl;
		}

	}
	else if (in_type_hw == GPU)
	{
		wql_res results_wql = get_wql_result("ROOT\\CIMV2", "select * from Win32_VideoController");
		dump_wql_res(results_wql, debugmode);
		map <string, string> nameparam;
		string description, dver;
		long long vram;
		for (int i = 0; i < results_wql.size(); i++)
		{
			nameparam = results_wql[i];
			description = nameparam["Description"];
			dver = nameparam["DriverVersion"];
			vram = stoll(nameparam["AdapterRAM"]);
			if (vram != 0)
			{
				vram /= 1024*1024;
			}
			cout << to_string(i + 1) << ". " << description << " (" << vram << "MB, driver-" << dver << ")" << endl;
		}
	}
	else if (in_type_hw == BIOS)//может добавить releace_date, например из MS_SystemInformation
	{
		wql_res results_wql = get_wql_result("ROOT\\CIMV2", "select * from Win32_Bios");
		dump_wql_res(results_wql, debugmode);
		map <string, string> nameparam;
		string name,serial,ver;//Version,SerialNumber,Name
		string outs;
		for (int i = 0; i < results_wql.size(); i++)
		{
			nameparam = results_wql[i];
			name = nameparam["Name"];
			serial = nameparam["SerialNumber"];
			ver = nameparam["Version"];
			outs += name + " (serial-'" + serial + "', version-'" + ver +"'";
			//cout << name << " (serial-" << serial << ", version-" << ver << ")" << endl;
		}

		results_wql = get_wql_result("ROOT\\wmi", "select * from MS_SystemInformation");
		dump_wql_res(results_wql, debugmode);
		string biosdate;
		for (int i = 0; i < results_wql.size(); i++)
		{
			nameparam = results_wql[i];
			biosdate = nameparam["BIOSReleaseDate"];
			outs += ", date-'" + biosdate + "')\n";
		}
		cout << outs;

	}
	else if (in_type_hw == SN)
	{
		wql_res results_wql = get_wql_result("ROOT\\CIMV2", "select * from Win32_SystemEnclosure");
		dump_wql_res(results_wql, debugmode);
		map <string, string> nameparam;
		string sn, sn_old;//SerialNumber,SMBIOSAssetTag
		for (int i = 0; i < results_wql.size(); i++)
		{
			nameparam = results_wql[i];
			sn = nameparam["SerialNumber"];
			sn_old = nameparam["SMBIOSAssetTag"];
			boost::smatch xResults;
			boost::regex r_regex("^\\s+$");
			if (sn_old.empty() || sn_old==sn)
			{
				cout << sn << endl;
			}
			else if (boost::regex_match(sn_old,xResults,r_regex))
			{
				cout << sn << endl;
			}
			else
			{
				cout << sn << " (" << sn_old << ")" << endl;
			}
		}
	}
	else if (in_type_hw == MODEL)
	{
		wql_res results_wql = get_wql_result("ROOT\\wmi", "select * from MS_SystemInformation");
		dump_wql_res(results_wql, debugmode);
		map <string, string> nameparam;
		string model, sku, product;//SystemVersion, SystemSKU
		for (int i = 0; i < results_wql.size(); i++)
		{
			nameparam = results_wql[i];
			model = nameparam["SystemVersion"];
			sku = nameparam["SystemSKU"];
			product = nameparam["SystemProductName"];
			if (model.empty())
			{
				cout << product << " (" << sku << ")" << endl;
			}
			else
			{
				cout << model << " (" << sku << ")" << endl;
			}
		}
	}
	else if (in_type_hw == INSTALLDATE)
	{
		wql_res results_wql = get_wql_result("ROOT\\CIMV2", "select * from Win32_OperatingSystem");
		dump_wql_res(results_wql, debugmode);
		map <string, string> nameparam;
		string installdate;
		for (int i = 0; i < results_wql.size(); i++)
		{
			nameparam = results_wql[i];
			installdate = nameparam["InstallDate"];
			if (!installdate.empty())
			{
				boost::smatch xResults;
				boost::regex r_regex("^(\\d{4})(\\d{2})(\\d{2}).+");
				if (boost::regex_search((string::const_iterator)installdate.begin(), (string::const_iterator)installdate.end(), xResults, r_regex))
				{
					cout << boost::format("%3%.%2%.%1%") % xResults[1] % xResults[2] % xResults[3] << endl;
				}
			}
		}
	}
	else if (in_type_hw == MONITOR)
	{
		wql_res results_wql = get_wql_result("ROOT\\wmi", "select * from WmiMonitorId");
		dump_wql_res(results_wql, debugmode);
		map <string, string> nameparam;
		if (in_instance.empty())
		{
			string model, serial, week, year;
			for (int i = 0; i < results_wql.size(); i++)
			{
				nameparam = results_wql[i];
				model = nameparam["UserFriendlyName"];
				serial = nameparam["SerialNumberID"];
				week = nameparam["WeekOfManufacture"];
				year = nameparam["YearOfManufacture"];
				cout << to_string(i + 1) << ". " << model << " (s/n-'" << serial << "', manufacture-'" << year << "year " << week << "week')" << endl;
			}
		}
		else
		{
			//cout << "in_instance - " << in_instance << endl;
			for (int i = 0; i < results_wql.size(); i++)
			{
				if (compare_instance(in_instance, results_wql[i]["InstanceName"]))
				{

					//cout << "OK. results_wql[i][\"InstanceName\"] - " << results_wql[i]["InstanceName"] << ", in_property" << in_property << endl;
					string model, serial, week, year;
					nameparam = results_wql[i];
					model = nameparam["UserFriendlyName"];
					serial = nameparam["SerialNumberID"];
					week = nameparam["WeekOfManufacture"];
					year = nameparam["YearOfManufacture"];
					cout <<  model << " (s/n-'" << serial << "', manufacture-'" << year << "year " << week << "week')" << endl;

				}
				else
				{
					//out << "FAIL. results_wql[i][\"InstanceName\"] - " << results_wql[i]["InstanceName"] << ", in_property" << in_property << endl;
				}
			}
		}
	}
	else if (in_type_discovery == MONITOR)
	{
		wql_res results_wql = get_wql_result("ROOT\\wmi", "select * from WmiMonitorId");
		dump_wql_res(results_wql, debugmode);

		namespace pt = boost::property_tree;
		pt::ptree root;
		pt::ptree data_node;

		map <string, string> nameparam;
		for (int i = 0; i < results_wql.size(); i++)
		{
			pt::ptree data_element;
			nameparam = results_wql[i];
			//data_element.put("{#MODEL}", nameparam["UserFriendlyName"]);
			data_element.put("{#SN}", nameparam["SerialNumberID"]);
			data_element.put("{#INSTANCE}", nameparam["InstanceName"]);
			data_node.push_back(make_pair("", data_element));
		}
		root.add_child("data", data_node);
		pt::write_json(cout, root);
	}
	else if (in_type_discovery == PROCESS)
	{
		wql_res results_wql = get_wql_result("ROOT\\CIMV2", "select ProcessId,Caption,Name,Description from Win32_Process");//ExecutablePath
		dump_wql_res(results_wql, debugmode);

		for (int i = 0; i < results_wql.size(); i++)
		{
			for (int j = 0; j < results_wql.size(); j++)
			{
				if (i != j)
				{
					if (results_wql[i]["Name"] == results_wql[j]["Name"])
					{
						results_wql.erase(results_wql.begin() + j);
					}
				}
			}
		}


		namespace pt = boost::property_tree;
		pt::ptree root;
		pt::ptree data_node;

		map <string, string> nameparam;
		for (int i = 0; i < results_wql.size(); i++)
		{
			pt::ptree data_element;
			nameparam = results_wql[i];
			data_element.put("{#PROCID}", nameparam["ProcessId"]);
			data_element.put("{#NAME}", nameparam["Name"]);
			data_element.put("{#CAPTION}", nameparam["Caption"]);
			data_node.push_back(make_pair("", data_element));
		}
		root.add_child("data", data_node);
		pt::write_json(cout, root);
	}
	else
	{
		cout << "unexpected param error :(" << endl;
	}


#ifdef _DEBUG
	string temp1;
	cin >> temp1;
#endif
    return 0;
}

