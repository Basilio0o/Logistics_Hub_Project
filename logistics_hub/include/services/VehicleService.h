#pragma once

#include <optional>
#include <string>
#include <vector>

#include "dal/Db.h"
#include "dal/DistrictRepository.h"
#include "dal/ParcelRepository.h"
#include "dal/ProductRepository.h"
#include "dal/VehicleRepository.h"
#include "models/District.h"
#include "models/Parcel/Parcel.h"
#include "models/Parcel/ParcelPart.h"
#include "models/Vehicle.h"
#include "services/AuditService.h"

class VehicleService {
   public:
    explicit VehicleService(Db& db)
        : vehiclerepo(db), parcelrepo(db), productrepo(db), districtrepo(db), audit(db) {}

    // MANAGER
    std::vector<Parcel> loadVehicle(int vehicle_id, int user_id);
    void dispatchVehicle(int vehicle_id, int user_id);
    void completeRoute(int vehicle_id);

    std::vector<Vehicle> getVehiclesByStatus(const std::string& status);
    std::optional<Vehicle> getVehicleById(int id);
    void assignDistrict(int vehicle_id, int district_id);
    void unassignDistrict(int vehicle_id, int district_id);
    std::vector<District> getVehicleDistricts(int vehicle_id);

    // CRUD для admin позже

   private:
    VehicleRepository vehiclerepo;
    ParcelRepository parcelrepo;
    ProductRepository productrepo;
    DistrictRepository districtrepo;
    AuditService audit;

    static int priorityToInt(const std::string& p);
    static bool isValidStatus(const std::string& s);
    std::optional<int> getMinShelfLife(const Parcel& parcel);
};