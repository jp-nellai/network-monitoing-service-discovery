#pragma once
#include "device.hpp"
#include <string>

class SnmpClient {
public:
    SnmpClient(const std::string& community, int timeoutMs, int retries);
    ~SnmpClient();
    SnmpClient(const SnmpClient&) = delete;
    SnmpClient& operator=(const SnmpClient&) = delete;
    Device discover(const std::string& ip);
private:
    std::string community_;
    int timeoutMs_;
    int retries_;
};
