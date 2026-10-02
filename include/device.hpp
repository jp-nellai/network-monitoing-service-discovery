#pragma once
#include <string>
#include <vector>

struct InterfaceInfo {
    int index = 0;
    std::string name;
    std::string description;
    std::string type;
    std::string speed;
    std::string adminStatus;
    std::string operStatus;
};

struct Device {
    std::string ip;
    bool reachable = false;
    std::string sysName;
    std::string sysDescr;
    std::string sysObjectId;
    std::string sysUpTime;
    std::vector<InterfaceInfo> interfaces;
    std::string deviceType;
    std::string error;
};
