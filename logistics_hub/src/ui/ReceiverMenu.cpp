#include "ui/ReceiverMenu.h"

#include <iomanip>
#include <iostream>
#include <limits>
#include <stdexcept>

#include "models/Supply/Supply.h"
#include "models/Supply/SupplyPart.h"

int ReceiverMenu::run() {
    while (true) {
        std::cout << "\n=== Меню приёмщика ===\n";
        if (current_supply_id != -1) {
            std::cout << "[Активная поставка: #" << current_supply_id << "]\n";
        }
        std::cout << "1. Выбрать поставку для приёмки\n";
        std::cout << "2. Вывести список товаров в поставке\n";
        std::cout << "3. Приёмка товара\n";
        std::cout << "4. Завершение поставки\n";
        std::cout << "5. Выйти (сменить аккаунт)\n";
        std::cout << "6. Выйти из приложения\n";
        std::cout << "Выбор: ";

        int choice = 0;
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Некорректный ввод\n";
            continue;
        }

        try {
            switch (choice) {
                case 1:
                    selectSupply();
                    break;
                case 2:
                    showSupplyComposition();
                    break;
                case 3:
                    receiveProduct();
                    break;
                case 4:
                    finishSupply();
                    break;
                case 5:
                    if (current_supply_id != -1) {
                        std::cout << "Завершите активную поставку перед выходом\n";
                    } else {
                        return 1;
                    }
                    break;
                case 6:
                    if (current_supply_id != -1) {
                        std::cout << "Завершите активную поставку перед выходом\n";
                    } else {
                        return 2;
                    }
                    break;
                default:
                    std::cout << "Неизвестный пункт\n";
            }
        } catch (const std::exception& e) {
            std::cerr << "Ошибка: " << e.what() << "\n";
        }
    }
}

int ReceiverMenu::showPendingSupplies() {
    auto supplies = warehouseService.getPendingSupplies();

    if (supplies.empty()) {
        std::cout << "Нет поставок со статусом pending\n";
        return -1;
    }

    std::cout << "\n--- Поставки pending ---\n";
    std::cout << "ID  | Поставщик\n";
    std::cout << "----|----------\n";
    for (const auto& s : supplies) {
        std::cout << std::left << std::setw(4) << s.getId() << "| " << std::setw(9)
                  << s.getSupplierId() << "\n";
    }
    return 0;
}

void ReceiverMenu::selectSupply() {
    if (current_supply_id != -1) {
        std::cout << "Уже выбрана поставка #" << current_supply_id
                  << ". Завершите её перед сменой.\n";
        return;
    }

    if (showPendingSupplies() == -1) return;

    std::cout << "\nВведите ID поставки (0 для отмены): ";
    int id = 0;
    std::cin >> id;
    if (id == 0) return;

    auto parts = warehouseService.getSupplyParts(id);
    if (parts.empty() || warehouseService.getSupplyById(id)->getStatus() != "pending") {
        std::cout << "Поставка #" << id
                  << " не найдена, или не имеет состава, или не имеет статуса 'pending'\n";
        return;
    }

    current_supply_id = id;
    session.clear();
    for (const auto& part : parts) {
        session[part.getProductId()] = {part.getQuantity(), 0};
    }

    std::cout << "Поставка #" << id << " выбрана. Состав:\n";
    showSupplyComposition();
}

void ReceiverMenu::showSupplyComposition() {
    if (current_supply_id == -1) {
        std::cout << "Сначала выберите поставку\n";
        return;
    }

    std::cout << "\n--- Состав поставки #" << current_supply_id << " ---\n";
    std::cout << "Product ID | Ожидается | Размещено | Разница\n";
    std::cout << "-----------|-----------|-----------|--------\n";
    for (const auto& [product_id, ep] : session) {
        int diff = ep.placed - ep.expected;
        std::cout << std::left << std::setw(11) << product_id << "| " << std::setw(10)
                  << ep.expected << "| " << std::setw(10) << ep.placed << "| "
                  << (diff > 0 ? "+" : "") << std::setw(7) << diff << "\n";
    }
}

void ReceiverMenu::receiveProduct() {
    if (current_supply_id == -1) {
        std::cout << "Сначала выберите поставку\n";
        return;
    }

    std::cout << "\nВведите ID товара (0 для отмены): ";
    int product_id = 0;
    std::cin >> product_id;
    if (product_id == 0) return;

    auto it = session.find(product_id);
    if (it == session.end()) {
        std::cout << "Товар #" << product_id << " не входит в состав поставки #"
                  << current_supply_id << "\n";
        return;
    }

    std::cout << "Ожидается: " << it->second.expected << ", уже размещено: " << it->second.placed
              << "\n";
    std::cout << "Введите фактическое количество: ";
    int qty = 0;
    if (!(std::cin >> qty) || qty <= 0) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Количество должно быть положительным целым числом\n";
        return;
    }

    PlacementResult result =
        warehouseService.placeSupplyProduct(current_supply_id, product_id, qty);

    it->second.placed += result.placed;

    if (!result.isFull()) {
        std::cout << "ВНИМАНИЕ: " << result.remaining << " ед. не поместились на полках (буфер)\n";
    }
    if (it->second.placed != it->second.expected) {
        std::cout << "ВНИМАНИЕ: расхождение — ожидается " << it->second.expected << ", размещено "
                  << it->second.placed << "\n";
    }

    if (result.placements.size() == 1) {
        std::cout << "Размещено: " << result.placed << " ед. на полке:\n";
    } else {
        std::cout << "Размещено: " << result.placed << " ед. на полках:\n";
    }

    for (const auto& [shelf_id, placed_qty] : result.placements) {
        std::cout << "  Полка #" << shelf_id << " — " << placed_qty << " ед.\n";
    }
}

void ReceiverMenu::finishSupply() {
    if (current_supply_id == -1) {
        std::cout << "Сначала выберите поставку\n";
        return;
    }

    printSessionSummary();

    bool hasDiscrepancy = false;
    for (const auto& [product_id, ep] : session) {
        if (ep.placed != ep.expected) {
            hasDiscrepancy = true;
            break;
        }
    }

    std::string reason = "";

    if (hasDiscrepancy) {
        std::cout << "\nЕсть расхождения между ожидаемым и фактическим количеством.\n";
        std::cout << "Вы уверены, что хотите завершить поставку? (y/n): ";
        char confirm = 'n';
        std::cin >> confirm;
        if (confirm != 'y' && confirm != 'Y') {
            std::cout << "Завершение отменено\n";
            return;
        }

        std::cout << "Укажите причину расхождения: ";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::getline(std::cin, reason);
    }

    if (hasDiscrepancy) {
        warehouseService.acceptSupply(current_supply_id, user_id, buildDiscrepancyJson(reason));
    } else {
        warehouseService.acceptSupply(current_supply_id, user_id);
    }

    std::cout << "Поставка #" << current_supply_id << " принята (статус: accepted)\n";
    resetSession();
}

void ReceiverMenu::resetSession() {
    current_supply_id = -1;
    session.clear();
}

void ReceiverMenu::printSessionSummary() {
    if (current_supply_id == -1) {
        std::cout << "Нет активной поставки\n";
        return;
    }
    std::cout << "\n=== Сводка по поставке #" << current_supply_id << " ===\n";
    int total_expected = 0, total_placed = 0;
    for (const auto& [product_id, ep] : session) {
        total_expected += ep.expected;
        total_placed += ep.placed;
    }
    std::cout << "Всего ожидается: " << total_expected << " ед.\n";
    std::cout << "Всего размещено: " << total_placed << " ед.\n";
    if (total_placed == total_expected) {
        std::cout << "Статус: без расхождений\n";
    } else {
        std::cout << "Статус: ЕСТЬ РАСХОЖДЕНИЯ (" << (total_placed - total_expected) << " ед.)\n";
    }
}

std::string ReceiverMenu::buildDiscrepancyJson(const std::string& reason) {
    std::string json = R"({"discrepancies": [)";
    bool first = true;
    for (const auto& [product_id, ep] : session) {
        if (ep.placed != ep.expected) {
            if (!first) json += ",";
            json += R"({"product_id": )" + std::to_string(product_id) + R"(, "expected": )" +
                    std::to_string(ep.expected) + R"(, "placed": )" + std::to_string(ep.placed) +
                    "}";
            first = false;
        }
    }
    json += R"(], "reason": ")" + escapeJson(reason) + R"("})";
    return json;
}

std::string ReceiverMenu::escapeJson(const std::string& s) {
    std::string out;
    for (char c : s) {
        if (c == '"' || c == '\\') out.push_back('\\');
        out.push_back(c);
    }
    return out;
}