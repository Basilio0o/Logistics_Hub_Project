#pragma once

#include <optional>
#include <string>
#include <vector>

#include "dal/Db.h"
#include "models/Parcel/Parcel.h"
#include "models/Parcel/ParcelPart.h"

class ParcelRepository {
   public:
    ParcelRepository(Db& db) : db(db) {}

    int createParcel(int order_id, int district_id, const std::string& type,
                     const std::string& priority);
    void addParcelPart(int parcel_id, int product_id, int quantity);
    void updateParcelStatus(int parcel_id, const std::string& status);
    void completeAssembly(int parcel_id, int user_id);
    void loadParcel(int parcel_id, int vehicle_id);
    std::optional<Parcel> getParcelById(int id);
    std::vector<ParcelPart> getParcelParts(int parcel_id);
    std::vector<Parcel> getParcelsByStatus(const std::string& status);

   private:
    Db& db;

    Parcel mapParcel(const pqxx::row& row);
    ParcelPart mapParcelPart(const pqxx::row& row);
};