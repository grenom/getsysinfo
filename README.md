# getsysinfo
Getting system information in command line for zabbix with autodiscovery. 

If you want to use this utility for monitoring (for example zabbix), conveniently store sysinfo.exe on public share with ro permisions.

_Compiled for x64, but if necessary i can compile for x86._

### How to use ###
**Usage with autodiscover in zabbix:**
* autodiscover windows processes: \\\\share\sysinfo.exe -c proc
* autodiscover displays: \\\\share\sysinfo.exe -c monitor
* get display info: \\\\share\sysinfo.exe -t monitor -i {#INSTANCE}

**Usual usage:**
* Get info about CPU: \\\\share\sysinfo.exe -t cpu
* Get info about RAM: \\\\share\sysinfo.exe -t ram
* Get info about S/N: \\\\share\sysinfo.exe -t sn

**Available getting info (-t param):**
* cpu
* ram - two modes
* po - installed soft
* gpu
* bios
* sn
* model
* monitor

Use --help for detail.
