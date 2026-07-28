#pragma once
#include <string>

class Zone {
   public:
    Zone();
    Zone(int id, int warehouse_id, const std::string& name, const std::string& code,
         double max_weight, double max_volume, double current_weight, double current_volume);
    ~Zone() = default;

    int getId() const;
    int getWarehouseId() const;
    const std::string& getName() const;
    const std::string& getCode() const;
    double getMaxWeight() const;
    double getMaxVolume() const;
    double getCurrentWeight() const;
    double getCurrentVolume() const;

   private:
    int id;
    int warehouse_id;
    std::string name;
    std::string code;
    double max_weight;
    double max_volume;
    double current_weight;
    double current_volume;
};