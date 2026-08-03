#include "dal/OrderRepository.h"

int OrderRepository::createOrder(int district_id, const std::string& priority) {
    auto result = db.query_params(
        "INSERT INTO orders (district_id, priority) "
        "VALUES ($1, $2) RETURNING id",
        district_id, priority);
    return result[0]["id"].as<int>();
}

void OrderRepository::addOrderPart(int order_id, int product_id, int quantity) {
    db.execute_params(
        "INSERT INTO order_items (order_id, product_id, quantity) "
        "VALUES ($1, $2, $3) "
        "ON CONFLICT (order_id, product_id) DO UPDATE "
        "SET quantity = order_items.quantity + EXCLUDED.quantity",
        order_id, product_id, quantity);
}

void OrderRepository::updateOrderStatus(int order_id, const std::string& status) {
    db.execute_params("UPDATE orders SET status = $2 WHERE id = $1", order_id, status);
}

std::vector<Order> OrderRepository::getOrdersByStatus(const std::string& status) {
    auto result = db.query_params(
        "SELECT id, district_id, priority, status, created_at "
        "FROM orders WHERE status = $1 ORDER BY created_at",
        status);
    std::vector<Order> orders;
    orders.reserve(result.size());

    for (const auto& row : result) {
        orders.push_back(mapOrder(row));
    }
    return orders;
}

std::optional<Order> OrderRepository::getOrderById(int order_id) {
    auto result = db.query_params(
        "SELECT id, district_id, priority, status, created_at "
        "FROM orders WHERE id = $1",
        order_id);
    if (result.empty()) return std::nullopt;
    return mapOrder(result[0]);
}

std::vector<OrderPart> OrderRepository::getOrderParts(int order_id) {
    auto result = db.query_params(
        "SELECT order_id, product_id, quantity "
        "FROM order_items WHERE order_id = $1 ORDER BY product_id",
        order_id);
    std::vector<OrderPart> parts;
    parts.reserve(result.size());

    for (const auto& row : result) {
        parts.push_back(mapOrderPart(row));
    }
    return parts;
}

Order OrderRepository::mapOrder(const pqxx::row& row) {
    return Order(row["id"].as<int>(), row["district_id"].as<int>(),
                 row["priority"].as<std::string>(), row["status"].as<std::string>(),
                 row["created_at"].as<std::string>());
}

OrderPart OrderRepository::mapOrderPart(const pqxx::row& row) {
    return OrderPart(row["order_id"].as<int>(), row["product_id"].as<int>(),
                     row["quantity"].as<int>());
}