#include "dal/UserRepository.h"

std::vector<User> UserRepository::getAll() {
    auto result = db.query("SELECT id, login, pass_hash, role, created_at FROM users");

    std::vector<User> users;
    users.reserve(result.size());

    for (const auto& row : result) {
        users.push_back(mapRow(row));
    }
    return users;
}

std::optional<User> UserRepository::getById(int id) {
    std::string sql = "SELECT id, login, pass_hash, role, created_at FROM users WHERE id = $1";
    auto result = db.query_params(sql, id);

    if (result.empty()) {
        return std::nullopt;
    }
    return mapRow(result[0]);
}

std::optional<User> UserRepository::getByLogin(const std::string& login) {
    std::string sql = "SELECT id, login, pass_hash, role, created_at FROM users WHERE login = $1";
    auto result = db.query_params(sql, login);

    if (result.empty()) {
        return std::nullopt;
    }
    return mapRow(result[0]);
}

User UserRepository::mapRow(const pqxx::row& row) {
    return User(row["id"].as<int>(), row["login"].as<std::string>(),
                row["pass_hash"].as<std::string>(), row["role"].as<std::string>(),
                row["created_at"].as<std::string>());
}