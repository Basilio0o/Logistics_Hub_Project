#include "ui/AssemblerMenu.h"

#include <iomanip>
#include <iostream>
#include <limits>
#include <stdexcept>

#include "models/Order/Order.h"
#include "models/Parcel/Parcel.h"
#include "models/Parcel/ParcelPart.h"

int AssemblerMenu::run() {
    while (true) {
        std::cout << "\n=== Меню сборщика ===\n";
        if (current_parcel_id != -1) {
            std::cout << "[Активная посылка: #" << current_parcel_id << "]\n";
        }
        std::cout << "1. Создать посылку из заказа\n";
        std::cout << "2. Выбрать посылку для сборки\n";
        std::cout << "3. Просмотреть состав посылки\n";
        std::cout << "4. Собрать посылку\n";
        std::cout << "5. Выйти в главное меню\n";
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
                    createParcelFromOrder();
                    break;
                case 2:
                    selectParcel();
                    break;
                case 3:
                    showParcelDetails();
                    break;
                case 4:
                    assembleSelectedParcel();
                    break;
                case 5:
                    if (current_parcel_id != -1) {
                        std::cout << "Сначала завершите сборку посылки #" << current_parcel_id
                                  << "\n";
                    } else
                        return 1;
                    break;
                case 6:
                    if (current_parcel_id != -1) {
                        std::cout << "Сначала завершите сборку посылки #" << current_parcel_id
                                  << "\n";
                    } else
                        return 2;
                    break;
                default:
                    std::cout << "Неизвестный пункт\n";
            }
        } catch (const std::exception& e) {
            std::cerr << "Ошибка: " << e.what() << "\n";
        }
    }
}

int AssemblerMenu::showProcessingOrders() {
    if (current_parcel_id != -1) {
        std::cout << "Уже выбрана посылка #" << current_parcel_id
                  << ". Сначала завершите её сборку.\n";
        return -1;
    }

    auto orders = orderService.getOrdersByStatus("processing");

    if (orders.empty()) {
        std::cout << "Нет активных заказов\n";
        return -1;
    }

    std::cout << "\n--- Заказы processing ---\n";
    std::cout << "ID  | Приоритет | Дата создания\n";
    std::cout << "----|-----------|--------------\n";
    for (const auto& o : orders) {
        std::cout << std::left << std::setw(4) << o.getId() << "| " << std::setw(10)
                  << o.getPriority() << "| " << std::setw(13) << o.getCreatedAt() << "\n";
    }

    return 0;
}

void AssemblerMenu::createParcelFromOrder() {
    if (current_parcel_id != -1) {
        std::cout << "Уже выбрана посылка #" << current_parcel_id
                  << ". Сначала завершите её сборку.\n";
        return;
    }

    if (showProcessingOrders() == -1) return;

    std::cout << "\nВведите ID заказа (0 для отмены): ";

    int id = 0;
    if (!(std::cin >> id) || id == 0) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return;
    }

    auto order = orderService.getOrderById(id);
    auto orderParts = orderService.getOrderParts(id);

    if (orderParts.empty() || order->getStatus() != "processing") {
        std::cout << "Заказ #" << id << " не найден, пуст или не имеет статуса 'processing'\n";
        return;
    }

    try {
        int new_parcel_id = parcelService.createParcelFromOrder(id);
        std::cout << "Посылка #" << new_parcel_id << " успешно создана из заказа #" << id << "\n";

    } catch (const std::exception& e) {
        std::cerr << "Ошибка при создании посылки: " << e.what() << "\n";
    }
}

void AssemblerMenu::showNewParcels() {
    auto parcels = parcelService.getParcelsByStatus("new");

    if (parcels.empty()) {
        std::cout << "Нет новых посылок для сборки.\n";
        return;
    }

    std::cout << "\n--- Новые посылки ---\n";
    std::cout << "ID  | Заказ | Приоритет | Вес (кг) | Объём (м³)\n";
    std::cout << "----|-------|-----------|----------|-----------\n";
    for (const auto& p : parcels) {
        std::cout << std::left << std::setw(4) << p.getId() << "| " << std::setw(6)
                  << p.getOrderId() << "| " << std::setw(10) << p.getPriority() << "| "
                  << std::setw(9) << p.getWeight() << "| " << p.getVolume() << "\n";
    }
}

void AssemblerMenu::selectParcel() {
    if (current_parcel_id != -1) {
        std::cout << "Уже выбрана посылка #" << current_parcel_id
                  << ". Сначала завершите её сборку.\n";
        return;
    }

    showNewParcels();

    std::cout << "\nВведите ID посылки для сборки (0 для отмены): ";

    int id = 0;
    if (!(std::cin >> id) || id == 0) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return;
    }

    try {
        auto parcel = parcelService.getParcelById(id);
        if (!parcel || parcel->getStatus() != "new") {
            std::cout << "Посылка #" << id << " не найдена или уже не в статусе 'new'.\n";
            return;
        }

        current_parcel_id = id;
        std::cout << "Посылка #" << current_parcel_id << " выбрана для сборки.\n";

        showParcelDetails();

    } catch (const std::exception& e) {
        std::cerr << "Ошибка при выборе посылки: " << e.what() << "\n";
    }
}

void AssemblerMenu::showParcelDetails() {
    if (current_parcel_id == -1) {
        std::cout << "Сначала выберите посылку для сборки.\n";
        return;
    }

    auto parts = parcelService.getParcelParts(current_parcel_id);
    if (parts.empty()) {
        std::cout << "Состав посылки пуст.\n";
        return;
    }

    std::cout << "\n--- Состав посылки #" << current_parcel_id << " ---\n";
    std::cout << "Товар ID | Требуется | Доступно на складе (по полкам)\n";
    std::cout << "---------|-----------|--------------------------------\n";
    for (const auto& part : parts) {
        std::cout << std::left << std::setw(9) << part.getProductId() << "| " << std::setw(10)
                  << part.getQuantity() << "| ";
        try {
            auto shelfProducts = warehouseService.getShelfProductsByProductId(part.getProductId());
            if (shelfProducts.empty()) {
                std::cout << "Нет на полках\n";
            } else {
                int totalAvailable = 0;
                for (const auto& sp : shelfProducts) {
                    totalAvailable += sp.getQuantity();
                }
                std::cout << "Всего: " << totalAvailable << " шт. (полки: ";
                for (size_t i = 0; i < shelfProducts.size(); ++i) {
                    std::cout
                        << warehouseService.getShelfById(shelfProducts[i].getShelfId())->getCode()
                        << ":" << shelfProducts[i].getQuantity();
                    if (i + 1 < shelfProducts.size()) std::cout << ", ";
                }
                std::cout << ")\n";
            }
        } catch (const std::exception& e) {
            std::cout << "Ошибка чтения полок: " << e.what() << "\n";
        }
    }
}

void AssemblerMenu::assembleSelectedParcel() {
    if (current_parcel_id == -1) {
        std::cout << "Сначала выберите посылку для сборки.\n";
        return;
    }

    std::cout << "\nНачать сборку посылки #" << current_parcel_id << "? (y/n): ";
    char confirm = 'n';
    std::cin >> confirm;
    if (confirm != 'y' && confirm != 'Y') {
        std::cout << "Сборка отменена.\n";
        return;
    }

    try {
        Parcel parcel = parcelService.assembleParcel(current_parcel_id, user_id);
        std::cout << "Посылка #" << current_parcel_id << " успешно собрана!\n";
        std::cout << "Вес посылки: " << parcel.getWeight()
                  << ", объём посылки: " << parcel.getVolume() << std::endl;
        current_parcel_id = -1;
    } catch (const std::exception& e) {
        std::cerr << "Ошибка сборки: " << e.what() << "\n";
        std::cout
            << "Внимание: заказ, вероятно, переведён в 'canceled', посылка осталась в 'new'.\n";
        current_parcel_id = -1;
    }
}