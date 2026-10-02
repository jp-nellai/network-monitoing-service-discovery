#pragma once
#include "model.hpp"
#include "snmp_client.hpp"
#include <string>
#include <vector>
class DiscoveryEngine
{
    SnmpCredentials c_;
    int timeout_, retries_, workers_;

public:
    DiscoveryEngine(SnmpCredentials, int, int, int);
    std::vector<Device> run(const std::vector<std::string> &);
};
