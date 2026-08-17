#pragma once

#include <optional>
#include <string>
#include <vector>

#include "dal/DistrictRepository.h"
#include "models/District.h"
#include "services/AuditService.h"

class DistrictService {
   public:
    DistrictService(Db& db) : districtrepo(db), audit(db) {}

    int createDistrict(const std::string& name);
    void updateDistrict(int id, const std::string& name);

    std::vector<District> getAllDistricts();
    std::optional<District> getDistrictById(int id);

   private:
    DistrictRepository districtrepo;
    AuditService audit;

    void isValidName(const std::string& name) const;
    static std::string escapeJson(const std::string& s);
};