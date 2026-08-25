#include "services/WarehouseService.h"

#include <algorithm>
#include <cctype>

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

void WarehouseService::acceptSupply(int supply_id, int accepted_by, const std::string& details) {
    const auto supply = supplyRepo.getSupplyById(supply_id);

    if (!supply)
        throw std::runtime_error("Поставка с id " + std::to_string(supply_id) + " не найдена");

    if (supply->getStatus() != "pending")
        throw std::invalid_argument("Принять можно только поставку со статусом pending");

    audit.log("shipment", supply_id, "receive", details);
    supplyRepo.acceptSupply(supply_id, accepted_by);
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

std::optional<Supplier> WarehouseService::getSupplierById(int supplier_id) {
    auto supplier = supplierRepo.getById(supplier_id);

    if (!supplier)
        throw std::runtime_error("Поставщик с id = " + std::to_string(supplier_id) + " не найден");
    return supplier;
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

std::vector<ShelfProduct> WarehouseService::getShelfProductsByProductId(int product_id) {
    return productRepo.getShelfProductsByProductId(product_id);
}

int WarehouseService::createWarehouse(const std::string& name, const std::string& address,
                                      double square) {
    isValidName(name);
    isValidName(address);
    if (square <= 0) throw std::invalid_argument("Площадь должна быть больше 0)");

    for (const auto& w : warehouseRepo.getAllWarehouses()) {
        if ((w.getName() == name) || (w.getAddress() == address)) {
            throw std::runtime_error(
                "Склад с таким названием уже существует или по этому адресу уже находится склад");
        }
    }

    int id = warehouseRepo.createWarehouse(name, address, square);

    audit.log("cell", id, "update", "");

    return id;
}

void WarehouseService::updateWarehouse(int id, const std::string& name, const std::string& address,
                                       double square) {
    if (!warehouseRepo.getWarehouseById(id))
        throw std::runtime_error("Склад с id " + std::to_string(id) + " не найден");

    isValidName(name);
    isValidName(address);
    if (square <= 0) throw std::invalid_argument("Площадь должна быть больше 0)");

    for (const auto& w : warehouseRepo.getAllWarehouses()) {
        if ((w.getName() == name && w.getId() != id) ||
            (w.getAddress() == address && w.getId() != id)) {
            throw std::runtime_error(
                "Склад с таким названием уже существует или по этому адресу уже находится склад");
        }
    }

    warehouseRepo.updateWarehouse(id, name, address, square);

    audit.log("cell", id, "update", "");
}

std::vector<Warehouse> WarehouseService::getAllWarehouses() {
    return warehouseRepo.getAllWarehouses();
}

std::optional<Warehouse> WarehouseService::getWarehouseById(int id) {
    return warehouseRepo.getWarehouseById(id);
}

int WarehouseService::createZone(int warehouse_id, const std::string& name, const std::string& code,
                                 double max_weight, double max_volume) {
    if (!warehouseRepo.getWarehouseById(warehouse_id))
        throw std::runtime_error("Склад с id " + std::to_string(warehouse_id) + " не найден");

    isValidName(name);
    isValidCode(code);
    if (max_weight <= 0 || max_volume <= 0)
        throw std::invalid_argument("Численные параметры должны быть больше 0");

    for (const auto& z : warehouseRepo.getZonesByWarehouseId(warehouse_id)) {
        if ((z.getName() == name) || (z.getCode() == code)) {
            throw std::runtime_error(
                "Зона с таким названием уже существует или уже используется такой код");
        }
    }

    int id = warehouseRepo.createZone(warehouse_id, name, code, max_weight, max_volume);

    audit.log("cell", id, "update", "");

    return id;
}

void WarehouseService::updateZone(int id, const std::string& name, const std::string& code,
                                  double max_weight, double max_volume) {
    auto zone = warehouseRepo.getZoneById(id);
    if (!zone) throw std::runtime_error("Зона с id " + std::to_string(id) + " не найден");

    isValidName(name);
    isValidCode(code);
    if (max_weight <= 0 || max_volume <= 0)
        throw std::invalid_argument("Численные параметры должны быть больше 0");

    for (const auto& z : warehouseRepo.getZonesByWarehouseId(zone->getWarehouseId())) {
        if (z.getId() != id && ((z.getName() == name) || (z.getCode() == code))) {
            throw std::runtime_error(
                "Зона с таким названием уже существует или уже используется такой код");
        }
    }

    warehouseRepo.updateZone(id, name, code, max_weight, max_volume);

    audit.log("cell", id, "update", "");
}

std::vector<Zone> WarehouseService::getZonesByWarehouseId(int warehouse_id) {
    return warehouseRepo.getZonesByWarehouseId(warehouse_id);
}

std::optional<Zone> WarehouseService::getZoneByName(const std::string& name) {
    isValidName(name);
    return warehouseRepo.getZoneByName(name);
}

std::optional<Zone> WarehouseService::getZoneById(int zone_id) {
    return warehouseRepo.getZoneById(zone_id);
}

int WarehouseService::createRack(int zone_id, const std::string& code, double max_weight,
                                 double max_volume) {
    if (!warehouseRepo.getZoneById(zone_id))
        throw std::runtime_error("Зона с id " + std::to_string(zone_id) + " не найден");

    isValidCode(code);
    if (max_weight <= 0 || max_volume <= 0)
        throw std::invalid_argument("Численные параметры должны быть больше 0");

    for (const auto& r : warehouseRepo.getRacksByZoneId(zone_id)) {
        if (r.getCode() == code) {
            throw std::runtime_error("Такой код зоны уже используется в этом складе");
        }
    }

    int id = warehouseRepo.createRack(zone_id, code, max_weight, max_volume);

    audit.log("cell", id, "update", "");

    return id;
}
void WarehouseService::updateRack(int id, const std::string& code, double max_weight,
                                  double max_volume) {
    auto currentRack = warehouseRepo.getRackById(id);
    if (!currentRack) throw std::runtime_error("Стеллаж с id " + std::to_string(id) + " не найден");

    isValidCode(code);
    if (max_weight <= 0 || max_volume <= 0)
        throw std::invalid_argument("Численные параметры должны быть больше 0");

    int zone_id = currentRack->getZoneId();
    for (const auto& r : warehouseRepo.getRacksByZoneId(zone_id)) {
        if (r.getId() != id && r.getCode() == code) {
            throw std::runtime_error("Такой код стеллажа уже используется в этой зоне");
        }
    }

    warehouseRepo.updateRack(id, code, max_weight, max_volume);

    audit.log("cell", id, "update", "");
}

std::vector<Rack> WarehouseService::getRacksByZoneId(int zone_id) {
    return warehouseRepo.getRacksByZoneId(zone_id);
}

std::optional<Rack> WarehouseService::getRackById(int rack_id) {
    return warehouseRepo.getRackById(rack_id);
}

int WarehouseService::createShelf(int rack_id, const std::string& code, double max_weight,
                                  double max_volume) {
    if (!warehouseRepo.getRackById(rack_id))
        throw std::runtime_error("Стеллаж с id " + std::to_string(rack_id) + " не найден");

    isValidCode(code);
    if (max_weight <= 0 || max_volume <= 0)
        throw std::invalid_argument("Численные параметры должны быть больше 0");

    for (const auto& r : warehouseRepo.getShelvesByRackId(rack_id)) {
        if (r.getCode() == code) {
            throw std::runtime_error("Уже используется такой код");
        }
    }

    int id = warehouseRepo.createShelf(rack_id, code, max_weight, max_volume);

    audit.log("cell", id, "update", "");

    return id;
}
void WarehouseService::updateShelf(int id, const std::string& code, double max_weight,
                                   double max_volume) {
    auto currentShelf = warehouseRepo.getShelfById(id);
    if (!currentShelf) throw std::runtime_error("Полка с id " + std::to_string(id) + " не найдена");

    isValidCode(code);
    if (max_weight <= 0 || max_volume <= 0)
        throw std::invalid_argument("Численные параметры должны быть больше 0");

    int rack_id = currentShelf->getRackId();
    for (const auto& s : warehouseRepo.getShelvesByRackId(rack_id)) {
        if (s.getId() != id && s.getCode() == code) {
            throw std::runtime_error("Такой код полки уже используется на этом стеллаже");
        }
    }

    warehouseRepo.updateShelf(id, code, max_weight, max_volume);

    audit.log("cell", id, "update", "");
}

std::vector<Shelf> WarehouseService::getShelvesByRackId(int rack_id) {
    return warehouseRepo.getShelvesByRackId(rack_id);
}

std::optional<Shelf> WarehouseService::getShelfById(int shelf_id) {
    auto shelf = warehouseRepo.getShelfById(shelf_id);
    if (!shelf)
        throw std::runtime_error("Полка с id = " + std::to_string(shelf_id) + " не найдена");
    return shelf;
}

std::string WarehouseService::zoneNameForType(const std::string& product_type) {
    if (product_type == "regular") return "regular";
    if (product_type == "perishable") return "cold";
    if (product_type == "oversized") return "oversized";
    throw std::invalid_argument("Неизвестный тип товара: " + product_type);
}

void WarehouseService::isValidName(const std::string& name) {
    if (name.empty()) {
        throw std::invalid_argument("Название не может быть пустым");
    }
    if (std::all_of(name.begin(), name.end(), [](unsigned char c) { return std::isspace(c); })) {
        throw std::invalid_argument("Название не может состоять только из пробелов");
    }
}

void WarehouseService::isValidCode(const std::string& code) {
    if (code.empty()) throw std::invalid_argument("Код не может быть пустым");
    if (!std::all_of(code.begin(), code.end(),
                     [](unsigned char c) { return std::isalnum(c) || c == '-'; })) {
        throw std::invalid_argument("Код должен содержать только буквы, цифры и дефис");
    }
}

std::string WarehouseService::escapeJson(const std::string& s) {
    std::string out;
    for (char c : s) {
        if (c == '"' || c == '\\') out.push_back('\\');
        out.push_back(c);
    }
    return out;
}