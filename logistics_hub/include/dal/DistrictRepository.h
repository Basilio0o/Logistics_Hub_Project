#pragma once
#include <vector>

#include "dal/Db.h"
#include "models/District.h"

class DistrictRepository {
   public:
    explicit DistrictRepository(Db& db) : db(db) {}

    int createDistrict(const std::string& name);
    void updateDistrict(int id, const std::string& name);

    std::vector<District> getAll();
    std::optional<District> getById(int id);

   private:
    Db& db;
    District mapRow(const pqxx::row& row);
};