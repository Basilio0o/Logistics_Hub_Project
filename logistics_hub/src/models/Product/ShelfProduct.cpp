#include "models/Product/ShelfProduct.h"

ShelfProduct::ShelfProduct() : shelf_id(0), product_id(0), quantity(0), received_at("") {}

ShelfProduct::ShelfProduct(int shelf_id, int product_id, int quantity,
                           const std::string& received_at)
    : shelf_id(shelf_id), product_id(product_id), quantity(quantity), received_at(received_at) {}

int ShelfProduct::getShelfId() const {
    return shelf_id;
}

int ShelfProduct::getProductId() const {
    return product_id;
}

int ShelfProduct::getQuantity() const {
    return quantity;
}

const std::string& ShelfProduct::getReceivedAt() const {
    return received_at;
}