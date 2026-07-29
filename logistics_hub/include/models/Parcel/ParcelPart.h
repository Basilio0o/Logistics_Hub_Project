#pragma once

class ParcelPart {
   public:
    ParcelPart();
    ParcelPart(int parcel_id, int product_id, int quantity);
    ~ParcelPart() = default;

    int getParcelId() const;
    int getProductId() const;
    int getQuantity() const;

   private:
    int parcel_id;
    int product_id;
    int quantity;
};