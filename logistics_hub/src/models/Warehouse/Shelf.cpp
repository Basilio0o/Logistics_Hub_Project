#include "models/Warehouse/Shelf.h"

Shelf::Shelf()
    : id(0),
      rack_id(0),
      code(""),
      max_weight(0.0),
      max_volume(0.0),
      current_weight(0.0),
      current_volume(0.0),
      status("active") {}

Shelf::Shelf(int id, int rack_id, const std::string& code, double max_weight, double max_volume,
             double current_weight, double current_volume, const std::string& status)
    : id(id),
      rack_id(rack_id),
      code(code),
      max_weight(max_weight),
      max_volume(max_volume),
      current_weight(current_weight),
      current_volume(current_volume),
      status(status) {}

int Shelf::getId() const {
    return id;
}

int Shelf::getRackId() const {
    return rack_id;
}

const std::string& Shelf::getCode() const {
    return code;
}

double Shelf::getMaxWeight() const {
    return max_weight;
}

double Shelf::getMaxVolume() const {
    return max_volume;
}

double Shelf::getCurrentWeight() const {
    return current_weight;
}

double Shelf::getCurrentVolume() const {
    return current_volume;
}

const std::string& Shelf::getStatus() const {
    return status;
}