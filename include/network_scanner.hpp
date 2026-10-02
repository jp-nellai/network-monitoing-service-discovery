#pragma once
#include <cstdint>
#include <string>
#include <vector>

class NetworkScanner {
public:
    static std::vector<std::string> expandCIDR(const std::string& cidr);
private:
    static uint32_t ipToUint(const std::string& ip);
    static std::string uintToIp(uint32_t value);
};
