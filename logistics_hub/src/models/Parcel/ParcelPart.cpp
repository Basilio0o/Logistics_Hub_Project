#include "models/Parcel/ParcelPart.h"

ParcelPart::ParcelPart() : parcel_id(0), product_id(0), quantity(0) {}

ParcelPart::ParcelPart(int parcel_id, int product_id, int quantity)
    : parcel_id(parcel_id), product_id(product_id), quantity(quantity) {}

int ParcelPart::getParcelId() const {
    return parcel_id;
}
int ParcelPart::getProductId() const {
    return product_id;
}
int ParcelPart::getQuantity() const {
    return quantity;
}