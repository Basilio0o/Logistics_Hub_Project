#pragma once

class OrderPart {
   public:
    OrderPart();
    OrderPart(int order_id, int product_id, int quantity);
    ~OrderPart() = default;

    int getOrderId() const;
    int getProductId() const;
    int getQuantity() const;

   private:
    int order_id;
    int product_id;
    int quantity;
};