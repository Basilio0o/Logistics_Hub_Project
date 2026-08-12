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

void WarehouseService::acceptSupply(int supply_id, int accepted_by) {
    const auto supply = supplyRepo.getSupplyById(supply_id);

    if (!supply)
        throw std::runtime_error("Поставка с id " + std::to_string(supply_id) + "не найдена");

    if (supply->getStatus() != "pending")
        throw std::invalid_argument("Принять можно только поставку со статусом pending");

    supplyRepo.acceptSupply(supply_id, accepted_by);

    audit.log("shipment", supply_id, "receive", "");
}

void WarehouseService::rejectSupply(int supply_id, const std::string& reason) {
    const auto supply = supplyRepo.getSupplyById(supply_id);

    if (!supply)
        throw std::runtime_error("Поставка с id " + std::to_string(supply_id) + "не найдена");

    if (supply->getStatus() != "pending")
        throw std::invalid_argument("Отклонить можно только поставку со статусом pending");

    supplyRepo.rejectSupply(supply_id);

    const std::string details = R"({"reason": ")" + escapeJson(reason) + R"("})";
    audit.log("shipment", supply_id, "update", details);
}

int WarehouseService::createSupply(int supplier_id) {
    if (!supplierRepo.getById(supplier_id)) {
        throw std::runtime_error("Поставщик с id " + std::to_string(supplier_id) + " не найден");
    }
    return supplyRepo.createSupply(supplier_id);
}

void WarehouseService::addSupplyPart(int supply_id, int product_id, int quantity) {
    if (quantity <= 0) {
        throw std::invalid_argument("Количество должно быть больше нуля");
    }

    const auto supply = supplyRepo.getSupplyById(supply_id);

    if (!supply) {
        throw std::runtime_error("Поставка с id " + std::to_string(supply_id) + "не найдена");
    }
    if (supply->getStatus() != "pending") {
        throw std::invalid_argument("Добавлять товары можно только в поставку со статусом pending");
    }

    if (!productRepo.getProductById(product_id)) {
        throw std::runtime_error("Товар с id " + std::to_string(product_id) + " не найден");
    }

    supplyRepo.addSupplyPart(supply_id, product_id, quantity);
}

std::string WarehouseService::zoneNameForType(const std::string& product_type) {
    if (product_type == "regular") return "Обычная";
    if (product_type == "perishable") return "Холодная";
    if (product_type == "oversized") return "Крупногабаритная";
    throw std::invalid_argument("Неизвестный тип товара: " + product_type);
}

std::string WarehouseService::escapeJson(const std::string& s) {
    std::string out;
    for (char c : s) {
        if (c == '"' || c == '\\') out.push_back('\\');
        out.push_back(c);
    }
    return out;
}