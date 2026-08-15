#include "services/VehicleService.h"

#include <algorithm>

std::vector<Parcel> VehicleService::loadVehicle(int vehicle_id, int user_id) {
    auto vehicle = vehiclerepo.getVehicleById(vehicle_id);
    if (!vehicle)
        throw std::runtime_error("Машина с id = " + std::to_string(vehicle_id) + " не найдена");
    if (vehicle->getStatus() != "available") {
        throw std::invalid_argument("Машина недоступна для погрузки");
    }

    vehiclerepo.updateVehicleStatus(vehicle_id, "loading");

    std::vector<District> districts = vehiclerepo.getVehicleDistricts(vehicle_id);
    std::vector<Parcel> all_parcels = parcelrepo.getParcelsByStatus("assembled");

    std::vector<Parcel> target_parcels;
    target_parcels.reserve(all_parcels.size());

    for (const auto& p : all_parcels) {
        for (const auto& d : districts) {
            if (p.getDistrictId() == d.getId()) {
                target_parcels.push_back(p);
                break;
            }
        }
    }

    std::sort(target_parcels.begin(), target_parcels.end(),
              [this](const Parcel& a, const Parcel& b) {
                  int pa = priorityToInt(a.getPriority());
                  int pb = priorityToInt(b.getPriority());
                  if (pa != pb) return pa > pb;

                  auto life_a = this->getMinShelfLife(a);
                  auto life_b = this->getMinShelfLife(b);

                  if (!life_a && !life_b) return false;
                  if (!life_a) return false;
                  if (!life_b) return true;
                  return life_a.value() < life_b.value();
              });

    double max_weight = vehicle->getMaxWeight();
    double max_volume = vehicle->getMaxVolume();
    int max_parcels = vehicle->getMaxParcels();

    double cur_w = vehicle->getCurrentWeight();
    double cur_v = vehicle->getCurrentVolume();
    int cur_p = vehicle->getCurrentParcels();

    std::vector<Parcel> loaded_ids;

    for (const auto& parcel : target_parcels) {
        if (cur_w + parcel.getWeight() <= max_weight && cur_v + parcel.getVolume() <= max_volume &&
            cur_p + 1 <= max_parcels) {
            parcelrepo.loadParcel(parcel.getId(), vehicle_id);

            cur_w += parcel.getWeight();
            cur_v += parcel.getVolume();
            cur_p++;
            loaded_ids.push_back(parcel);
        }
    }

    auditservice.log("vehicle", vehicle_id, "update",
                     "Загружено посылок: " + std::to_string(loaded_ids.size()));

    return loaded_ids;
}

int VehicleService::priorityToInt(const std::string& p) {
    if (p == "urgent") return 4;
    if (p == "high") return 3;
    if (p == "normal") return 2;
    if (p == "low") return 1;
    return 0;
}

std::optional<int> VehicleService::getMinShelfLife(const Parcel& parcel) {
    std::vector<ParcelPart> parts = parcelrepo.getParcelParts(parcel.getId());
    std::optional<int> min_days = std::nullopt;

    for (const auto& part : parts) {
        auto product = productrepo.getProductById(part.getProductId());
        if (!product) continue;

        auto shelf_life = product->getShelfLifeDays();

        if (shelf_life.has_value()) {
            if (!min_days.has_value() || shelf_life.value() < min_days.value()) {
                min_days = shelf_life;
            }
        }
    }

    return min_days;
}
