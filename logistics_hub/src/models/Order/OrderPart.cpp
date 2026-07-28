#include "models/Order/OrderPart.h"

OrderPart::OrderPart() : order_id(0), product_id(0), quantity(0) {}

OrderPart::OrderPart(int order_id, int product_id, int quantity)
    : order_id(order_id), product_id(product_id), quantity(quantity) {}

int OrderPart::getOrderId() const {
    return order_id;
}

int OrderPart::getProductId() const {
    return product_id;
}

int OrderPart::getQuantity() const {
    return quantity;
}