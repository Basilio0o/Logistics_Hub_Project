#pragma once
#include <optional>
#include <string>

class Vehicle {
   public:
    Vehicle();
    Vehicle(int id, const std::string& number, const std::string& driver, double max_weight,
            double max_volume, int max_parcels, double current_weight, double current_volume,
            int current_parcels, const std::string& status, std::optional<int> dispatched_by);
    ~Vehicle() = default;

    int getId() const;
    const std::string& getNumber() const;
    const std::string& getDriver() const;
    double getMaxWeight() const;
    double getMaxVolume() const;
    int getMaxParcels() const;
    double getCurrentWeight() const;
    double getCurrentVolume() const;
    int getCurrentParcels() const;
    const std::string& getStatus() const;
    std::optional<int> getDispatchedBy() const;

   private:
    int id;
    std::string number;
    std::string driver;
    double max_weight;
    double max_volume;
    int max_parcels;
    double current_weight;
    double current_volume;
    int current_parcels;
    std::string status;
    std::optional<int> dispatched_by;
};