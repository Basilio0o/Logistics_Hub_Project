#pragma once
#include <optional>
#include <string>

class Product {
   public:
    Product();
    Product(int id, const std::string& name, int supplier_id, const std::string& type,
            double unit_weight, double unit_volume, double unit_price, int quantity_in_stock,
            std::optional<int> shelf_life_days);
    ~Product() = default;

    int getId() const;
    const std::string& getName() const;
    int getSupplierId() const;
    const std::string& getType() const;
    double getUnitWeight() const;
    double getUnitVolume() const;
    double getUnitPrice() const;
    int getQuantityInStock() const;
    std::optional<int> getShelfLifeDays() const;

   private:
    int id;
    std::string name;
    int supplier_id;
    std::string type;
    double unit_weight;
    double unit_volume;
    double unit_price;
    int quantity_in_stock;
    std::optional<int> shelf_life_days;
};