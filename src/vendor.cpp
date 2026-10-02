#include "vendor.hpp"
#include <algorithm>
#include <cctype>
static std::string lo(std::string s){for(char&c:s)c=char(std::tolower((unsigned char)c));return s;}
std::string detectVendor(const std::string&o,const std::string&d){if(o.find("9.1.")!=std::string::npos)return"Cisco";if(o.find("2011.2.")!=std::string::npos)return"Huawei";if(o.find("2636.")!=std::string::npos)return"Juniper";if(o.find("11.2.")!=std::string::npos)return"HP";if(o.find("674.")!=std::string::npos)return"Dell";if(o.find("14988.")!=std::string::npos)return"MikroTik";auto x=lo(d);if(x.find("aruba")!=std::string::npos)return"Aruba";if(x.find("fortigate")!=std::string::npos)return"Fortinet";return"Unknown";}
std::string detectDeviceType(const std::string&d){auto x=lo(d);if(x.find("router")!=std::string::npos)return"router";if(x.find("switch")!=std::string::npos)return"switch";if(x.find("firewall")!=std::string::npos)return"firewall";if(x.find("printer")!=std::string::npos)return"printer";if(x.find("linux")!=std::string::npos)return"linux-host";if(x.find("windows")!=std::string::npos)return"windows-host";return"unknown";}
