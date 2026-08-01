#include "dal/ProductRepository.h"

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

void ProductRepository::addOrUpdateShelfProduct(int shelf_id, int product_id, int quantity) {
    db.execute_params(
        "INSERT INTO shelf_products (shelf_id, product_id, quantity, received_at) VALUES($1, $2, "
        "$3, CURRENT_TIMESTAMP) ON CONFLICT (shelf_id, product_id) DO UPDATE SET quantity = "
        "shelf_products.quantity + EXCLUDED.quantity, received_at = CURRENT_TIMESTAMP",
        shelf_id, product_id, quantity);
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

void ProductRepository::decreaseShelfProductQuantity(int shelf_id, int product_id, int quantity) {
    db.execute_params(
        "UPDATE shelf_products set quantity = quantity - $3 WHERE shelf_id = $1 "
        "AND product_id = $2 AND quantity >= $3",
        shelf_id, product_id, quantity);
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