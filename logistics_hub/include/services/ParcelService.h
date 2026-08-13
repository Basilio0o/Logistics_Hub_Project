#pragma once

#include <optional>
#include <string>
#include <vector>

#include "dal/DistrictRepository.h"
#include "dal/OrderRepository.h"
#include "dal/ProductRepository.h"
#include "models/Parcel/Parcel.h"
#include "models/Parcel/ParcelPart.h"
#include "services/AuditService.h"

class ParcelService {
   public:
    explicit ParcelService(Db& db) : productRepo(db), orderRepo(db), districtRepo(db), audit(db) {}

    // ASSEMBLER
    Parcel assembleParcel(int parcel_id, int user_id);
    std::vector<ParcelPart> getParcelParts(int parcel_id);

    // MANAGER
    int createParcelFromOrder(int order_id, const std::string& type);
    void addParcelPart(int parcel_id, int product_id, int quantity);

    std::vector<Parcel> getParcelsByStatus(const std::string& status);
    std::optional<Parcel> getParcelById(int parcel_id);

   private:
    ProductRepository productRepo;
    OrderRepository orderRepo;
    DistrictRepository districtRepo;
    AuditService audit;

    static void isValidPriority(const std::string& p);
    static void isValidType(const std::string& t);
    static void isValidStatus(const std::string& s);
    static std::string escapeJson(const std::string& s);
};