#pragma once
#include <string>
#include <vector>
struct IpAddress
{
    std::string address;
    int ifIndex{};
    std::string prefix;
};
struct InterfaceInfo
{
    int index{};
    std::string name, description, type, speed, adminStatus, operStatus;
    std::vector<IpAddress> addresses;
};
struct Device
{
    std::string ip, sysName, sysDescr, sysObjectId, sysUpTime, vendor, deviceType, error;
    bool reachable{};
    std::vector<InterfaceInfo> interfaces;
};
