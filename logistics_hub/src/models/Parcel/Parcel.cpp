#include "models/Parcel/Parcel.h"

Parcel::Parcel()
    : id(0),
      order_id(0),
      district_id(0),
      type(""),
      weight(0.0),
      volume(0.0),
      priority("normal"),
      status("new"),
      assembled_by(std::nullopt),
      created_at(""),
      assembled_at(std::nullopt),
      dispatched_at(std::nullopt),
      delivered_at(std::nullopt),
      vehicle_id(std::nullopt) {}

Parcel::Parcel(int id, int order_id, int district_id, const std::string& type, double weight,
               double volume, const std::string& priority, const std::string& status,
               std::optional<int> assembled_by, const std::string& created_at,
               std::optional<std::string> assembled_at, std::optional<std::string> dispatched_at,
               std::optional<std::string> delivered_at, std::optional<int> vehicle_id)
    : id(id),
      order_id(order_id),
      district_id(district_id),
      type(type),
      weight(weight),
      volume(volume),
      priority(priority),
      status(status),
      assembled_by(assembled_by),
      created_at(created_at),
      assembled_at(assembled_at),
      dispatched_at(dispatched_at),
      delivered_at(delivered_at),
      vehicle_id(vehicle_id) {}

int Parcel::getId() const {
    return id;
}

int Parcel::getOrderId() const {
    return order_id;
}

int Parcel::getDistrictId() const {
    return district_id;
}

const std::string& Parcel::getType() const {
    return type;
}

double Parcel::getWeight() const {
    return weight;
}

double Parcel::getVolume() const {
    return volume;
}

const std::string& Parcel::getPriority() const {
    return priority;
}

const std::string& Parcel::getStatus() const {
    return status;
}

std::optional<int> Parcel::getAssembledBy() const {
    return assembled_by;
}

const std::string& Parcel::getCreatedAt() const {
    return created_at;
}

std::optional<std::string> Parcel::getAssembledAt() const {
    return assembled_at;
}

std::optional<std::string> Parcel::getDispatchedAt() const {
    return dispatched_at;
}

std::optional<std::string> Parcel::getDeliveredAt() const {
    return delivered_at;
}

std::optional<int> Parcel::getVehicleId() const {
    return vehicle_id;
}