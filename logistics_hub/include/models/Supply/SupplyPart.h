#pragma once
#include <string>

class SupplyPart {
   public:
    SupplyPart();
    SupplyPart(int supply_id, int product_id, int quantity, const std::string& received_at);
    ~SupplyPart() = default;

    int getSupplyId() const;
    int getProductId() const;
    int getQuantity() const;
    const std::string& getReceivedAt() const;

   private:
    int supply_id;
    int product_id;
    int quantity;
    std::string received_at;
};