#pragma once
#include <string>

class User {
   public:
    User();
    User(int id, const std::string& login, const std::string& pass_hash, const std::string& role,
         const std::string& created_at);
    ~User() = default;

    int getId() const;
    const std::string& getLogin() const;
    const std::string& getPassHash() const;
    const std::string& getRole() const;
    const std::string& getCreatedAt() const;

   private:
    int id;
    std::string login;
    std::string pass_hash;
    std::string role;
    std::string created_at;
};