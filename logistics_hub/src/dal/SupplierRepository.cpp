#include "dal/SupplierRepository.h"

int SupplierRepository::createSupplier(const std::string& name,
                                       const std::optional<std::string>& phone,
                                       const std::optional<std::string>& address) {
    auto result = db.query_params(
        "INSERT INTO suppliers (name, phone, address) VALUES($1, $2, $3) RETURNING id", name, phone,
        address);
    return result[0]["id"].as<int>();
}

void SupplierRepository::updateSupplier(int id, const std::string& name,
                                        const std::optional<std::string>& phone,
                                        const std::optional<std::string>& address) {
    db.execute_params("UPDATE suppliers SET name = $2, phone = $3, address = $4 WHERE id = $1", id,
                      name, phone, address);
}

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