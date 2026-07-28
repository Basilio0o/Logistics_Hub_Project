#include "models/Warehouse/Warehouse.h"

Warehouse::Warehouse() : id(0), name(""), address(""), square(0.0) {}

Warehouse::Warehouse(int id, const std::string& name, const std::string& address, double square)
    : id(id), name(name), address(address), square(square) {}

int Warehouse::getId() const {
    return id;
}

const std::string& Warehouse::getName() const {
    return name;
}

const std::string& Warehouse::getAddress() const {
    return address;
}

double Warehouse::getSquare() const {
    return square;
}
