#include "dal/VehicleRepository.h"

int VehicleRepository::createVehicle(const std::string& number, const std::string& driver,
                                     double max_weight, double max_volume, int max_parcels) {
    auto result = db.query_params(
        "INSERT INTO vehicles (number, driver, max_weight, max_volume, max_parcels) "
        "VALUES($1, $2, $3, $4, $5) RETURNING id",
        number, driver, max_weight, max_volume, max_parcels);
    return result[0]["id"].as<int>();
}

void VehicleRepository::updateVehicle(int id, const std::string& number, const std::string& driver,
                                      double max_weight, double max_volume, int max_parcels) {
    db.execute_params(
        "UPDATE vehicles SET number = $2, driver = $3, max_weight = $4, "
        "max_volume = $5, max_parcels = $6 "
        "WHERE id = $1",
        id, number, driver, max_weight, max_volume, max_parcels);
}

void VehicleRepository::assignDistrict(int vehicle_id, int district_id) {
    db.execute_params(
        "INSERT INTO vehicle_districts (vehicle_id, district_id) "
        "VALUES($1, $2) "
        "ON CONFLICT (vehicle_id, district_id) DO NOTHING",
        vehicle_id, district_id);
}

void VehicleRepository::unassignDistrict(int vehicle_id, int district_id) {
    db.execute_params("DELETE FROM vehicle_districts WHERE vehicle_id = $1 AND district_id = $2",
                      vehicle_id, district_id);
}

std::vector<District> VehicleRepository::getVehicleDistricts(int vehicle_id) {
    auto result = db.query_params(
        "SELECT d.id, d.name "
        "FROM vehicle_districts vd "
        "JOIN districts d ON d.id = vd.district_id "
        "WHERE vd.vehicle_id = $1 "
        "ORDER BY d.name",
        vehicle_id);

    std::vector<District> districts;
    districts.reserve(result.size());

    for (const auto& row : result) {
        districts.emplace_back(row["id"].as<int>(), row["name"].as<std::string>());
    }
    return districts;
}

std::optional<Vehicle> VehicleRepository::getVehicleById(int id) {
    auto result = db.query_params("SELECT * FROM vehicles WHERE id = $1", id);

    if (result.empty()) {
        return std::nullopt;
    }
    return mapVehicle(result[0]);
}

std::vector<Vehicle> VehicleRepository::getAllVehicles() {
    auto result = db.query("SELECT * FROM vehicles ORDER BY id");

    std::vector<Vehicle> vehicles;
    vehicles.reserve(result.size());

    for (const auto& row : result) {
        vehicles.push_back(mapVehicle(row));
    }
    return vehicles;
}

std::vector<Vehicle> VehicleRepository::getVehiclesByStatus(const std::string& status) {
    auto result = db.query_params("SELECT * FROM vehicles WHERE status = $1 ORDER BY id", status);

    std::vector<Vehicle> vehicles;
    vehicles.reserve(result.size());

    for (const auto& row : result) {
        vehicles.push_back(mapVehicle(row));
    }
    return vehicles;
}

void VehicleRepository::updateVehicleStatus(int vehicle_id, const std::string& status) {
    db.execute_params("UPDATE vehicles SET status = $2 WHERE id = $1", vehicle_id, status);
}

void VehicleRepository::sendOnRoute(int vehicle_id, int sent_by) {
    db.execute_params("UPDATE vehicles SET status = 'on_route', dispatched_by = $2 WHERE id = $1",
                      vehicle_id, sent_by);
}

void VehicleRepository::completeRoute(int vehicle_id) {
    db.execute_params(
        "UPDATE vehicles SET status = 'available', dispatched_by = NULL WHERE id = $1", vehicle_id);
}

Vehicle VehicleRepository::mapVehicle(const pqxx::row& row) {
    return Vehicle(row["id"].as<int>(), row["number"].as<std::string>(),
                   row["driver"].as<std::string>(), row["max_weight"].as<double>(),
                   row["max_volume"].as<double>(), row["max_parcels"].as<int>(),
                   row["current_weight"].as<double>(), row["current_volume"].as<double>(),
                   row["current_parcels"].as<int>(), row["status"].as<std::string>(),
                   row["dispatched_by"].as<std::optional<int>>());
}