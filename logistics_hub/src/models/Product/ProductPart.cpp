#include "models/Product/ProductPart.h"

ProductPart::ProductPart() : shelf_id(0), product_id(0), quantity(0), received_at("") {}

ProductPart::ProductPart(int shelf_id, int product_id, int quantity, const std::string& received_at)
    : shelf_id(shelf_id), product_id(product_id), quantity(quantity), received_at(received_at) {}

int ProductPart::getShelfId() const {
    return shelf_id;
}

int ProductPart::getProductId() const {
    return product_id;
}

int ProductPart::getQuantity() const {
    return quantity;
}

const std::string& ProductPart::getReceivedAt() const {
    return received_at;
}