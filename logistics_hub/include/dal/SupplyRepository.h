#pragma once

#include <optional>
#include <string>
#include <vector>

#include "dal/Db.h"
#include "models/Supply/Supply.h"
#include "models/Supply/SupplyPart.h"

class SupplyRepository {
   public:
    explicit SupplyRepository(Db& db) : db(db){};

    int createSupply(int supplier_id);
    void addSupplyPart(int supply_id, int product_id, int quantity);
    void acceptSupply(int supply_id, int accepted_by);
    void rejectSupply(int supply_id);
    std::vector<Supply> getPendingSupplies();
    std::optional<Supply> getSupplyById(int supply_id);
    std::vector<SupplyPart> getSupplyParts(int supply_id);

   private:
    Db& db;

    Supply mapSupply(const pqxx::row& row);
    SupplyPart mapSupplyPart(const pqxx::row& row);
};