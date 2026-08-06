#pragma once
#include <optional>
#include <string>

#include "dal/Db.h"
#include "dal/UserRepository.h"
#include "models/User.h"

class UserService {
   public:
    explicit UserService(Db& db) : db(db), userRepo(db) {}

    std::optional<User> login(const std::string& login, const std::string& password);
    int createUser(const std::string& login, const std::string& password, const std::string& role);

   private:
    Db& db;
    UserRepository userRepo;
};