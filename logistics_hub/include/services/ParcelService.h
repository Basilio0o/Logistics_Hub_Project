#pragma once

#include <optional>
#include <string>
#include <vector>

#include "dal/DistrictRepository.h"
#include "dal/OrderRepository.h"
#include "dal/ParcelRepository.h"
#include "dal/ProductRepository.h"
#include "models/Parcel/Parcel.h"
#include "models/Parcel/ParcelPart.h"
#include "models/Product/Product.h"
#include "models/Product/ShelfProduct.h"
#include "services/AuditService.h"

class ParcelService {
   public:
    explicit ParcelService(Db& db)
        : parcelRepo(db), productRepo(db), orderRepo(db), districtRepo(db), audit(db) {}

    // MANAGER
    int createParcelFromOrder(int order_id, const std::string& type);

    // ASSEMBLER
    Parcel assembleParcel(int parcel_id, int user_id);
    std::vector<ParcelPart> getParcelParts(int parcel_id);

    std::vector<Parcel> getParcelsByStatus(const std::string& status);
    std::optional<Parcel> getParcelById(int parcel_id);

   private:
    ParcelRepository parcelRepo;
    ProductRepository productRepo;
    OrderRepository orderRepo;
    DistrictRepository districtRepo;
    AuditService audit;

    static void isValidPriority(const std::string& p);
    static void isValidType(const std::string& t);
    static void isValidStatus(const std::string& s);
};