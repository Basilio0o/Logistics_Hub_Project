#pragma once
#include <string>

class Rack {
   public:
    Rack();
    Rack(int id, int zone_id, const std::string& code, double max_weight, double max_volume,
         double current_weight, double current_volume);
    ~Rack() = default;

    int getId() const;
    int getZoneId() const;
    const std::string& getCode() const;
    double getMaxWeight() const;
    double getMaxVolume() const;
    double getCurrentWeight() const;
    double getCurrentVolume() const;

   private:
    int id;
    int zone_id;
    std::string code;
    double max_weight;
    double max_volume;
    double current_weight;
    double current_volume;
};