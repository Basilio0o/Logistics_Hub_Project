#include "models/Warehouse/Zone.h"

Zone::Zone()
    : id(0),
      warehouse_id(0),
      name(""),
      code(""),
      max_weight(0.0),
      max_volume(0.0),
      current_weight(0.0),
      current_volume(0.0) {}

Zone::Zone(int id, int warehouse_id, const std::string& name, const std::string& code,
           double max_weight, double max_volume, double current_weight, double current_volume)
    : id(id),
      warehouse_id(warehouse_id),
      name(name),
      code(code),
      max_weight(max_weight),
      max_volume(max_volume),
      current_weight(current_weight),
      current_volume(current_volume) {}

int Zone::getId() const {
    return id;
}

int Zone::getWarehouseId() const {
    return warehouse_id;
}

const std::string& Zone::getName() const {
    return name;
}

const std::string& Zone::getCode() const {
    return code;
}

double Zone::getMaxWeight() const {
    return max_weight;
}

double Zone::getMaxVolume() const {
    return max_volume;
}

double Zone::getCurrentWeight() const {
    return current_weight;
}

double Zone::getCurrentVolume() const {
    return current_volume;
}