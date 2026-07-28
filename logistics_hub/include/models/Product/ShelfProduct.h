#pragma once
#include <string>

class ShelfProduct {
   public:
    ShelfProduct();
    ShelfProduct(int shelf_id, int product_id, int quantity, const std::string& received_at);
    ~ShelfProduct() = default;

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