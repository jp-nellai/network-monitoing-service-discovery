#include "discovery.hpp"
#include <atomic>
#include <thread>
#include <mutex>
#include <algorithm>
#include <iostream>
DiscoveryEngine::DiscoveryEngine(SnmpCredentials c,int t,int r,int w):c_(std::move(c)),timeout_(t),retries_(r),workers_(std::max(1,w)){}
std::vector<Device> DiscoveryEngine::run(const std::vector<std::string>&ips){std::atomic<size_t>next{0};std::mutex m;std::vector<Device>out;std::vector<std::thread>ts;for(int i=0;i<std::min(workers_,(int)ips.size());++i)ts.emplace_back([&]{SnmpClient c(c_,timeout_,retries_);for(;;){auto n=next.fetch_add(1);if(n>=ips.size())break;auto d=c.discover(ips[n]);if(d.reachable){std::lock_guard<std::mutex>g(m);std::cout<<"[+] "<<ips[n]<<" "<<d.vendor<<" "<<d.deviceType<<"\n";out.push_back(std::move(d));}}});for(auto&t:ts)t.join();std::sort(out.begin(),out.end(),[](auto&a,auto&b){return a.ip<b.ip;});return out;}
