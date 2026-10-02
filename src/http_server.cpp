#include "http_server.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sstream>
#include <stdexcept>
#include <iostream>
static const char *html = "<!doctype html><html><head><meta charset='utf-8'><title>NMS</title><style>body{font-family:system-ui;margin:30px}table{border-collapse:collapse;width:100%}td,th{padding:8px;border:1px solid #ccc}</style></head><body><h1>NMS Discovery</h1><table><thead><tr><th>IP</th><th>Name</th><th>Vendor</th><th>Type</th><th>Uptime</th><th>Last seen</th></tr></thead><tbody id='x'></tbody></table><script>fetch('/api/devices').then(r=>r.json()).then(a=>x.innerHTML=a.map(d=>`<tr><td>${d.ip}</td><td>${d.sysName}</td><td>${d.vendor}</td><td>${d.deviceType}</td><td>${d.sysUpTime}</td><td>${d.lastSeen}</td></tr>`).join(''))</script></body></html>";
void HttpServer::run()
{
    int s = socket(AF_INET, SOCK_STREAM, 0);
    int y = 1;
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &y, sizeof(y));
    sockaddr_in a{};
    a.sin_family = AF_INET;
    a.sin_addr.s_addr = INADDR_ANY;
    a.sin_port = htons(port_);
    if (bind(s, (sockaddr *)&a, sizeof(a)) < 0 || listen(s, 16) < 0)
        throw std::runtime_error("HTTP bind/listen failed");
    std::cout << "Dashboard: http://127.0.0.1:" << port_ << "/\n";
    for (;;)
    {
        int c = accept(s, nullptr, nullptr);
        if (c < 0)
            continue;
        char b[4096]{};
        int n = read(c, b, sizeof(b) - 1);
        std::string req(b, n), body = req.find("GET /api/devices") != std::string::npos ? db_.devicesJson() : html, type = req.find("GET /api/devices") != std::string::npos ? "application/json" : "text/html";
        std::ostringstream h;
        h << "HTTP/1.1 200 OK\r\nContent-Type: " << type << "\r\nContent-Length: " << body.size() << "\r\nConnection: close\r\n\r\n"
          << body;
        auto z = h.str();
        write(c, z.data(), z.size());
        close(c);
    }
}
