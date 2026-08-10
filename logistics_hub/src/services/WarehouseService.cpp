#include "services/WarehouseService.h"

#include <algorithm>

PlacementResult WarehouseService::placeProduct(int product_id, int quantity) {
    auto product = productRepo.getProductById(product_id);
    if (!product)
        throw std::invalid_argument("Товар с id " + std::to_string(product_id) + " не найден");
    if (quantity <= 0) throw std::invalid_argument("Количество должно быть > 0");

    auto zoneName = zoneNameForType(product->getType());
    auto zone = warehouseRepo.getZoneByName(zoneName);
    if (!zone) throw std::runtime_error("Зона '" + zoneName + "' не найдена");

    auto shelves = warehouseRepo.findActiveShelvesByZoneId(zone->getId());
    if (shelves.empty()) {
        return PlacementResult{{}, 0, quantity};
    }

    std::sort(shelves.begin(), shelves.end(), [](const Shelf& a, const Shelf& b) {
        auto rw = [](const Shelf& s) {
            return s.getMaxWeight() > 0.0 ? s.getCurrentWeight() / s.getMaxWeight() : 1.0;
        };
        auto rv = [](const Shelf& s) {
            return s.getMaxVolume() > 0.0 ? s.getCurrentVolume() / s.getMaxVolume() : 1.0;
        };
        if (rw(a) < rw(b)) return true;
        if (rw(b) < rw(a)) return false;
        return rv(a) < rv(b);
    });

    PlacementResult result;

    for (const Shelf& s : shelves) {
        bool fitsWhole =
            s.getCurrentWeight() + product->getUnitWeight() * quantity <= s.getMaxWeight() &&
            s.getCurrentVolume() + product->getUnitVolume() * quantity <= s.getMaxVolume();
        if (fitsWhole) {
            productRepo.addOrUpdateShelfProduct(s.getId(), product_id, quantity);
            result.placements.push_back({s.getId(), quantity});
            result.placed = quantity;
            result.remaining = 0;
            return result;
        }
    }

    int remaining = quantity;
    for (const Shelf& s : shelves) {
        if (remaining == 0) break;

        int byWeight =
            static_cast<int>((s.getMaxWeight() - s.getCurrentWeight()) / product->getUnitWeight());
        int byVolume =
            static_cast<int>((s.getMaxVolume() - s.getCurrentVolume()) / product->getUnitVolume());
        int take = std::min({byWeight, byVolume, remaining});

        if (take > 0) {
            productRepo.addOrUpdateShelfProduct(s.getId(), product_id, take);
            result.placements.push_back({s.getId(), take});
            result.placed += take;
            remaining -= take;
        }
    }

    result.remaining = remaining;
    return result;
}

std::string WarehouseService::zoneNameForType(const std::string& product_type) {
    if (product_type == "regular") return "Обычная";
    if (product_type == "perishable") return "Холодная";
    if (product_type == "oversized") return "Крупногабаритная";
    throw std::invalid_argument("Неизвестный тип товара: " + product_type);
}