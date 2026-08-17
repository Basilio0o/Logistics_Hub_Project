#pragma once

#include <optional>
#include <string>
#include <vector>

#include "dal/ProductRepository.h"
#include "dal/SupplierRepository.h"
#include "dal/SupplyRepository.h"
#include "dal/WarehouseRepository.h"
#include "models/Product/Product.h"
#include "models/Supplier.h"
#include "models/Supply/Supply.h"
#include "models/Supply/SupplyPart.h"
#include "services/AuditService.h"

struct PlacementResult {
    std::vector<std::pair<int, int>> placements;
    int placed = 0;
    int remaining = 0;
    bool isFull() const { return remaining == 0; }
};

class WarehouseService {
   public:
    explicit WarehouseService(Db& db)
        : warehouseRepo(db), productRepo(db), supplyRepo(db), supplierRepo(db), audit(db) {}

    // RECEIVER
    PlacementResult placeProduct(int product_id, int quantity);
    PlacementResult placeSupplyProduct(int supply_id, int product_id, int quantity);
    void acceptSupply(int supply_id, int accepted_by, const std::string& details = "");
    std::vector<Supply> getPendingSupplies();
    std::vector<SupplyPart> getSupplyParts(int supply_id);

    // MANAGER
    int createSupply(int supplier_id);
    void addSupplyPart(int supply_id, int product_id, int quantity);
    void rejectSupply(int supply_id, const std::string& reason);
    std::optional<Supply> getSupplyById(int supply_id);
    // void moveProduct(int from_shelf_id, int to_shelf_id, int product_id, int quantity, const
    // std::string& reason);

    // ADMIN
    int createSupplier(const std::string& name, const std::optional<std::string>& phone,
                       const std::optional<std::string>& address);
    void updateSupplier(int id, const std::string& name, const std::optional<std::string>& phone,
                        const std::optional<std::string>& address);
    std::vector<Supplier> getAllSuppliers();
    int createProduct(const std::string& name, int supplier_id, const std::string& type,
                      double unit_weight, double unit_volume, double unit_price,
                      std::optional<int> shelf_life_days);
    void updateProduct(int id, const std::string& name, int supplier_id, const std::string& type,
                       double unit_weight, double unit_volume, double unit_price,
                       std::optional<int> shelf_life_days);
    std::vector<Product> getAllProducts();
    std::optional<Product> getProductById(int id);
    // CRUD зон/стеллажей/полок для admin-меню; статистика заполненности

   private:
    WarehouseRepository warehouseRepo;
    ProductRepository productRepo;
    SupplyRepository supplyRepo;
    SupplierRepository supplierRepo;
    AuditService audit;

    static std::string zoneNameForType(const std::string& product_type);
    static std::string escapeJson(const std::string& s);
};