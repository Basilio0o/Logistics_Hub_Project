#include "services/UserService.h"

#include <iostream>

std::optional<User> UserService::login(const std::string& login, const std::string& password) {
    auto user = userRepo.getByLogin(login);
    if (!user) {
        return std::nullopt;
    }

    if (user->getPassHash() != password) {
        return std::nullopt;
    }

    db.execute_params("SELECT set_config('app.current_user_id', $1, false)", user->getId());
    return user;
}

int UserService::createUser(const std::string& login, const std::string& password,
                            const std::string& role) {
    if (role != "admin" && role != "manager" && role != "receiver" && role != "assembler") {
        std::cout << "Неверная роль. Допустимы: admin, manager, receiver, assembler" << std::endl;
        return -1;
    } else {
        return userRepo.createUser(login, password, role);
    }
}