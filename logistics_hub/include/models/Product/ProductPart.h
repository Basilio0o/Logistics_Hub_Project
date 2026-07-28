#pragma once
#include <string>

class ProductPart {
   public:
    ProductPart();
    ProductPart(int shelf_id, int product_id, int quantity, const std::string& received_at);
    ~ProductPart() = default;

    int getShelfId() const;
    int getProductId() const;
    int getQuantity() const;
    const std::string& getReceivedAt() const;

   private:
    int shelf_id;
    int product_id;
    int quantity;
    std::string received_at;
};