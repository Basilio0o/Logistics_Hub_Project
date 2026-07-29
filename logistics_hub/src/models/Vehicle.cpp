#include "models/Vehicle.h"

Vehicle::Vehicle()
    : id(0),
      number(""),
      driver(""),
      max_weight(0.0),
      max_volume(0.0),
      max_parcels(0),
      current_weight(0.0),
      current_volume(0.0),
      current_parcels(0),
      status("available"),
      dispatched_by(std::nullopt) {}

Vehicle::Vehicle(int id, const std::string& number, const std::string& driver, double max_weight,
                 double max_volume, int max_parcels, double current_weight, double current_volume,
                 int current_parcels, const std::string& status, std::optional<int> dispatched_by)
    : id(id),
      number(number),
      driver(driver),
      max_weight(max_weight),
      max_volume(max_volume),
      max_parcels(max_parcels),
      current_weight(current_weight),
      current_volume(current_volume),
      current_parcels(current_parcels),
      status(status),
      dispatched_by(dispatched_by) {}

int Vehicle::getId() const {
    return id;
}

const std::string& Vehicle::getNumber() const {
    return number;
}

const std::string& Vehicle::getDriver() const {
    return driver;
}

double Vehicle::getMaxWeight() const {
    return max_weight;
}

double Vehicle::getMaxVolume() const {
    return max_volume;
}

int Vehicle::getMaxParcels() const {
    return max_parcels;
}

double Vehicle::getCurrentWeight() const {
    return current_weight;
}

double Vehicle::getCurrentVolume() const {
    return current_volume;
}

int Vehicle::getCurrentParcels() const {
    return current_parcels;
}

const std::string& Vehicle::getStatus() const {
    return status;
}

std::optional<int> Vehicle::getDispatchedBy() const {
    return dispatched_by;
}