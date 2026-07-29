#pragma once
#include <optional>
#include <string>

class Parcel {
   public:
    Parcel();
    Parcel(int id, int order_id, int district_id, const std::string& type, double weight,
           double volume, const std::string& priority, const std::string& status,
           std::optional<int> assembled_by, const std::string& created_at,
           std::optional<std::string> assembled_at, std::optional<std::string> dispatched_at,
           std::optional<std::string> delivered_at, std::optional<int> vehicle_id);
    ~Parcel() = default;

    int getId() const;
    int getOrderId() const;
    int getDistrictId() const;
    const std::string& getType() const;
    double getWeight() const;
    double getVolume() const;
    const std::string& getPriority() const;
    const std::string& getStatus() const;
    std::optional<int> getAssembledBy() const;
    const std::string& getCreatedAt() const;
    std::optional<std::string> getAssembledAt() const;
    std::optional<std::string> getDispatchedAt() const;
    std::optional<std::string> getDeliveredAt() const;
    std::optional<int> getVehicleId() const;

   private:
    int id;
    int order_id;
    int district_id;
    std::string type;
    double weight;
    double volume;
    std::string priority;
    std::string status;
    std::optional<int> assembled_by;
    std::string created_at;
    std::optional<std::string> assembled_at;
    std::optional<std::string> dispatched_at;
    std::optional<std::string> delivered_at;
    std::optional<int> vehicle_id;
};