#include "services/DistrictService.h"

#include <algorithm>
#include <cctype>

int DistrictService::createDistrict(const std::string& name) {
    isValidName(name);

    if (districtrepo.getDistrictByName(name))
        throw std::invalid_argument("Район с названием '" + name + "' уже существует");

    int new_id = districtrepo.createDistrict(name);
    std::string details = R"({"name": ")" + escapeJson(name) + R"("})";
    audit.log("district", new_id, "update", details);
    return new_id;
}

void DistrictService::updateDistrict(int id, const std::string& name) {
    isValidName(name);

    auto district = districtrepo.getById(id);

    if (!district) {
        throw std::runtime_error("Район с ID " + std::to_string(id) + " не найден");
    }

    if (districtrepo.getDistrictByName(name))
        throw std::invalid_argument("Район с названием '" + name + "' уже существует");

    if (district->getName() == name) return;

    districtrepo.updateDistrict(id, name);
    std::string details = R"({"name": ")" + escapeJson(name) + R"("})";
    audit.log("district", id, "update", details);
}

std::vector<District> DistrictService::getAllDistricts() {
    return districtrepo.getAll();
}

std::optional<District> DistrictService::getDistrictById(int id) {
    return districtrepo.getById(id);
}

void DistrictService::isValidName(const std::string& name) const {
    if (name.empty()) {
        throw std::invalid_argument("Название района не может быть пустым");
    }
    if (std::all_of(name.begin(), name.end(), [](unsigned char c) { return std::isspace(c); })) {
        throw std::invalid_argument("Название района не может состоять только из пробелов");
    }
}

std::string DistrictService::escapeJson(const std::string& s) {
    std::string out;
    for (char c : s) {
        if (c == '"' || c == '\\') out.push_back('\\');
        out.push_back(c);
    }
    return out;
}