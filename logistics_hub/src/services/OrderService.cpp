#include "services/OrderService.h"

int OrderService::createOrder(int district_id, const std::string& priority) {
    if (!districtRepo.getById(district_id))
        throw std::runtime_error("Район с id " + std::to_string(district_id) + " не найден");

    isValidPriority(priority);

    return orderRepo.createOrder(district_id, priority);
}

void OrderService::addOrderPart(int order_id, int product_id, int quantity) {
    auto order = orderRepo.getOrderById(order_id);

    if (!order) throw std::runtime_error("Заказ с id " + std::to_string(order_id) + " не найден");

    if (order->getStatus() != "new")
        throw std::invalid_argument("Добавлять товары можно только в заказ со статусом 'new'");

    if (!productRepo.getProductById(product_id))
        throw std::runtime_error("Товар с id " + std::to_string(product_id) + " не найден");

    if (quantity <= 0) throw std::invalid_argument("Количество должно быть > 0");

    orderRepo.addOrderPart(order_id, product_id, quantity);
}

void OrderService::startProcessing(int order_id) {
    auto order = orderRepo.getOrderById(order_id);

    if (!order) throw std::runtime_error("Заказ с id " + std::to_string(order_id) + " не найден");

    if (order->getStatus() != "new")
        throw std::invalid_argument("Начать сборку можно только для заказ со статусом 'new'");

    orderRepo.updateOrderStatus(order_id, "processing");
    audit.log("order", order_id, "update", "");
}

void OrderService::cancelOrder(int order_id, const std::string& details) {
    auto order = orderRepo.getOrderById(order_id);

    if (!order) throw std::runtime_error("Заказ с id " + std::to_string(order_id) + " не найден");

    std::string status = order->getStatus();

    if (status == "delivered" || status == "canceled")
        throw std::invalid_argument("Нельзя отменить заказ со статусом '" + status + "'");

    orderRepo.updateOrderStatus(order_id, "canceled");

    const std::string jsonDetails = R"({"reason": ")" + escapeJson(details) + R"("})";
    audit.log("order", order_id, "update", jsonDetails);
}

std::vector<Order> OrderService::getOrdersByStatus(const std::string& status) {
    isValidStatus(status);

    return orderRepo.getOrdersByStatus(status);
}

std::vector<OrderPart> OrderService::getOrderParts(int order_id) {
    if (!orderRepo.getOrderById(order_id))
        throw std::runtime_error("Заказ с id " + std::to_string(order_id) + " не найден");

    return orderRepo.getOrderParts(order_id);
}

void OrderService::isValidPriority(const std::string& p) {
    if (p != "low" && p != "normal" && p != "high" && p != "urgent")
        throw std::invalid_argument(
            "Приоритет может быть только: 'low', 'normal', 'high', 'urgent'");
}

void OrderService::isValidStatus(const std::string& s) {
    if (s != "new" && s != "processing" && s != "assembled" && s != "dispatched" &&
        s != "delivered" && s != "canceled")
        throw std::invalid_argument(
            "Статус может быть только: 'new', 'processing', 'assembled', 'dispatched', "
            "'delivered', 'canceled'");
}

std::string OrderService::escapeJson(const std::string& s) {
    std::string out;
    for (char c : s) {
        if (c == '"' || c == '\\') out.push_back('\\');
        out.push_back(c);
    }
    return out;
}