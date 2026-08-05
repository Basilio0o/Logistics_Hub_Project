#pragma once
#include <optional>
#include <string>
#include <vector>

#include "dal/Db.h"
#include "models/User.h"

class UserRepository {
   public:
    explicit UserRepository(Db& db) : db(db) {}

    int createUser(const std::string& login, const std::string& pass_hash, const std::string& role);

    std::vector<User> getAll();
    std::optional<User> getById(int id);
    std::optional<User> getByLogin(const std::string& login);

   private:
    Db& db;
    User mapRow(const pqxx::row& row);
};