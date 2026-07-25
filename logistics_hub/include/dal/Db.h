#pragma once
#include <pqxx/pqxx>
#include <string>

class Db {
   public:
    Db(const std::string& conn_string);
    ~Db();

   private:
    pqxx::connection conn;
};