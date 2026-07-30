#include "models/Supplier.h"

Supplier::Supplier() : id(0), name(""), phone(std::nullopt), address(std::nullopt) {}

Supplier::Supplier(int id, const std::string& name, std::optional<std::string> phone,
                   std::optional<std::string> address)
    : id(id), name(name), phone(phone), address(address) {}

int Supplier::getId() const {
    return id;
}

const std::string& Supplier::getName() const {
    return name;
}

std::optional<std::string> Supplier::getPhone() const {
    return phone;
}

std::optional<std::string> Supplier::getAddress() const {
    return address;
}