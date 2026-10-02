#pragma once
#include "database.hpp"
class HttpServer { Database& db_; int port_; public: HttpServer(Database& d,int p):db_(d),port_(p){} void run(); };
