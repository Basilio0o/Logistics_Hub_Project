#include "dal/DistrictRepository.h"

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
    std::string sql = "SELECT id, name FROM districts WHERE id = " + std::to_string(id);
    auto result = db.query(sql);

    if (result.empty()) {
        return std::nullopt;
    }
    return mapRow(result[0]);
}

District DistrictRepository::mapRow(const pqxx::row& row) {
    return District(row["id"].as<int>(), row["name"].as<std::string>());
}