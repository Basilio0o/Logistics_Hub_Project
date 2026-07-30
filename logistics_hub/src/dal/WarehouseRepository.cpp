#include "dal/WarehouseRepository.h"

std::vector<Warehouse> WarehouseRepository::getAllWarehouses() {
    auto result = db.query("SELECT id, name, address, square FROM warehouses");
    std::vector<Warehouse> warehouses;
    warehouses.reserve(result.size());
    for (const auto& row : result) {
        warehouses.push_back(mapWarehouse(row));
    }
    return warehouses;
}

std::optional<Warehouse> WarehouseRepository::getWarehouseById(int id) {
    auto result =
        db.query_params("SELECT id, name, address, square FROM warehouses WHERE id = $1", id);
    if (result.empty()) return std::nullopt;
    return mapWarehouse(result[0]);
}

std::vector<Zone> WarehouseRepository::getAllZones() {
    auto result = db.query(
        "SELECT id, warehouse_id, name, code, max_weight, max_volume, current_weight, "
        "current_volume FROM zones");
    std::vector<Zone> zones;
    zones.reserve(result.size());
    for (const auto& row : result) {
        zones.push_back(mapZone(row));
    }
    return zones;
}

std::optional<Zone> WarehouseRepository::getZoneByName(const std::string& name) {
    auto result = db.query_params(
        "SELECT id, warehouse_id, name, code, max_weight, max_volume, "
        "current_weight, current_volume FROM zones WHERE name = $1",
        name);
    if (result.empty()) return std::nullopt;
    return mapZone(result[0]);
}

void WarehouseRepository::updateZoneCurrent(int zoneId, double weightDelta, double volumeDelta) {
    db.execute_params(
        "UPDATE zones SET current_weight = current_weight + $2, "
        "current_volume = current_volume + $3 WHERE id = $1",
        zoneId, weightDelta, volumeDelta);
}

std::vector<Rack> WarehouseRepository::getRacksByZoneId(int zoneId) {
    auto result = db.query_params(
        "SELECT id, zone_id, code, max_weight, max_volume, "
        "current_weight, current_volume FROM racks WHERE zone_id = $1",
        zoneId);
    std::vector<Rack> racks;
    racks.reserve(result.size());
    for (const auto& row : result) {
        racks.push_back(mapRack(row));
    }
    return racks;
}

void WarehouseRepository::updateRackCurrent(int rackId, double weightDelta, double volumeDelta) {
    db.execute_params(
        "UPDATE racks SET current_weight = current_weight + $2, "
        "current_volume = current_volume + $3 WHERE id = $1",
        rackId, weightDelta, volumeDelta);
}

std::vector<Shelf> WarehouseRepository::getShelvesByRackId(int rackId) {
    auto result = db.query_params(
        "SELECT id, rack_id, code, max_weight, max_volume, "
        "current_weight, current_volume, status FROM shelves WHERE rack_id = $1",
        rackId);
    std::vector<Shelf> shelves;
    shelves.reserve(result.size());
    for (const auto& row : result) {
        shelves.push_back(mapShelf(row));
    }
    return shelves;
}

std::vector<Shelf> WarehouseRepository::findActiveShelvesByZoneId(int zoneId) {
    auto result = db.query_params(
        "SELECT s.id, s.rack_id, s.code, s.max_weight, s.max_volume, "
        "s.current_weight, s.current_volume, s.status "
        "FROM shelves s "
        "JOIN racks r ON s.rack_id = r.id "
        "WHERE r.zone_id = $1 AND s.status = 'active'",
        zoneId);
    std::vector<Shelf> shelves;
    shelves.reserve(result.size());
    for (const auto& row : result) {
        shelves.push_back(mapShelf(row));
    }
    return shelves;
}

void WarehouseRepository::updateShelfCurrent(int shelfId, double weightDelta, double volumeDelta) {
    db.execute_params(
        "UPDATE shelves SET current_weight = current_weight + $2, "
        "current_volume = current_volume + $3 WHERE id = $1",
        shelfId, weightDelta, volumeDelta);
}

Warehouse WarehouseRepository::mapWarehouse(const pqxx::row& row) {
    return Warehouse(row["id"].as<int>(), row["name"].as<std::string>(),
                     row["address"].as<std::string>(), row["square"].as<double>());
}

Zone WarehouseRepository::mapZone(const pqxx::row& row) {
    return Zone(row["id"].as<int>(), row["warehouse_id"].as<int>(), row["name"].as<std::string>(),
                row["code"].as<std::string>(), row["max_weight"].as<double>(),
                row["max_volume"].as<double>(), row["current_weight"].as<double>(),
                row["current_volume"].as<double>());
}

Rack WarehouseRepository::mapRack(const pqxx::row& row) {
    return Rack(row["id"].as<int>(), row["zone_id"].as<int>(), row["code"].as<std::string>(),
                row["max_weight"].as<double>(), row["max_volume"].as<double>(),
                row["current_weight"].as<double>(), row["current_volume"].as<double>());
}

Shelf WarehouseRepository::mapShelf(const pqxx::row& row) {
    return Shelf(row["id"].as<int>(), row["rack_id"].as<int>(), row["code"].as<std::string>(),
                 row["max_weight"].as<double>(), row["max_volume"].as<double>(),
                 row["current_weight"].as<double>(), row["current_volume"].as<double>(),
                 row["status"].as<std::string>());
}