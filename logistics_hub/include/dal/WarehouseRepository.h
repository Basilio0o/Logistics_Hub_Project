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

    std::vector<Warehouse> getAllWarehouses();
    std::optional<Warehouse> getWarehouseById(int id);

    std::vector<Zone> getAllZones();
    std::optional<Zone> getZoneByName(const std::string& name);
    void updateZoneCurrent(int zoneId, double weightDelta, double volumeDelta);

    std::vector<Rack> getRacksByZoneId(int zoneId);
    void updateRackCurrent(int rackId, double weightDelta, double volumeDelta);

    std::vector<Shelf> getShelvesByRackId(int rackId);
    std::vector<Shelf> findActiveShelvesByZoneId(int zoneId);
    void updateShelfCurrent(int shelfId, double weightDelta, double volumeDelta);

   private:
    Db& db;
    Warehouse mapWarehouse(const pqxx::row& row);
    Zone mapZone(const pqxx::row& row);
    Rack mapRack(const pqxx::row& row);
    Shelf mapShelf(const pqxx::row& row);
};