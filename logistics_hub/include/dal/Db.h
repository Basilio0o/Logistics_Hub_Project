#pragma once
#include <pqxx/pqxx>
#include <string>

class Db {
   public:
    Db(const std::string& conn_string);
    ~Db();
    template <typename... Args>
    pqxx::result query_params(const std::string& sql, Args&&... args);
    template <typename... Args>
    int execute_params(const std::string& sql, Args&&... args);

    int execute(const std::string& sql);

    pqxx::result query(const std::string& sql);

   private:
    pqxx::connection conn;
};

template <typename... Args>
pqxx::result Db::query_params(const std::string& sql, Args&&... args) {
    pqxx::work txn(conn);
    pqxx::result res = txn.exec_params(sql, std::forward<Args>(args)...);
    txn.commit();
    return res;
}

template <typename... Args>
int Db::execute_params(const std::string& sql, Args&&... args) {
    pqxx::work txn(conn);
    pqxx::result res = txn.exec_params(sql, std::forward<Args>(args)...);
    txn.commit();
    return res.affected_rows();
}