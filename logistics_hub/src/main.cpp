#include <iostream>

#include "dal/Db.h"
#include "services/AuditService.h"
#include "services/OrderService.h"
#include "services/ParcelService.h"
#include "services/UserService.h"
#include "services/WarehouseService.h"

int main() {
    try {
        Db db("dbname=logistics_hub");
        std::cout << "Success" << std::endl;

        UserService userService(db);
        AuditService audit(db);
        WarehouseService warehouseService(db);
        ParcelService parcelService(db);
        OrderService orderService(db);

        // try {
        //     std::cout << "created id=" << userService.createUser("tester", "1234", "receiver")
        //               << std::endl;
        // } catch (const std::exception& e) {
        //     std::cout << "createUser: " << e.what() << "\n";
        // }

        // auto user = userService.login("tester", "1234");
        // if (!user) {
        //     std::cout << "login failed" << std::endl;
        //     return 1;
        // }

        // bool ok = audit.log("item", 1, "updae", R"({"note":"smoke test"})");
        // std::cout << "audit.log -> " << std::boolalpha << ok << std::endl;

        // for (const auto& l :
        //      audit.getLogs(user->getId(), std::nullopt, std::nullopt, std::nullopt)) {
        //     std::cout << l.getCreatedAt() << " | " << l.getEntityType() << "/" << l.getEntityId()
        //               << " | " << l.getOperation() << " | by="
        //               << (l.getPerformedBy() ? std::to_string(*l.getPerformedBy()) : "NULL")
        //               << " | " << l.getDetails() << std::endl;
        // }

        // int id = warehouseService.createSupply(1);
        // warehouseService.addSupplyPart(id, 1, 5);
        // warehouseService.addSupplyPart(id, 2, 1);
        // PlacementResult product1 = warehouseService.placeProduct(1, 5);
        // PlacementResult product2 = warehouseService.placeProduct(2, 1);

        // std::cout << "shelf | quantity | placed | remaining" << std::endl;

        // std::cout << product1.placements[0].first << ", " << product1.placements[0].second << ",
        // "
        //           << product1.placed << ", " << product1.remaining << std::endl;
        // std::cout << product2.placements[0].first << ", " << product2.placements[0].second << ",
        // "
        //           << product2.placed << ", " << product2.remaining << std::endl;

        // warehouseService.acceptSupply(id, user->getId());

        // orderService.startProcessing(1);

        // int id = parcelService.createParcelFromOrder(1, "standard");

        // auto parts = parcelService.getParcelParts(id);

        // for (const auto& part : parts) {
        //     std::cout << "Product_id : " << part.getProductId()
        //               << ", quantity : " << part.getQuantity() << std::endl;
        // }

        // auto parcel = parcelService.assembleParcel(id, user->getId());

        // std::cout << "Посылка создана, вес - " << parcel.getWeight() << ", объём - "
        //           << parcel.getVolume() << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error:" << e.what() << std::endl;
    }
    return 0;
}