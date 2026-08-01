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

    std::vector<Product> getAllProducts();
    std::optional<Product> getProductById(int id);
    void addOrUpdateShelfProduct(int shelf_id, int product_id, int quantity);
    std::vector<ShelfProduct> getShelfProductsByProductId(int product_id);
    void decreaseShelfProductQuantity(int shelf_id, int product_id, int quantity);

   private:
    Db& db;
    Product mapProduct(const pqxx::row& row);
    ShelfProduct mapShelfProduct(const pqxx::row& row);
};