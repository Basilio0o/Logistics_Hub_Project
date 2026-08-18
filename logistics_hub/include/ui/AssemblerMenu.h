#pragma once
#include "dal/Db.h"
#include "services/OrderService.h"
#include "services/ParcelService.h"

class AssemblerMenu {
   public:
    explicit AssemblerMenu(Db& db, int user_id)
        : orderService(db), parcelService(db), user_id(user_id) {}
    int run();

   private:
    OrderService orderService;
    ParcelService parcelService;
    int user_id;
    int current_parcel_id = -1;

    int showProcessingOrders();
    void createParcelFromOrder();
    void showNewParcels();
    void selectParcel();
    void showParcelDetails();
    void assembleSelectedParcel();
};