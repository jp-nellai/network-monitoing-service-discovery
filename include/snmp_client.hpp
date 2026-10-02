#pragma once
#include "model.hpp"
#include <string>
struct SnmpCredentials { std::string securityName,authPass,privPass; int securityLevel{3}; };
class SnmpClient { public: SnmpClient(SnmpCredentials,int,int); Device discover(const std::string&); private: SnmpCredentials c_; int timeout_,retries_; };
