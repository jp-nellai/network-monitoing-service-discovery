#include "discovery.hpp"
#include "snmp_client.hpp"
#include <algorithm>
#include <atomic>
#include <iostream>
#include <mutex>
#include <thread>

DiscoveryEngine::DiscoveryEngine(std::string community, int timeoutMs, int retries, int workerCount)
    : community_(std::move(community)), timeoutMs_(timeoutMs), retries_(retries),
      workerCount_(std::max(1, workerCount)) {}

std::vector<Device> DiscoveryEngine::discover(const std::vector<std::string>& addresses) {
    if (addresses.empty()) return {};

    std::atomic<size_t> nextIndex{0};
    std::mutex resultMutex;
    std::vector<Device> devices;
    std::vector<std::thread> workers;

    const int count = std::min(workerCount_, static_cast<int>(addresses.size()));
    for (int i = 0; i < count; ++i) {
        workers.emplace_back([&]() {
            SnmpClient client(community_, timeoutMs_, retries_);
            while (true) {
                const size_t index = nextIndex.fetch_add(1);
                if (index >= addresses.size()) break;

                Device device = client.discover(addresses[index]);
                if (device.reachable) {
                    std::lock_guard<std::mutex> lock(resultMutex);
                    devices.push_back(std::move(device));
                    std::cout << "[+] SNMP device: " << addresses[index] << '\n';
                } else {
                    std::cout << "[-] No SNMP response: " << addresses[index] << '\n';
                }
            }
        });
    }

    for (auto& worker : workers) worker.join();

    std::sort(devices.begin(), devices.end(),
              [](const Device& a, const Device& b) { return a.ip < b.ip; });
    return devices;
}
