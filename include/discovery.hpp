#pragma once
#include "device.hpp"
#include <string>
#include <vector>

class DiscoveryEngine {
public:
    DiscoveryEngine(std::string community, int timeoutMs, int retries, int workerCount);
    std::vector<Device> discover(const std::vector<std::string>& addresses);
private:
    std::string community_;
    int timeoutMs_;
    int retries_;
    int workerCount_;
};
