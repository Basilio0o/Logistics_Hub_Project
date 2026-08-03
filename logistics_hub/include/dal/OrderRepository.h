#pragma once

#include <optional>
#include <string>
#include <vector>

#include "dal/Db.h"
#include "models/Order/Order.h"
#include "models/Order/OrderPart.h"

class OrderRepository {
   public:
    explicit OrderRepository(Db& db) : db(db) {}

    int createOrder(int district_id, const std::string& priority);
    void addOrderPart(int order_id, int product_id, int quantity);
    void updateOrderStatus(int order_id, const std::string& status);
    std::vector<Order> getOrdersByStatus(const std::string& status);
    std::optional<Order> getOrderById(int order_id);
    std::vector<OrderPart> getOrderParts(int order_id);

   private:
    Db& db;

    Order mapOrder(const pqxx::row& row);
    OrderPart mapOrderPart(const pqxx::row& row);
};