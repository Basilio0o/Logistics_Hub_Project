#pragma once

#include <optional>
#include <string>
#include <vector>

#include "dal/DistrictRepository.h"
#include "dal/OrderRepository.h"
#include "dal/ProductRepository.h"
#include "models/Order/Order.h"
#include "models/Order/OrderPart.h"
#include "services/AuditService.h"

class OrderService {
   public:
    explicit OrderService(Db& db) : orderRepo(db), districtRepo(db), productRepo(db), audit(db) {}

    // MANAGER
    int createOrder(int district_id, const std::string& priority);
    void addOrderPart(int order_id, int product_id, int quantity);
    void startProcessing(int order_id);
    void cancelOrder(int order_id, const std::string& details);

    std::vector<Order> getOrdersByStatus(const std::string& status);
    std::vector<OrderPart> getOrderParts(int order_id);
    std::optional<Order> getOrderById(int order_id);

   private:
    OrderRepository orderRepo;
    DistrictRepository districtRepo;
    ProductRepository productRepo;
    AuditService audit;

    static void isValidPriority(const std::string& p);
    static void isValidStatus(const std::string& s);
    static std::string escapeJson(const std::string& s);
};