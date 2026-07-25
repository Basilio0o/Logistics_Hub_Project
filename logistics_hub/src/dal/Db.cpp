#include "dal/Db.h"

#include <iostream>

Db::Db(const std::string& conn_string) : conn(conn_string) {
    std::cout << "connected to: " << conn.dbname() << std::endl;
}

int Db::execute(const std::string& sql) {
    pqxx::work txn(conn);

    pqxx::result res = txn.exec(sql);

    txn.commit();

    return res.affected_rows();
}

pqxx::result Db::query(const std::string& sql) {
    pqxx::work txn(conn);

    pqxx::result res = txn.exec(sql);

    txn.commit();

    return res;
}

Db::~Db() = default;