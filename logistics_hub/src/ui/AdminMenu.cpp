#include "ui/AdminMenu.h"

#include <iomanip>
#include <iostream>
#include <limits>

int AdminMenu::run() {
    while (true) {
        std::cout << "\n=== Меню администратора ===\n";
        std::cout << "1. Управление складом (просмотр)\n";
        std::cout << "2. Управление поставщиками\n";
        std::cout << "3. Управление товарами\n";
        std::cout << "4. Выйти в главное меню\n";
        std::cout << "5. Выйти из приложения\n";
        std::cout << "Выбор: ";

        int choice = 0;
        if (!std::cin >> choice) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Некорректный ввод. Попробуйте снова.\n";
            continue;
        }

        try {
            switch (choice) {
                case 1:
                    showWarehouses();
                    break;
                case 4:
                    return 1;
                case 5:
                    return 2;
                default:
                    std::cout << "Неизвестный пункт меню.\n";
            }
        } catch (const std::exception& e) {
            std::cerr << "Ошибка: " << e.what() << "\n";
        }
    }
}

void AdminMenu::showWarehouses() {
    std::cout << "\n=== Список складов ===\n";
    auto warehouses = warehouseService.getAllWarehouses();

    if (warehouses.empty()) {
        std::cout << "В системе пока нет зарегистрированных складов.\n";
        return;
    }

    std::cout << "ID | Название       | Адрес               | Площадь (м²)\n";
    std::cout << "---|----------------|---------------------|-------------\n";

    for (const auto& wh : warehouses) {
        std::cout << std::left << std::setw(3) << wh.getId() << std::setw(15) << wh.getName()
                  << std::setw(20) << wh.getAddress() << std::setw(12) << wh.getSquare() << "\n";
    }
}