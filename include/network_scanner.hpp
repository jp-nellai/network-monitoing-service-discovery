#pragma once
#include <cstdint>
#include <string>
#include <vector>
class NetworkScanner
{
public:
    static std::vector<std::string> expandCIDR(const std::string &);

private:
    static uint32_t toInt(const std::string &);
    static std::string toIp(uint32_t);
};
