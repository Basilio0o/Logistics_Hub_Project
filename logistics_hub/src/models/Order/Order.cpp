#include "models/Order/Order.h"

Order::Order() : id(0), district_id(0), priority("normal"), status("new"), created_at("") {}

Order::Order(int id, int district_id, const std::string& priority, const std::string& status,
             const std::string& created_at)
    : id(id),
      district_id(district_id),
      priority(priority),
      status(status),
      created_at(created_at) {}

int Order::getId() const {
    return id;
}

int Order::getDistrictId() const {
    return district_id;
}

const std::string& Order::getPriority() const {
    return priority;
}

const std::string& Order::getStatus() const {
    return status;
}

const std::string& Order::getCreatedAt() const {
    return created_at;
}