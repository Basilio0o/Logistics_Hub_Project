#pragma once
#include <pqxx/pqxx>
#include <string>

class Db {
   public:
    Db(const std::string& conn_string);
    ~Db();

    int execute(const std::string& sql);

    pqxx::result query(const std::string& sql);

   private:
    pqxx::connection conn;
};