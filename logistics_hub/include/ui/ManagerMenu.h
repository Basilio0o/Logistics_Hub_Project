#pragma once
#include <string>
#include <vector>

#include "dal/Db.h"
#include "services/AuditService.h"
#include "services/DistrictService.h"
#include "services/OrderService.h"
#include "services/ParcelService.h"
#include "services/VehicleService.h"
#include "services/WarehouseService.h"

class ManagerMenu {
   public:
    explicit ManagerMenu(Db& db, int user_id)
        : orderService(db),
          parcelService(db),
          vehicleService(db),
          warehouseService(db),
          districtService(db),
          audit(db),
          user_id(user_id) {}
    int run();

   private:
    OrderService orderService;
    ParcelService parcelService;
    VehicleService vehicleService;
    WarehouseService warehouseService;
    DistrictService districtService;
    AuditService audit;
    int user_id;

    // Пункты меню
    void showDashboard();
    void generateRandomOrder();
    void generateRandomSupply();
    void generateBatchOrders();
    void generateBatchSupplies();
    void startProcessingOrder();
    void showCanceledOrders();
    void managePendingSupplies();
    void manageParcels();
    void manageVehicle();
};