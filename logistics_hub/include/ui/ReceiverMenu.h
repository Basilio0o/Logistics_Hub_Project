#pragma once
#include <map>
#include <string>

#include "dal/Db.h"
#include "services/WarehouseService.h"

class ReceiverMenu {
   public:
    explicit ReceiverMenu(Db& db, int user_id) : warehouseService(db), user_id(user_id) {}
    int run();

   private:
    WarehouseService warehouseService;
    int user_id;

    int current_supply_id = -1;

    struct ExpectedPlaced {
        int expected;
        int placed;
    };
    std::map<int, ExpectedPlaced> session;

    // Пункты меню
    int showPendingSupplies();
    void selectSupply();
    void showSupplyComposition();
    void receiveProduct();
    void finishSupply();

    // Вспомогательные
    void resetSession();
    void printSessionSummary();
    std::string buildDiscrepancyJson(const std::string& s);
    std::string escapeJson(const std::string& s);
};