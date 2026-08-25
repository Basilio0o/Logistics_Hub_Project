#pragma once
#include <string>

#include "dal/Db.h"
#include "services/WarehouseService.h"

class AdminMenu {
   public:
    explicit AdminMenu(Db& db, int user_id) : warehouseService(db), user_id(user_id) {}

    int run();

   private:
    WarehouseService warehouseService;
    int user_id;

    void showWarehouses();
};