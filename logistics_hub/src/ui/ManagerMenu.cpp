#include "ui/ManagerMenu.h"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

#include "models/Vehicle.h"

int ManagerMenu::run() {
    while (true) {
        std::cout << "\n=== Меню менеджера ===\n";
        std::cout << "1.  Сводка (Дашборд)\n";
        std::cout << "2.  Создать заказ\n";
        std::cout << "3.  Создать N заказов\n";
        std::cout << "4.  Заказать поставку\n";
        std::cout << "5.  Заказать N поставок\n";
        std::cout << "6.  Начать собирать заказ\n";
        std::cout << "7.  Посмотреть список отменённых заказов\n";
        std::cout << "8.  Управление поставками (pending)\n";
        std::cout << "9.  Управление погрузкой\n";
        std::cout << "10. Управление машинами\n";
        std::cout << "11. Выйти в главное меню\n";
        std::cout << "12. Выйти из приложения\n";
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
                    showDashboard();
                    break;
                case 2:
                    generateRandomOrder();
                    break;
                case 3:
                    generateBatchOrders();
                    break;
                case 4:
                    generateRandomSupply();
                    break;
                case 5:
                    generateBatchSupplies();
                    break;
                case 6:
                    startProcessingOrder();
                    break;
                case 7:
                    showCanceledOrders();
                    break;
                case 8:
                    managePendingSupplies();
                    break;
                case 9:
                    manageParcels();
                    break;
                case 10:
                    manageVehicle();
                    break;
                case 11:
                    return 1;
                case 12:
                    return 2;
                default:
                    std::cout << "Неизвестный пункт\n";
            }
        } catch (const std::exception& e) {
            std::cerr << "Ошибка: " << e.what() << "\n";
        }
    }
}

void ManagerMenu::showDashboard() {
    std::cout << "\n=== Сводка по логистическому хабу ===\n";

    std::cout << "\n[Автопарк]\n";
    auto availVehicles = vehicleService.getVehiclesByStatus("available");
    auto routeVehicles = vehicleService.getVehiclesByStatus("on_route");
    auto loadingVehicles = vehicleService.getVehiclesByStatus("loading");
    std::cout << "Доступно: " << availVehicles.size() << " | В загрузке: " << loadingVehicles.size()
              << " | В рейсе: " << routeVehicles.size() << "\n";

    std::cout << "\n[Заказы и посылки]\n";
    auto newOrders = orderService.getOrdersByStatus("new");
    auto procOrders = orderService.getOrdersByStatus("processing");
    auto newParcels = parcelService.getParcelsByStatus("new");
    auto assembledParcels = parcelService.getParcelsByStatus("assembled");

    std::cout << "Заказы: new=" << newOrders.size() << ", processing=" << procOrders.size() << "\n";
    std::cout << "Посылки: new=" << newParcels.size() << ", assembled=" << assembledParcels.size()
              << "\n";

    std::cout << "\n[Складские зоны]\n";
    std::cout << "ID | Название  | Код | Текущий вес / Максимальный вес | Текущий объём / "
                 "Максимальный объём\n";
    std::cout << "---|-----------|-----|--------------------------------|-----------"
                 "----------------------- \n";

    auto warehouses = warehouseService.getAllWarehouses();
    auto zones = warehouseService.getZonesByWarehouseId(warehouses[0].getId());

    for (const auto& zone : zones) {
        std::ostringstream weightStream, volumeStream;
        weightStream << zone.getCurrentWeight() << " / " << zone.getMaxWeight();
        volumeStream << zone.getCurrentVolume() << " / " << zone.getMaxVolume();

        std::cout << std::left << std::setw(3) << zone.getId() << "| " << std::setw(10)
                  << zone.getName() << "| " << std::setw(4) << zone.getCode() << "| "
                  << std::setw(31) << weightStream.str() << "| " << std::setw(34)
                  << volumeStream.str() << "\n";
    }
}

void ManagerMenu::generateRandomOrder() {
    auto districts = districtService.getAllDistricts();
    auto products = warehouseService.getAllProducts();

    if (districts.empty() || products.empty()) {
        std::cout << "Недостаточно справочных данных для генерации (нет районов или товаров).\n";
        return;
    }

    try {
        int randDistrictIdx = std::rand() % districts.size();
        int districtId = districts[randDistrictIdx].getId();

        std::vector<std::string> priorities = {"low", "normal", "high", "urgent"};
        std::string randPriority = priorities[std::rand() % priorities.size()];

        int newOrderId = orderService.createOrder(districtId, randPriority);

        int numItems = 1 + (std::rand() % 3);
        for (int i = 0; i < numItems; ++i) {
            int randProductIdx = std::rand() % products.size();
            int productId = products[randProductIdx].getId();
            int quantity = 1 + (std::rand() % 5);

            orderService.addOrderPart(newOrderId, productId, quantity);
        }

        std::cout << "Успешно создан заказ #" << newOrderId
                  << " (район: " << districts[randDistrictIdx].getName()
                  << ", позиций: " << numItems << ").\n";
    } catch (const std::exception& e) {
        std::cerr << "Ошибка генерации заказа: " << e.what() << "\n";
    }
}

void ManagerMenu::generateRandomSupply() {
    auto suppliers = warehouseService.getAllSuppliers();
    auto products = warehouseService.getAllProducts();

    if (suppliers.empty() || products.empty()) {
        std::cout << "Недостаточно справочных данных (нет поставщиков или товаров).\n";
        return;
    }

    try {
        int randSupplierIdx = std::rand() % suppliers.size();
        int newSupplyId = warehouseService.createSupply(suppliers[randSupplierIdx].getId());
        int numItems = 1 + (std::rand() % 3);

        for (int i = 0; i < numItems; ++i) {
            int randProductIdx = std::rand() % products.size();
            int quantity = 10 + (std::rand() % 41);
            warehouseService.addSupplyPart(newSupplyId, products[randProductIdx].getId(), quantity);
        }

        std::cout << "Успешно создана поставка #" << newSupplyId
                  << " (поставщик: " << suppliers[randSupplierIdx].getName()
                  << ", позиций: " << numItems << ").\n";
    } catch (const std::exception& e) {
        std::cerr << "Ошибка генерации поставки: " << e.what() << "\n";
    }
}

void ManagerMenu::generateBatchOrders() {
    std::cout << "\nВведите количество заказов для генерации (N > 0): ";
    int n = 0;

    if (!(std::cin >> n) || n <= 0) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Некорректное число. Генерация отменена.\n";
        return;
    }

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    std::cout << "Начинаю генерацию " << n << " заказов...\n";
    int successCount = 0;

    for (int i = 0; i < n; ++i) {
        try {
            generateRandomOrder();

            successCount++;
        } catch (const std::exception& e) {
            std::cerr << "Ошибка при генерации заказа #" << (i + 1) << ": " << e.what() << "\n";
        }
    }

    std::cout << "Готово! Успешно создано " << successCount << " из " << n << " заказов.\n";
}

void ManagerMenu::generateBatchSupplies() {
    std::cout << "\nВведите количество поставок для генерации (N > 0): ";
    int n = 0;

    if (!(std::cin >> n) || n <= 0) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Некорректное число. Генерация отменена.\n";
        return;
    }

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    std::cout << "Начинаю генерацию " << n << " поставок...\n";
    int successCount = 0;

    for (int i = 0; i < n; ++i) {
        try {
            generateRandomSupply();

            successCount++;
        } catch (const std::exception& e) {
            std::cerr << "Ошибка при генерации поставки #" << (i + 1) << ": " << e.what() << "\n";
        }
    }

    std::cout << "Готово! Успешно создано " << successCount << " из " << n << " поставок.\n";
}

void ManagerMenu::startProcessingOrder() {
    auto newOrders = orderService.getOrdersByStatus("new");

    if (newOrders.empty()) {
        std::cout << "Нет заказов со статусом 'new' для перевода в обработку.\n";
        return;
    }

    std::cout << "\n--- Заказы new ---\n";
    std::cout << "ID  | Район | Приоритет | Дата создания\n";
    std::cout << "----|-------|-----------|--------------\n";
    for (const auto& order : newOrders) {
        std::cout << std::left << std::setw(4) << order.getId() << "| " << std::setw(6)
                  << order.getDistrictId() << "| " << std::setw(10) << order.getPriority() << "| "
                  << order.getCreatedAt() << "\n";
    }

    std::cout << "\nВведите ID заказа для перевода в processing (0 для отмены): ";
    int order_id = 0;
    if (!(std::cin >> order_id) || order_id == 0) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return;
    }

    bool found = false;
    for (const auto& order : newOrders) {
        if (order.getId() == order_id) {
            found = true;
            break;
        }
    }

    if (!found) {
        std::cout << "Заказ #" << order_id << " не найден или уже не в статусе 'new'.\n";
        return;
    }

    try {
        orderService.startProcessing(order_id);
        std::cout << "Заказ #" << order_id << " переведён в статус 'processing'.\n";
    } catch (const std::exception& e) {
        std::cerr << " Ошибка: " << e.what() << "\n";
    }
}

void ManagerMenu::showCanceledOrders() {
    auto canceledOrders = orderService.getOrdersByStatus("canceled");

    if (canceledOrders.empty()) {
        std::cout << "Нет отменённых заказов.\n";
        return;
    }

    std::cout << "\n--- Отменённые заказы ---\n";
    std::cout << "ID  | Район | Приоритет | Дата создания\n";
    std::cout << "----|-------|-----------|--------------\n";

    for (const auto& order : canceledOrders) {
        std::cout << std::left << std::setw(4) << order.getId() << "| " << std::setw(6)
                  << order.getDistrictId() << "| " << std::setw(10) << order.getPriority() << "| "
                  << order.getCreatedAt() << "\n";
    }

    std::cout << "\nВведите ID заказа для просмотра причины (0 для отмены): ";
    int order_id = 0;
    if (!(std::cin >> order_id) || order_id == 0) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return;
    }

    bool found = false;
    for (const auto& order : canceledOrders) {
        if (order.getId() == order_id) {
            found = true;
            break;
        }
    }

    if (!found) {
        std::cout << "Заказ #" << order_id << " не найден или не отменён.\n";
        return;
    }

    try {
        auto logs = audit.getLogs(std::nullopt, "update", std::nullopt, std::nullopt);

        bool reasonFound = false;
        for (const auto& log : logs) {
            if (log.getEntityType() == "order" && log.getEntityId() == order_id) {
                std::cout << "\nПричина отмены заказа #" << order_id << ":\n";
                if (log.getDetails() == "") {
                    std::cout << "Не указана\n";
                } else {
                    std::cout << log.getDetails() << "\n";
                }
                reasonFound = true;
                break;
            }
        }

        if (!reasonFound) {
            std::cout << "Причина отмены не найдена в журнале аудита.\n";
        }

    } catch (const std::exception& e) {
        std::cerr << "Ошибка при получении данных аудита: " << e.what() << "\n";
    }
}

void ManagerMenu::managePendingSupplies() {
    auto supplies = warehouseService.getPendingSupplies();

    if (supplies.empty()) {
        std::cout << "Нет поставок со статусом pending.\n";
        return;
    }

    std::cout << "\n--- Поставки pending ---\n";
    std::cout << "ID  | Поставщик\n";
    std::cout << "----|----------\n";
    for (const auto& s : supplies) {
        std::cout << std::left << std::setw(4) << s.getId() << "| "
                  << warehouseService.getSupplierById(s.getSupplierId())->getName() << "\n";
    }

    std::cout << "\nВведите ID поставки для отклонения (0 для отмены): ";
    int supply_id = 0;
    if (!(std::cin >> supply_id) || supply_id == 0) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return;
    }

    bool found = false;
    for (const auto& s : supplies) {
        if (s.getId() == supply_id) {
            found = true;
            break;
        }
    }

    if (!found) {
        std::cout << "Поставка #" << supply_id << " не найдена или уже не в статусе 'pending'.\n";
        return;
    }

    std::cout << "Укажите причину отклонения: ";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::string reason;
    std::getline(std::cin, reason);

    if (reason.empty()) {
        std::cout << "Причина не может быть пустой. Отклонение отменено.\n";
        return;
    }

    if (std::all_of(reason.begin(), reason.end(),
                    [](unsigned char c) { return std::isspace(c); })) {
        throw std::invalid_argument("Причина не может состоять из одних пробелов");
    }

    try {
        warehouseService.rejectSupply(supply_id, reason);
        std::cout << "Поставка #" << supply_id << " успешно отклонена.\n";
    } catch (const std::exception& e) {
        std::cerr << "Ошибка при отклонении поставки: " << e.what() << "\n";
    }
}

void ManagerMenu::manageParcels() {
    auto parcels = parcelService.getParcelsByStatus("assembled");
    auto vehicles = vehicleService.getVehiclesByStatus("available");

    if (parcels.empty()) {
        std::cout << "Нет собранных посылок (assembled), готовых к отгрузке.\n";
        return;
    }
    if (vehicles.empty()) {
        std::cout << "Нет доступных машин (available) для загрузки.\n";
        return;
    }

    std::cout << "\n--- Доступные машины ---\n";
    std::cout << "ID  | Номер      | Водитель        | Своб. вес / объём\n";
    std::cout << "----|------------|-----------------|------------------\n";
    for (const auto& v : vehicles) {
        double freeWeight = v.getMaxWeight() - v.getCurrentWeight();
        double freeVolume = v.getMaxVolume() - v.getCurrentVolume();
        std::cout << std::left << std::setw(4) << v.getId() << "| " << std::setw(11)
                  << v.getNumber() << "| " << std::setw(16) << v.getDriver() << "| " << freeWeight
                  << " кг / " << freeVolume << " м³\n";

        auto districts = vehicleService.getVehicleDistricts(v.getId());
        std::cout << "   | Районы: ";
        for (size_t i = 0; i < districts.size(); ++i) {
            std::cout << districts[i].getName() << (i + 1 < districts.size() ? ", " : "");
        }
        std::cout << "\n";
    }

    std::cout << "\nВведите ID машины для загрузки (0 для отмены): ";
    int vehicle_id = 0;
    if (!(std::cin >> vehicle_id) || vehicle_id == 0) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return;
    }

    bool found = false;
    for (const auto& v : vehicles) {
        if (v.getId() == vehicle_id) {
            found = true;
            break;
        }
    }

    if (!found) {
        std::cout << "Машина #" << vehicle_id << " не найдена или уже не в статусе 'avaible'.\n";
        return;
    }

    try {
        std::cout << "Выполняется загрузка машины...\n";
        auto loadedParcels = vehicleService.loadVehicle(vehicle_id, user_id);

        if (loadedParcels.empty()) {
            std::cout << "Нет посылок с подходящим районом для данной машины.\n";
            return;
        }

        std::cout << "Успешно загружено посылок: " << loadedParcels.size() << "\n";
    } catch (const std::exception& e) {
        std::cerr << "Ошибка при управлении отгрузкой: " << e.what() << "\n";
    }
}

void ManagerMenu::manageVehicle() {
    auto vehiclesLoaded = vehicleService.getVehiclesByStatus("loading");
    auto vehiclesDispatched = vehicleService.getVehiclesByStatus("on_route");

    std::vector<Vehicle> vehicles;
    vehicles.reserve(vehiclesLoaded.size() + vehiclesDispatched.size());
    vehicles.insert(vehicles.end(), vehiclesLoaded.begin(), vehiclesLoaded.end());
    vehicles.insert(vehicles.end(), vehiclesDispatched.begin(), vehiclesDispatched.end());

    if (vehicles.empty()) {
        std::cout << "Нет загруженных машин и нет отправленных машин.\n";
        return;
    }

    std::cout << "\n--- Доступные машины ---\n";
    std::cout << "ID  | Номер      | Водитель        | Статус\n";
    std::cout << "----|------------|-----------------|-------\n";
    for (const auto& v : vehicles) {
        std::cout << std::left << std::setw(4) << v.getId() << "| " << std::setw(11)
                  << v.getNumber() << "| " << std::setw(16) << v.getDriver() << "| " << std::setw(6)
                  << v.getStatus() << std::endl;

        auto districts = vehicleService.getVehicleDistricts(v.getId());
        std::cout << "    | Районы: ";
        for (size_t i = 0; i < districts.size(); ++i) {
            std::cout << districts[i].getName() << (i + 1 < districts.size() ? ", " : "");
        }
        std::cout << "\n";
    }

    std::cout << "\nВведите ID машины для отправки или для возврата (0 для отмены): ";

    int vehicle_id = 0;
    if (!(std::cin >> vehicle_id) || vehicle_id == 0) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return;
    }

    bool found = false;
    for (const auto& v : vehicles) {
        if (v.getId() == vehicle_id) {
            found = true;
            break;
        }
    }

    if (!found) {
        std::cout << "Машина #" << vehicle_id
                  << " не найдена или уже не в статусе 'loading' или 'on_route'.\n";
        return;
    }
    try {
        if (vehicleService.getVehicleById(vehicle_id)->getStatus() == "loading") {
            std::cout << "Отправить машину в рейс? (y/n): ";
            char confirm = 'n';
            std::cin >> confirm;
            if (confirm == 'y' || confirm == 'Y') {
                vehicleService.dispatchVehicle(vehicle_id, user_id);
                std::cout << "Машина успешно отправлена в рейс (статус: on_route)!\n";
            } else {
                std::cout << "Машина осталась в статусе 'loading'. Вы можете отправить её позже.\n";
            }
        } else {
            std::cout << "Завершить рейс? (y/n): ";
            char confirm = 'n';
            std::cin >> confirm;
            if (confirm == 'y' || confirm == 'Y') {
                vehicleService.completeRoute(vehicle_id);
                std::cout << "Машина успешно вернулась в гараж (статус: avaible)!\n";
            } else {
                std::cout
                    << "Машина осталась в статусе 'on_route'. Вы можете отправить её позже.\n";
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Ошибка при изменении статуса машины: " << e.what() << "\n";
    }
}