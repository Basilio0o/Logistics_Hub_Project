#include "models/Warehouse/Rack.h"

Rack::Rack()
    : id(0),
      zone_id(0),
      code(""),
      max_weight(0.0),
      max_volume(0.0),
      current_weight(0.0),
      current_volume(0.0) {}

Rack::Rack(int id, int zone_id, const std::string& code, double max_weight, double max_volume,
           double current_weight, double current_volume)
    : id(id),
      zone_id(zone_id),
      code(code),
      max_weight(max_weight),
      max_volume(max_volume),
      current_weight(current_weight),
      current_volume(current_volume) {}

int Rack::getId() const {
    return id;
}

int Rack::getZoneId() const {
    return zone_id;
}

const std::string& Rack::getCode() const {
    return code;
}

double Rack::getMaxWeight() const {
    return max_weight;
}

double Rack::getMaxVolume() const {
    return max_volume;
}

double Rack::getCurrentWeight() const {
    return current_weight;
}

double Rack::getCurrentVolume() const {
    return current_volume;
}