#pragma once
#include <optional>
#include <string>
#include <vector>

#include "dal/Db.h"
#include "models/Supplier.h"

class SupplierRepository {
   public:
    explicit SupplierRepository(Db& db) : db(db) {}

    int createSupplier(const std::string& name, const std::optional<std::string>& phone,
                       const std::optional<std::string>& address);
    void updateSupplier(int id, const std::string& name, const std::optional<std::string>& phone,
                        const std::optional<std::string>& address);

    std::vector<Supplier> getAll();
    std::optional<Supplier> getById(int id);
    std::optional<Supplier> getByName(const std::string& name);

   private:
    Db& db;
    Supplier mapRow(const pqxx::row& row);
};