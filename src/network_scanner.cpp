#include "network_scanner.hpp"
#include <arpa/inet.h>
#include <stdexcept>
uint32_t NetworkScanner::toInt(const std::string &s)
{
    in_addr a{};
    if (inet_pton(AF_INET, s.c_str(), &a) != 1)
        throw std::invalid_argument("invalid IPv4");
    return ntohl(a.s_addr);
}
std::string NetworkScanner::toIp(uint32_t v)
{
    in_addr a{};
    a.s_addr = htonl(v);
    char b[INET_ADDRSTRLEN]{};
    inet_ntop(AF_INET, &a, b, sizeof(b));
    return b;
}
std::vector<std::string> NetworkScanner::expandCIDR(const std::string &c)
{
    auto p = c.find('/');
    if (p == std::string::npos)
        throw std::invalid_argument("CIDR required");
    int n = std::stoi(c.substr(p + 1));
    if (n < 0 || n > 32)
        throw std::invalid_argument("bad prefix");
    uint32_t ip = toInt(c.substr(0, p)), mask = n ? 0xffffffffu << (32 - n) : 0, net = ip & mask, bc = net | ~mask;
    std::vector<std::string> r;
    if (n >= 31)
        for (uint64_t x = net; x <= bc; ++x)
            r.push_back(toIp((uint32_t)x));
    else
        for (uint32_t x = net + 1; x < bc; ++x)
            r.push_back(toIp(x));
    return r;
}
