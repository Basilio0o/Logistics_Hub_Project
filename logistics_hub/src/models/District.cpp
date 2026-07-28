#include "models/District.h"

District::District() : id(0), name("") {}

District::District(int id, const std::string& name) : id(id), name(name) {}

int District::getId() const {
    return id;
}

const std::string& District::getName() const {
    return name;
}