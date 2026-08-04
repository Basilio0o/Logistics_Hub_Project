#include "dal/ParcelRepository.h"

int ParcelRepository::createParcel(int order_id, int district_id, const std::string& type,
                                   const std::string& priority) {
    auto result = db.query_params(
        "INSERT INTO parcels (order_id, district_id, type, weight, volume, priority) "
        "VALUES($1, $2, $3, 0, 0, $4) RETURNING id",
        order_id, district_id, type, priority);
    return result[0]["id"].as<int>();
}

void ParcelRepository::addParcelPart(int parcel_id, int product_id, int quantity) {
    db.execute_params(
        "INSERT INTO parcel_items (parcel_id, product_id, quantity) "
        "VALUES($1, $2, $3) "
        "ON CONFLICT (parcel_id, product_id) DO UPDATE "
        "SET quantity = parcel_items.quantity + EXCLUDED.quantity",
        parcel_id, product_id, quantity);
}

void ParcelRepository::updateParcelStatus(int parcel_id, const std::string& status) {
    db.execute_params("UPDATE parcels SET status = $2 WHERE id = $1", parcel_id, status);
}

void ParcelRepository::completeAssembly(int parcel_id, int user_id) {
    db.execute_params(
        "UPDATE parcels SET status = 'assembled', "
        "assembled_by = $2 "
        "WHERE id = $1",
        parcel_id, user_id);
}

void ParcelRepository::loadParcel(int parcel_id, int vehicle_id) {
    db.execute_params(
        "UPDATE parcels SET status = 'loaded', "
        "vehicle_id = $2 "
        "WHERE id = $1",
        parcel_id, vehicle_id);
}

std::optional<Parcel> ParcelRepository::getParcelById(int id) {
    auto result = db.query_params("SELECT * FROM parcels WHERE id = $1", id);

    if (result.empty()) {
        return std::nullopt;
    }
    return mapParcel(result[0]);
}

std::vector<ParcelPart> ParcelRepository::getParcelParts(int parcel_id) {
    auto result = db.query_params("SELECT * FROM parcel_items WHERE parcel_id = $1", parcel_id);

    std::vector<ParcelPart> parcelParts;
    parcelParts.reserve(result.size());

    for (const auto& row : result) {
        parcelParts.push_back(mapParcelPart(row));
    }
    return parcelParts;
}
std::vector<Parcel> ParcelRepository::getParcelsByStatus(const std::string& status) {
    auto result = db.query_params(
        "SELECT * FROM parcels "
        "WHERE status = $1 "
        "ORDER BY CASE priority "
        "WHEN 'urgent' THEN 1 WHEN 'high' THEN 2 "
        "WHEN 'normal' THEN 3 ELSE 4 "
        "END, "
        "created_at",
        status);

    std::vector<Parcel> parcels;
    parcels.reserve(result.size());

    for (const auto& row : result) {
        parcels.push_back(mapParcel(row));
    }
    return parcels;
}

Parcel ParcelRepository::mapParcel(const pqxx::row& row) {
    return Parcel(row["id"].as<int>(), row["order_id"].as<int>(), row["district_id"].as<int>(),
                  row["type"].as<std::string>(), row["weight"].as<double>(),
                  row["volume"].as<double>(), row["priority"].as<std::string>(),
                  row["status"].as<std::string>(), row["assembled_by"].as<std::optional<int>>(),
                  row["created_at"].as<std::string>(),
                  row["assembled_at"].as<std::optional<std::string>>(),
                  row["dispatched_at"].as<std::optional<std::string>>(),
                  row["delivered_at"].as<std::optional<std::string>>(),
                  row["vehicle_id"].as<std::optional<int>>());
}
ParcelPart ParcelRepository::mapParcelPart(const pqxx::row& row) {
    return ParcelPart(row["parcel_id"].as<int>(), row["product_id"].as<int>(),
                      row["quantity"].as<int>());
}