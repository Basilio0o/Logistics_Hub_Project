#include "dal/SupplyRepository.h"

int SupplyRepository::createSupply(int supplier_id) {
    auto result = db.query_params(
        "INSERT INTO supplies (supplier_id) "
        "VALUES ($1) RETURNING id",
        supplier_id);
    return result[0]["id"].as<int>();
}

void SupplyRepository::addSupplyPart(int supply_id, int product_id, int quantity) {
    db.execute_params(
        "INSERT INTO supply_items (supply_id, product_id, quantity, received_at) "
        "VALUES ($1, $2, $3, CURRENT_TIMESTAMP) "
        "ON CONFLICT (supply_id, product_id) DO UPDATE "
        "SET quantity = supply_items.quantity + EXCLUDED.quantity, "
        "received_at = CURRENT_TIMESTAMP",
        supply_id, product_id, quantity);
}

void SupplyRepository::acceptSupply(int supply_id, int accepted_by) {
    db.execute_params(
        "UPDATE supplies "
        "SET status = 'accepted', received_by = $2, received_at = CURRENT_TIMESTAMP "
        "WHERE id = $1",
        supply_id, accepted_by);
}
void SupplyRepository::rejectSupply(int supply_id) {
    db.execute_params(
        "UPDATE supplies "
        "SET status = 'rejected' "
        "WHERE id = $1",
        supply_id);
}

std::vector<Supply> SupplyRepository::getPendingSupplies() {
    auto result = db.query(
        "SELECT id, supplier_id, received_by, received_at, status "
        "FROM supplies WHERE status = 'pending' ORDER BY id");
    std::vector<Supply> supplies;
    supplies.reserve(result.size());

    for (const auto& row : result) {
        supplies.push_back(mapSupply(row));
    }
    return supplies;
}

std::optional<Supply> SupplyRepository::getSupplyById(int supply_id) {
    auto result = db.query_params(
        "SELECT id, supplier_id, received_by, received_at, status "
        "FROM supplies WHERE id = $1",
        supply_id);
    if (result.empty()) return std::nullopt;
    return mapSupply(result[0]);
}

std::vector<SupplyPart> SupplyRepository::getSupplyParts(int supply_id) {
    auto result = db.query_params(
        "SELECT supply_id, product_id, quantity, received_at "
        "FROM supply_items WHERE supply_id = $1 ORDER BY product_id",
        supply_id);
    std::vector<SupplyPart> parts;
    parts.reserve(result.size());

    for (const auto& row : result) {
        parts.push_back(mapSupplyPart(row));
    }
    return parts;
}

Supply SupplyRepository::mapSupply(const pqxx::row& row) {
    return Supply(row["id"].as<int>(), row["supplier_id"].as<int>(),
                  row["received_by"].as<std::optional<int>>(),
                  row["received_at"].as<std::optional<std::string>>(),
                  row["status"].as<std::string>());
}

SupplyPart SupplyRepository::mapSupplyPart(const pqxx::row& row) {
    return SupplyPart(row["supply_id"].as<int>(), row["product_id"].as<int>(),
                      row["quantity"].as<int>(), row["received_at"].as<std::string>());
}