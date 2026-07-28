#pragma once
#include <string>

class Shelf {
   public:
    Shelf();
    Shelf(int id, int rack_id, const std::string& code, double max_weight, double max_volume,
          double current_weight, double current_volume, const std::string& status);
    ~Shelf() = default;

    int getId() const;
    int getRackId() const;
    const std::string& getCode() const;
    double getMaxWeight() const;
    double getMaxVolume() const;
    double getCurrentWeight() const;
    double getCurrentVolume() const;
    const std::string& getStatus() const;

   private:
    int id;
    int rack_id;
    std::string code;
    double max_weight;
    double max_volume;
    double current_weight;
    double current_volume;
    std::string status;
};