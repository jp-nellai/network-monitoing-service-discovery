#include "device.hpp"
#include "discovery.hpp"
#include "network_scanner.hpp"
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

namespace {
struct Options {
    std::string subnet;
    std::string community = "public";
    int timeoutMs = 1000;
    int retries = 1;
    int workers = 8;
    std::string output;
    std::string format = "json";
};

void printUsage(const char* p) {
    std::cout << "Usage: " << p << " --subnet <CIDR> [options]\n\n"
              << "  --subnet <CIDR>       IPv4 network, e.g. 192.168.1.0/24\n"
              << "  --community <string>  SNMPv2c community (default: public)\n"
              << "  --timeout <ms>        SNMP timeout (default: 1000)\n"
              << "  --retries <n>         SNMP retries (default: 1)\n"
              << "  --workers <n>         Concurrent workers (default: 8)\n"
              << "  --output <file>       Write discovery results\n"
              << "  --format <json|csv>   Output format (default: json)\n"
              << "  --help                Show this help\n";
}

bool parseArguments(int argc, char** argv, Options& o) {
    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        auto value = [&](const std::string& name) {
            if (i + 1 >= argc) throw std::invalid_argument("Missing value for " + name);
            return std::string(argv[++i]);
        };

        if (arg == "--subnet") o.subnet = value(arg);
        else if (arg == "--community") o.community = value(arg);
        else if (arg == "--timeout") o.timeoutMs = std::stoi(value(arg));
        else if (arg == "--retries") o.retries = std::stoi(value(arg));
        else if (arg == "--workers") o.workers = std::stoi(value(arg));
        else if (arg == "--output") o.output = value(arg);
        else if (arg == "--format") o.format = value(arg);
        else if (arg == "--help") { printUsage(argv[0]); return false; }
        else throw std::invalid_argument("Unknown argument: " + arg);
    }

    if (o.subnet.empty()) throw std::invalid_argument("--subnet is required");
    if (o.timeoutMs <= 0 || o.retries < 0 || o.workers <= 0)
        throw std::invalid_argument("Invalid timeout, retry, or worker count");
    if (o.format != "json" && o.format != "csv")
        throw std::invalid_argument("--format must be json or csv");
    return true;
}

std::string jsonEscape(const std::string& s) {
    std::ostringstream out;
    for (char c : s) {
        switch (c) {
            case '"': out << "\\\""; break;
            case '\\': out << "\\\\"; break;
            case '\n': out << "\\n"; break;
            case '\r': out << "\\r"; break;
            case '\t': out << "\\t"; break;
            default: out << c;
        }
    }
    return out.str();
}

void writeJson(std::ostream& out, const std::vector<Device>& devices) {
    out << "[\n";
    for (size_t i = 0; i < devices.size(); ++i) {
        const auto& d = devices[i];
        out << "  {\n"
            << "    \"ip\": \"" << jsonEscape(d.ip) << "\",\n"
            << "    \"reachable\": " << (d.reachable ? "true" : "false") << ",\n"
            << "    \"sysName\": \"" << jsonEscape(d.sysName) << "\",\n"
            << "    \"sysDescr\": \"" << jsonEscape(d.sysDescr) << "\",\n"
            << "    \"sysObjectId\": \"" << jsonEscape(d.sysObjectId) << "\",\n"
            << "    \"sysUpTime\": \"" << jsonEscape(d.sysUpTime) << "\",\n"
            << "    \"deviceType\": \"" << jsonEscape(d.deviceType) << "\",\n"
            << "    \"interfaces\": [\n";

        for (size_t j = 0; j < d.interfaces.size(); ++j) {
            const auto& x = d.interfaces[j];
            out << "      {\n"
                << "        \"index\": " << x.index << ",\n"
                << "        \"name\": \"" << jsonEscape(x.name) << "\",\n"
                << "        \"description\": \"" << jsonEscape(x.description) << "\",\n"
                << "        \"type\": \"" << jsonEscape(x.type) << "\",\n"
                << "        \"speed\": \"" << jsonEscape(x.speed) << "\",\n"
                << "        \"adminStatus\": \"" << jsonEscape(x.adminStatus) << "\",\n"
                << "        \"operStatus\": \"" << jsonEscape(x.operStatus) << "\"\n"
                << "      }" << (j + 1 < d.interfaces.size() ? "," : "") << "\n";
        }
        out << "    ]\n  }" << (i + 1 < devices.size() ? "," : "") << "\n";
    }
    out << "]\n";
}

std::string csvEscape(const std::string& s) {
    std::string r = "\"";
    for (char c : s) r += (c == '"' ? "\"\"" : std::string(1, c));
    return r + "\"";
}

void writeCsv(std::ostream& out, const std::vector<Device>& devices) {
    out << "ip,sysName,sysDescr,sysObjectId,sysUpTime,deviceType,interfaceCount\n";
    for (const auto& d : devices)
        out << csvEscape(d.ip) << "," << csvEscape(d.sysName) << ","
            << csvEscape(d.sysDescr) << "," << csvEscape(d.sysObjectId) << ","
            << csvEscape(d.sysUpTime) << "," << csvEscape(d.deviceType) << ","
            << d.interfaces.size() << "\n";
}
}

int main(int argc, char** argv) {
    try {
        Options o;
        if (!parseArguments(argc, argv, o)) return 0;

        const auto addresses = NetworkScanner::expandCIDR(o.subnet);
        std::cout << "Network: " << o.subnet << "\n"
                  << "Addresses to scan: " << addresses.size() << "\n"
                  << "Workers: " << o.workers << "\n\n";

        DiscoveryEngine engine(o.community, o.timeoutMs, o.retries, o.workers);
        const auto devices = engine.discover(addresses);

        std::cout << "\nDiscovery complete.\nSNMP devices found: "
                  << devices.size() << "\n";

        if (!o.output.empty()) {
            std::ofstream file(o.output);
            if (!file) throw std::runtime_error("Unable to open output file: " + o.output);
            if (o.format == "json") writeJson(file, devices);
            else writeCsv(file, devices);
            std::cout << "Results written to " << o.output << "\n";
        }

        std::cout << "\nDevices:\n";
        for (const auto& d : devices)
            std::cout << "  " << std::left << std::setw(16) << d.ip
                      << " " << std::setw(24) << d.sysName
                      << " " << d.deviceType
                      << " interfaces=" << d.interfaces.size() << "\n";
        return 0;
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << '\n';
        return 1;
    }
}
