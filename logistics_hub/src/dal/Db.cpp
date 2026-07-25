#include "dal/Db.h"

#include <iostream>

Db::Db(const std::string& conn_string) : conn(conn_string) {
    std::cout << "connected to: " << conn.dbname() << std::endl;
}

Db::~Db() = default;