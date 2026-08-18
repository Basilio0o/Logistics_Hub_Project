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
                  << o.getPriority() << std::setw(13) << o.getCreatedAt() << "\n";
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
    std::cin >> id;
    if (id == 0) return;

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
void AssemblerMenu::showNewParcels() {}
void AssemblerMenu::selectParcel() {}
void AssemblerMenu::showParcelDetails() {}
void AssemblerMenu::assembleSelectedParcel() {}