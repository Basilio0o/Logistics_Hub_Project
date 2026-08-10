#include "dal/ProductRepository.h"

int ProductRepository::createProduct(const std::string& name, int supplier_id,
                                     const std::string& type, double unit_weight,
                                     double unit_volume, double unit_price,
                                     std::optional<int> shelf_life_days) {
    auto result = db.query_params(
        "INSERT INTO products (name, supplier_id, type, unit_weight, unit_volume, unit_price, "
        "shelf_life_days) VALUES($1, $2, $3, $4, $5, $6, $7) RETURNING id",
        name, supplier_id, type, unit_weight, unit_volume, unit_price, shelf_life_days);
    return result[0]["id"].as<int>();
}

void ProductRepository::updateProduct(int id, const std::string& name, int supplier_id,
                                      const std::string& type, double unit_weight,
                                      double unit_volume, double unit_price,
                                      std::optional<int> shelf_life_days) {
    db.execute_params(
        "UPDATE products SET name = $2, supplier_id = $3, type = $4, unit_weight = $5, "
        "unit_volume = $6, unit_price = $7, shelf_life_days = $8 WHERE id = $1",
        id, name, supplier_id, type, unit_weight, unit_volume, unit_price, shelf_life_days);
}

void ProductRepository::addOrUpdateShelfProduct(int shelf_id, int product_id, int quantity) {
    db.execute_params(
        "INSERT INTO shelf_products (shelf_id, product_id, quantity, received_at) VALUES($1, $2, "
        "$3, CURRENT_TIMESTAMP) ON CONFLICT (shelf_id, product_id) DO UPDATE SET quantity = "
        "shelf_products.quantity + EXCLUDED.quantity",
        shelf_id, product_id, quantity);
}

void ProductRepository::decreaseShelfProductQuantity(int shelf_id, int product_id, int quantity) {
    db.execute_params(
        "WITH del AS ( "
        "DELETE FROM shelf_products WHERE shelf_id = $1 AND product_id = "
        "$2 AND quantity = $3 RETURNING shelf_id "
        ") "
        "UPDATE shelf_products SET quantity = quantity - $3 WHERE shelf_id = $1 AND product_id = "
        "$2 AND quantity > $3",
        shelf_id, product_id, quantity);
}

std::vector<Product> ProductRepository::getAllProducts() {
    auto result = db.query(
        "SELECT id, name, supplier_id, type, unit_weight, unit_volume, unit_price, "
        "quantity_in_stock, shelf_life_days FROM products");
    std::vector<Product> products;
    products.reserve(result.size());

    for (const auto& row : result) {
        products.push_back(mapProduct(row));
    }
    return products;
}

std::optional<Product> ProductRepository::getProductById(int id) {
    auto result = db.query_params(
        "SELECT id, name, supplier_id, type, unit_weight, unit_volume, unit_price, "
        "quantity_in_stock, shelf_life_days FROM products WHERE id = $1",
        id);
    if (result.empty()) return std::nullopt;
    return mapProduct(result[0]);
}

std::vector<ShelfProduct> ProductRepository::getShelfProductsByProductId(int product_id) {
    auto result = db.query_params(
        "SELECT shelf_id, product_id, quantity, received_at FROM shelf_products WHERE product_id = "
        "$1 and quantity > 0 ORDER BY received_at ASC",
        product_id);
    std::vector<ShelfProduct> shelfProducts;
    shelfProducts.reserve(result.size());

    for (const auto& row : result) {
        shelfProducts.push_back(mapShelfProduct(row));
    }
    return shelfProducts;
}

Product ProductRepository::mapProduct(const pqxx::row& row) {
    return Product(row["id"].as<int>(), row["name"].as<std::string>(), row["supplier_id"].as<int>(),
                   row["type"].as<std::string>(), row["unit_weight"].as<double>(),
                   row["unit_volume"].as<double>(), row["unit_price"].as<double>(),
                   row["quantity_in_stock"].as<int>(),
                   row["shelf_life_days"].as<std::optional<int>>());
}

ShelfProduct ProductRepository::mapShelfProduct(const pqxx::row& row) {
    return ShelfProduct(row["shelf_id"].as<int>(), row["product_id"].as<int>(),
                        row["quantity"].as<int>(), row["received_at"].as<std::string>());
}