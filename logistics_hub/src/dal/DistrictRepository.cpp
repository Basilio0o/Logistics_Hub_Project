#include "dal/DistrictRepository.h"

int DistrictRepository::createDistrict(const std::string& name) {
    auto result = db.query_params("INSERT INTO districts (name) VALUES($1) RETURNING id", name);
    return result[0]["id"].as<int>();
}

void DistrictRepository::updateDistrict(int id, const std::string& name) {
    db.execute_params("UPDATE districts SET name = $2 WHERE id = $1", id, name);
}

std::vector<District> DistrictRepository::getAll() {
    auto result = db.query("SELECT id, name FROM districts");

    std::vector<District> districts;
    districts.reserve(result.size());

    for (const auto& row : result) {
        districts.push_back(mapRow(row));
    }
    return districts;
}

std::optional<District> DistrictRepository::getById(int id) {
    std::string sql = "SELECT id, name FROM districts WHERE id = $1";
    auto result = db.query_params(sql, id);

    if (result.empty()) {
        return std::nullopt;
    }
    return mapRow(result[0]);
}

std::optional<District> DistrictRepository::getDistrictByName(const std::string& name) {
    std::string sql = "SELECT id, name FROM districts WHERE name = $1";
    auto result = db.query_params(sql, name);

    if (result.empty()) {
        return std::nullopt;
    }
    return mapRow(result[0]);
}

District DistrictRepository::mapRow(const pqxx::row& row) {
    return District(row["id"].as<int>(), row["name"].as<std::string>());
}