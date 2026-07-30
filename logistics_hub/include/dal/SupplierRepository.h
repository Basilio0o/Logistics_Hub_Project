#pragma once
#include <optional>
#include <string>
#include <vector>

#include "dal/Db.h"
#include "models/Supplier.h"

class SupplierRepository {
   public:
    explicit SupplierRepository(Db& db) : db(db) {}

    std::vector<Supplier> getAll();
    std::optional<Supplier> getById(int id);
    std::optional<Supplier> getByName(const std::string& name);

   private:
    Db& db;
    Supplier mapRow(const pqxx::row& row);
};