#pragma once

#include <optional>
#include <string>
#include <vector>

#include "dal/ProductRepository.h"
#include "dal/SupplierRepository.h"
#include "dal/SupplyRepository.h"
#include "dal/WarehouseRepository.h"
#include "models/Product/Product.h"
#include "models/Product/ShelfProduct.h"
#include "models/Supplier.h"
#include "models/Supply/Supply.h"
#include "models/Supply/SupplyPart.h"
#include "models/Warehouse/Shelf.h"
#include "models/Warehouse/Zone.h"
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
    std::optional<Supplier> getSupplierById(int supplier_id);

    int createProduct(const std::string& name, int supplier_id, const std::string& type,
                      double unit_weight, double unit_volume, double unit_price,
                      std::optional<int> shelf_life_days);
    void updateProduct(int id, const std::string& name, int supplier_id, const std::string& type,
                       double unit_weight, double unit_volume, double unit_price,
                       std::optional<int> shelf_life_days);
    std::vector<Product> getAllProducts();
    std::optional<Product> getProductById(int id);
    std::vector<ShelfProduct> getShelfProductsByProductId(int product_id);

    int createWarehouse(const std::string& name, const std::string& address, double square);
    void updateWarehouse(int id, const std::string& name, const std::string& address,
                         double square);
    std::vector<Warehouse> getAllWarehouses();
    std::optional<Warehouse> getWarehouseById(int id);

    int createZone(int warehouse_id, const std::string& name, const std::string& code,
                   double max_weight, double max_volume);
    void updateZone(int id, const std::string& name, const std::string& code, double max_weight,
                    double max_volume);
    std::vector<Zone> getZonesByWarehouseId(int warehouse_id);
    std::optional<Zone> getZoneByName(const std::string& name);
    std::optional<Zone> getZoneById(int zone_id);

    int createRack(int zone_id, const std::string& code, double max_weight, double max_volume);
    void updateRack(int id, const std::string& code, double max_weight, double max_volume);
    std::vector<Rack> getRacksByZoneId(int zone_id);
    std::optional<Rack> getRackById(int rack_id);

    int createShelf(int rack_id, const std::string& code, double max_weight, double max_volume);
    void updateShelf(int id, const std::string& code, double max_weight, double max_volume);
    void updateShelfStatus(int id, const std::string& status);
    std::vector<Shelf> getShelvesByRackId(int rack_id);
    std::optional<Shelf> getShelfById(int shelf_id);

   private:
    WarehouseRepository warehouseRepo;
    ProductRepository productRepo;
    SupplyRepository supplyRepo;
    SupplierRepository supplierRepo;
    AuditService audit;

    static std::string zoneNameForType(const std::string& product_type);
    static void isValidName(const std::string& name);
    static void isValidCode(const std::string& code);
    static std::string escapeJson(const std::string& s);
};