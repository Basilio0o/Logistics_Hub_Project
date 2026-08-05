#pragma once

#include <optional>
#include <string>
#include <vector>

#include "dal/Db.h"
#include "models/District.h"
#include "models/Vehicle.h"

class VehicleRepository {
   public:
    explicit VehicleRepository(Db& db) : db(db) {}

    int createVehicle(const std::string& number, const std::string& driver, double max_weight,
                      double max_volume, int max_parcels);
    void updateVehicle(int id, const std::string& number, const std::string& driver,
                       double max_weight, double max_volume, int max_parcels);

    void assignDistrict(int vehicle_id, int district_id);
    void unassignDistrict(int vehicle_id, int district_id);
    std::vector<District> getVehicleDistricts(int vehicle_id);

    std::optional<Vehicle> getVehicleById(int id);
    std::vector<Vehicle> getAllVehicles();
    std::vector<Vehicle> getVehiclesByStatus(const std::string& status);

    void updateVehicleStatus(int vehicle_id, const std::string& status);
    void sendOnRoute(int vehicle_id, int sent_by);
    void completeRoute(int vehicle_id);

   private:
    Db& db;
    Vehicle mapVehicle(const pqxx::row& row);
};