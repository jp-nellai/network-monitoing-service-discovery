#include "network_scanner.hpp"
#include <arpa/inet.h>
#include <stdexcept>

uint32_t NetworkScanner::ipToUint(const std::string& ip) {
    struct in_addr addr{};
    if (inet_pton(AF_INET, ip.c_str(), &addr) != 1)
        throw std::invalid_argument("Invalid IPv4 address: " + ip);
    return ntohl(addr.s_addr);
}

std::string NetworkScanner::uintToIp(uint32_t value) {
    struct in_addr addr{};
    addr.s_addr = htonl(value);
    char buffer[INET_ADDRSTRLEN]{};
    if (!inet_ntop(AF_INET, &addr, buffer, sizeof(buffer)))
        throw std::runtime_error("Failed to convert IPv4 address");
    return std::string(buffer);
}

std::vector<std::string> NetworkScanner::expandCIDR(const std::string& cidr) {
    const auto slash = cidr.find('/');
    if (slash == std::string::npos)
        throw std::invalid_argument("CIDR must have the form address/prefix");

    const std::string ip = cidr.substr(0, slash);
    int prefix;
    try { prefix = std::stoi(cidr.substr(slash + 1)); }
    catch (...) { throw std::invalid_argument("Invalid CIDR prefix"); }

    if (prefix < 0 || prefix > 32)
        throw std::invalid_argument("CIDR prefix must be 0-32");

    const uint32_t address = ipToUint(ip);
    const uint32_t mask = prefix == 0 ? 0u : (0xFFFFFFFFu << (32 - prefix));
    const uint32_t network = address & mask;
    const uint32_t broadcast = network | ~mask;

    std::vector<std::string> result;
    if (prefix >= 31) {
        for (uint64_t v = network; v <= broadcast; ++v)
            result.push_back(uintToIp(static_cast<uint32_t>(v)));
    } else {
        for (uint32_t v = network + 1; v < broadcast; ++v)
            result.push_back(uintToIp(v));
    }
    return result;
}
