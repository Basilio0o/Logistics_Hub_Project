#include "services/ParcelService.h"

#include <algorithm>

int ParcelService::createParcelFromOrder(int order_id) {
    const auto order = orderRepo.getOrderById(order_id);

    if (!order) throw std::runtime_error("Заказ с id " + std::to_string(order_id) + " не найден");

    if (order->getStatus() != "processing")
        throw std::invalid_argument("Можно собрать заказ только со статусом 'processing'");

    std::vector<OrderPart> order_items = orderRepo.getOrderParts(order_id);
    if (order_items.empty()) throw std::invalid_argument("Заказ пуст");

    std::string parcel_type = "standard";

    for (const auto& item : order_items) {
        auto product = productRepo.getProductById(item.getProductId());
        if (!product) continue;

        if (product->getType() == "perishable") {
            parcel_type = "express";
            break;
        } else if (product->getType() == "oversized") {
            parcel_type = "oversized";
        }
    }

    int parcel_id = parcelRepo.createParcel(order_id, order->getDistrictId(), parcel_type,
                                            order->getPriority());

    for (const auto& item : order_items) {
        parcelRepo.addParcelPart(parcel_id, item.getProductId(), item.getQuantity());
    }

    return parcel_id;
}

Parcel ParcelService::assembleParcel(int parcel_id, int user_id) {
    const auto parcel = parcelRepo.getParcelById(parcel_id);
    if (!parcel)
        throw std::runtime_error("Посылка с id " + std::to_string(parcel_id) + " не найдена");

    if (parcel->getStatus() != "new")
        throw std::invalid_argument("Нельзя собрать посылку со статусом '" + parcel->getStatus() +
                                    "'");

    std::vector<ParcelPart> parcel_items = parcelRepo.getParcelParts(parcel_id);
    std::vector<std::pair<int, int>> missing_products;

    for (const auto& item : parcel_items) {
        int product_id = item.getProductId();
        int required = item.getQuantity();

        auto product = productRepo.getProductById(product_id);

        std::vector<ShelfProduct> shelf_products =
            productRepo.getShelfProductsByProductId(product_id);

        int available = 0;
        for (const auto& sp : shelf_products) {
            available += sp.getQuantity();
        }

        if (available < required) {
            missing_products.push_back({product_id, required - available});
        }
    }

    if (!missing_products.empty()) {
        orderRepo.updateOrderStatus(parcel->getOrderId(), "canceled");

        audit.log("order", parcel->getOrderId(), "update", R"({"reason" : "Нехватка товаров"})");

        std::string msg = "Не хватает товаров: ";
        for (const auto& [prod_id, deficit] : missing_products) {
            msg += "id =" + std::to_string(prod_id) + " (не хватает " + std::to_string(deficit) +
                   " шт.), ";
        }
        throw std::runtime_error(msg.erase(msg.length() - 2));
    }

    for (const auto& item : parcel_items) {
        int product_id = item.getProductId();
        int remaining = item.getQuantity();

        auto product = productRepo.getProductById(product_id);
        auto shelf_products = productRepo.getShelfProductsByProductId(product_id);

        if (product->getType() == "perishable") {
            std::sort(shelf_products.begin(), shelf_products.end(),
                      [](const ShelfProduct& a, const ShelfProduct& b) {
                          return a.getReceivedAt() < b.getReceivedAt();
                      });
        } else {
            std::sort(shelf_products.begin(), shelf_products.end(),
                      [](const ShelfProduct& a, const ShelfProduct& b) {
                          return a.getQuantity() > b.getQuantity();
                      });
        }

        for (const auto& sp : shelf_products) {
            if (remaining <= 0) break;

            int take = std::min(remaining, sp.getQuantity());
            productRepo.decreaseShelfProductQuantity(sp.getShelfId(), product_id, take);
            remaining -= take;
        }
    }

    parcelRepo.completeAssembly(parcel_id, user_id);
    orderRepo.updateOrderStatus(parcel->getOrderId(), "assembled");

    audit.log("parcel", parcel_id, "assemble", "");

    return parcelRepo.getParcelById(parcel_id).value();
}

std::vector<ParcelPart> ParcelService::getParcelParts(int parcel_id) {
    if (!parcelRepo.getParcelById(parcel_id))
        throw std::runtime_error("Посылка с id " + std::to_string(parcel_id) + " не найдена");
    return parcelRepo.getParcelParts(parcel_id);
}

std::vector<Parcel> ParcelService::getParcelsByStatus(const std::string& status) {
    isValidStatus(status);
    return parcelRepo.getParcelsByStatus(status);
}

std::optional<Parcel> ParcelService::getParcelById(int parcel_id) {
    return parcelRepo.getParcelById(parcel_id);
}

void ParcelService::isValidPriority(const std::string& p) {
    if (p != "low" && p != "normal" && p != "high" && p != "urgent")
        throw std::invalid_argument(
            "Приоритет может быть только: 'low', 'normal', 'high', 'urgent'");
}

void ParcelService::isValidStatus(const std::string& s) {
    if (s != "new" && s != "assembled" && s != "grouped" && s != "loaded" && s != "dispatched" &&
        s != "delivered")
        throw std::invalid_argument(
            "Статус может быть только: 'new', 'assembled', 'grouped', 'loaded', 'dispatched', "
            "'delivered'");
}