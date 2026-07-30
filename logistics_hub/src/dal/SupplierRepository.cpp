#include "dal/SupplierRepository.h"

std::vector<Supplier> SupplierRepository::getAll() {
    auto result = db.query("SELECT id, name, phone, address FROM suppliers");

    std::vector<Supplier> suppliers;
    suppliers.reserve(result.size());

    for (const auto& row : result) {
        suppliers.push_back(mapRow(row));
    }
    return suppliers;
}

std::optional<Supplier> SupplierRepository::getById(int id) {
    std::string sql = "SELECT id, name, phone, address FROM suppliers WHERE id = $1";
    auto result = db.query_params(sql, id);

    if (result.empty()) {
        return std::nullopt;
    }
    return mapRow(result[0]);
}

std::optional<Supplier> SupplierRepository::getByName(const std::string& name) {
    std::string sql = "SELECT id, name, phone, address FROM suppliers WHERE name = $1";
    auto result = db.query_params(sql, name);

    if (result.empty()) {
        return std::nullopt;
    }
    return mapRow(result[0]);
}

Supplier SupplierRepository::mapRow(const pqxx::row& row) {
    return Supplier(row["id"].as<int>(), row["name"].as<std::string>(),
                    row["phone"].as<std::optional<std::string>>(),
                    row["address"].as<std::optional<std::string>>());
}