#include "models/User.h"

User::User() : id(0), login(""), pass_hash(""), role(""), created_at("") {}

User::User(int id, const std::string& login, const std::string& pass_hash, const std::string& role,
           const std::string& created_at)
    : id(id), login(login), pass_hash(pass_hash), role(role), created_at(created_at) {}

int User::getId() const {
    return id;
}

const std::string& User::getLogin() const {
    return login;
}

const std::string& User::getPassHash() const {
    return pass_hash;
}

const std::string& User::getRole() const {
    return role;
}

const std::string& User::getCreatedAt() const {
    return created_at;
}