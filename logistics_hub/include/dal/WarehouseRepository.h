#pragma once
#include <optional>
#include <string>
#include <vector>

#include "dal/Db.h"
#include "models/Warehouse/Rack.h"
#include "models/Warehouse/Shelf.h"
#include "models/Warehouse/Warehouse.h"
#include "models/Warehouse/Zone.h"

class WarehouseRepository {
   public:
    explicit WarehouseRepository(Db& db) : db(db) {}

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

    std::vector<Rack> getRacksByZoneId(int zoneId);
    std::optional<Rack> getRackById(int rack_id);

    int createShelf(int rack_id, const std::string& code, double max_weight, double max_volume);
    void updateShelf(int id, const std::string& code, double max_weight, double max_volume);
    void updateShelfStatus(int shelf_id, const std::string& status);

    std::optional<Shelf> getShelfById(int shelfId);
    std::vector<Shelf> getShelvesByRackId(int rackId);
    std::vector<Shelf> findActiveShelvesByZoneId(int zoneId);

   private:
    Db& db;
    Warehouse mapWarehouse(const pqxx::row& row);
    Zone mapZone(const pqxx::row& row);
    Rack mapRack(const pqxx::row& row);
    Shelf mapShelf(const pqxx::row& row);
};