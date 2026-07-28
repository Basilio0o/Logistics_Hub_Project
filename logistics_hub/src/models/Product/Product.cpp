#include "models/Product/Product.h"

Product::Product()
    : id(0),
      name(""),
      supplier_id(0),
      type(""),
      unit_weight(0.0),
      unit_volume(0.0),
      unit_price(0.0),
      quantity_in_stock(0),
      shelf_life_days(std::nullopt) {}

Product::Product(int id, const std::string& name, int supplier_id, const std::string& type,
                 double unit_weight, double unit_volume, double unit_price, int quantity_in_stock,
                 std::optional<int> shelf_life_days)
    : id(id),
      name(name),
      supplier_id(supplier_id),
      type(type),
      unit_weight(unit_weight),
      unit_volume(unit_volume),
      unit_price(unit_price),
      quantity_in_stock(quantity_in_stock),
      shelf_life_days(shelf_life_days) {}

int Product::getId() const {
    return id;
}

const std::string& Product::getName() const {
    return name;
}

int Product::getSupplierId() const {
    return supplier_id;
}

const std::string& Product::getType() const {
    return type;
}

double Product::getUnitWeight() const {
    return unit_weight;
}

double Product::getUnitVolume() const {
    return unit_volume;
}

double Product::getUnitPrice() const {
    return unit_price;
}

int Product::getQuantityInStock() const {
    return quantity_in_stock;
}

std::optional<int> Product::getShelfLifeDays() const {
    return shelf_life_days;
}