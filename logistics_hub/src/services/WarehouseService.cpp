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

PlacementResult WarehouseService::placeSupplyProduct(int supply_id, int product_id, int quantity) {
    const auto supply = supplyRepo.getSupplyById(supply_id);

    if (!supply) {
        throw std::runtime_error("Поставка с id " + std::to_string(supply_id) + " не найдена");
    }
    if (supply->getStatus() != "pending") {
        throw std::invalid_argument("Принимать можно только поставку в статусе pending");
    }

    const auto parts = supplyRepo.getSupplyParts(supply_id);

    const bool inSupply =
        std::any_of(parts.begin(), parts.end(),
                    [product_id](const SupplyPart& p) { return p.getProductId() == product_id; });
    if (!inSupply) {
        throw std::invalid_argument("Товар с id " + std::to_string(product_id) +
                                    " не входит в данную поставку");
    }

    return placeProduct(product_id, quantity);
}

void WarehouseService::acceptSupply(int supply_id, int accepted_by) {
    const auto supply = supplyRepo.getSupplyById(supply_id);

    if (!supply)
        throw std::runtime_error("Поставка с id " + std::to_string(supply_id) + " не найдена");

    if (supply->getStatus() != "pending")
        throw std::invalid_argument("Принять можно только поставку со статусом pending");

    supplyRepo.acceptSupply(supply_id, accepted_by);

    audit.log("shipment", supply_id, "receive", "");
}

std::vector<Supply> WarehouseService::getPendingSupplies() {
    return supplyRepo.getPendingSupplies();
}

std::vector<SupplyPart> WarehouseService::getSupplyParts(int supply_id) {
    return supplyRepo.getSupplyParts(supply_id);
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
        throw std::runtime_error("Поставка с id " + std::to_string(supply_id) + " не найдена");
    }
    if (supply->getStatus() != "pending") {
        throw std::invalid_argument("Добавлять товары можно только в поставку со статусом pending");
    }

    if (!productRepo.getProductById(product_id)) {
        throw std::runtime_error("Товар с id " + std::to_string(product_id) + " не найден");
    }

    supplyRepo.addSupplyPart(supply_id, product_id, quantity);
}

void WarehouseService::rejectSupply(int supply_id, const std::string& reason) {
    const auto supply = supplyRepo.getSupplyById(supply_id);

    if (!supply)
        throw std::runtime_error("Поставка с id " + std::to_string(supply_id) + " не найдена");

    if (supply->getStatus() != "pending")
        throw std::invalid_argument("Отклонить можно только поставку со статусом pending");

    supplyRepo.rejectSupply(supply_id);

    const std::string details = R"({"reason": ")" + escapeJson(reason) + R"("})";
    audit.log("shipment", supply_id, "update", details);
}

std::optional<Supply> WarehouseService::getSupplyById(int supply_id) {
    return supplyRepo.getSupplyById(supply_id);
}

int WarehouseService::createSupplier(const std::string& name,
                                     const std::optional<std::string>& phone,
                                     const std::optional<std::string>& address) {
    if (name.empty()) throw std::invalid_argument("Название поставщика не может быть пустым");

    int id = supplierRepo.createSupplier(name, phone, address);

    audit.log("supplier", id, "update", "");
    return id;
}

void WarehouseService::updateSupplier(int id, const std::string& name,
                                      const std::optional<std::string>& phone,
                                      const std::optional<std::string>& address) {
    if (!supplierRepo.getById(id))
        throw std::runtime_error("Поставщик с id " + std::to_string(id) + " не найден");

    if (name.empty()) throw std::invalid_argument("Название поставщика не может быть пустым");

    supplierRepo.updateSupplier(id, name, phone, address);

    audit.log("supplier", id, "update", "");
}

std::vector<Supplier> WarehouseService::getAllSuppliers() {
    return supplierRepo.getAll();
}

int WarehouseService::createProduct(const std::string& name, int supplier_id,
                                    const std::string& type, double unit_weight, double unit_volume,
                                    double unit_price, std::optional<int> shelf_life_days) {
    if (name.empty()) throw std::invalid_argument("Название товара не может быть пустым");

    if (!supplierRepo.getById(supplier_id))
        throw std::runtime_error("Поставщик с id " + std::to_string(supplier_id) + " не найден");

    zoneNameForType(type);

    if (unit_weight <= 0 || unit_volume <= 0 || unit_price <= 0)
        throw std::invalid_argument("Вес, объём и цена должны быть > 0");

    if (shelf_life_days.has_value() && *shelf_life_days <= 0)
        throw std::invalid_argument("Срок годности должен быть > 0");

    int id = productRepo.createProduct(name, supplier_id, type, unit_weight, unit_volume,
                                       unit_price, shelf_life_days);

    audit.log("item", id, "update", "");

    return id;
}

void WarehouseService::updateProduct(int id, const std::string& name, int supplier_id,
                                     const std::string& type, double unit_weight,
                                     double unit_volume, double unit_price,
                                     std::optional<int> shelf_life_days) {
    if (!productRepo.getProductById(id))
        throw std::runtime_error("Товар с id " + std::to_string(id) + " не найден");

    if (name.empty()) throw std::invalid_argument("Название товара не может быть пустым");

    if (!supplierRepo.getById(supplier_id))
        throw std::runtime_error("Поставщик с id " + std::to_string(supplier_id) + " не найден");

    zoneNameForType(type);

    if (unit_weight <= 0 || unit_volume <= 0 || unit_price <= 0)
        throw std::invalid_argument("Вес, объём и цена должны быть > 0");

    if (shelf_life_days.has_value() && *shelf_life_days <= 0)
        throw std::invalid_argument("Срок годности должен быть > 0");

    productRepo.updateProduct(id, name, supplier_id, type, unit_weight, unit_volume, unit_price,
                              shelf_life_days);

    audit.log("item", id, "update", "");
}

std::vector<Product> WarehouseService::getAllProducts() {
    return productRepo.getAllProducts();
}

std::optional<Product> WarehouseService::getProductById(int id) {
    return productRepo.getProductById(id);
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