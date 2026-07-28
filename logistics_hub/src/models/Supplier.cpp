#include "models/Supplier.h"

Supplier::Supplier() : id(0), name(""), phone(""), address("") {}

Supplier::Supplier(int id, const std::string& name, const std::string& phone,
                   const std::string& address)
    : id(id), name(name), phone(phone), address(address) {}

int Supplier::getId() const {
    return id;
}

const std::string& Supplier::getName() const {
    return name;
}

const std::string& Supplier::getPhone() const {
    return phone;
}

const std::string& Supplier::getAddress() const {
    return address;
}