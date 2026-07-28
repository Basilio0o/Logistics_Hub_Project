#include "models/Supply/SupplyPart.h"

SupplyPart::SupplyPart() : supply_id(0), product_id(0), quantity(0), received_at("") {}

SupplyPart::SupplyPart(int supply_id, int product_id, int quantity, const std::string& received_at)
    : supply_id(supply_id), product_id(product_id), quantity(quantity), received_at(received_at) {}

int SupplyPart::getSupplyId() const {
    return supply_id;
}

int SupplyPart::getProductId() const {
    return product_id;
}

int SupplyPart::getQuantity() const {
    return quantity;
}

const std::string& SupplyPart::getReceivedAt() const {
    return received_at;
}