#pragma once
#include "model.hpp"
#include <string>
struct sqlite3;
class Database { sqlite3* db_{}; public: explicit Database(const std::string&); ~Database(); void init(); void save(const Device&); std::string devicesJson() const; };
