#include "network_scanner.hpp"
#include "discovery.hpp"
#include "database.hpp"
#include "http_server.hpp"
#include <iostream>
#include <stdexcept>
static void usage(const char *p) { std::cout << "Usage: " << p << " --subnet CIDR --security-name USER [--auth-pass PASS --priv-pass PASS] [options]\nOptions: --security-level 1|2|3 --db FILE --port N --timeout MS --retries N --workers N\n"; }
int main(int argc, char **argv)
{
    try
    {
        std::string subnet, db = "nms.db", user, auth, priv;
        int port = 8080, timeout = 1500, retries = 1, workers = 8, level = 3;
        for (int i = 1; i < argc; ++i)
        {
            std::string a = argv[i];
            auto v = [&]()
            {if(i+1>=argc)throw std::runtime_error("missing value");return std::string(argv[++i]); };
            if (a == "--subnet")
                subnet = v();
            else if (a == "--db")
                db = v();
            else if (a == "--port")
                port = std::stoi(v());
            else if (a == "--timeout")
                timeout = std::stoi(v());
            else if (a == "--retries")
                retries = std::stoi(v());
            else if (a == "--workers")
                workers = std::stoi(v());
            else if (a == "--security-name")
                user = v();
            else if (a == "--auth-pass")
                auth = v();
            else if (a == "--priv-pass")
                priv = v();
            else if (a == "--security-level")
                level = std::stoi(v());
            else if (a == "--help")
            {
                usage(argv[0]);
                return 0;
            }
            else
                throw std::runtime_error("unknown option " + a);
        }
        if (subnet.empty() || user.empty())
        {
            usage(argv[0]);
            return 2;
        }
        if (level >= 2 && auth.empty())
            throw std::runtime_error("auth-pass required for level >=2");
        if (level >= 3 && priv.empty())
            throw std::runtime_error("priv-pass required for level 3");
        auto ips = NetworkScanner::expandCIDR(subnet);
        SnmpCredentials c{user, auth, priv, level};
        DiscoveryEngine e(c, timeout, retries, workers);
        auto ds = e.run(ips);
        Database database(db);
        for (auto &d : ds)
            database.save(d);
        std::cout << "Stored " << ds.size() << " devices in " << db << "\n";
        HttpServer server(database, port);
        server.run();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
}
