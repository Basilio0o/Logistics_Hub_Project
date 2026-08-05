#pragma once
#include <optional>
#include <string>
#include <vector>

#include "dal/Db.h"
#include "models/Product/Product.h"
#include "models/Product/ShelfProduct.h"

class ProductRepository {
   public:
    explicit ProductRepository(Db& db) : db(db) {}

    int createProduct(const std::string& name, int supplier_id, const std::string& type,
                      double unit_weight, double unit_volume, double unit_price,
                      std::optional<int> shelf_life_days);
    void updateProduct(int id, const std::string& name, int supplier_id, const std::string& type,
                       double unit_weight, double unit_volume, double unit_price,
                       std::optional<int> shelf_life_days);
    void addOrUpdateShelfProduct(int shelf_id, int product_id, int quantity);
    void decreaseShelfProductQuantity(int shelf_id, int product_id, int quantity);

    std::vector<Product> getAllProducts();
    std::optional<Product> getProductById(int id);
    std::vector<ShelfProduct> getShelfProductsByProductId(int product_id);

   private:
    Db& db;
    Product mapProduct(const pqxx::row& row);
    ShelfProduct mapShelfProduct(const pqxx::row& row);
};