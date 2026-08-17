#include <iostream>
#include <limits>
#include <string>

#include "dal/Db.h"
#include "services/UserService.h"
#include "ui/ReceiverMenu.h"

int main() {
    try {
        Db db("dbname=logistics_hub");
        UserService userService(db);

        while (true) {
            std::string login, password;
            std::cout << "\n=== Вход в систему ===\n";
            std::cout << "Логин: ";
            std::cin >> login;
            std::cout << "Пароль: ";
            std::cin >> password;

            int choice = -1;

            auto user = userService.login(login, password);
            if (!user) {
                std::cout << "Неверный логин или пароль\n";
                std::cout
                    << "Нажмите enter, если хотите повторить попытку. Введите 0, чтобы выйти\n";
                std::cout << "Ваш ввод: ";

                std::cin >> choice;

                if (choice == 0) {
                    return 0;
                }
                continue;

                if (!(std::cin >> choice)) {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    std::cout << "Некорректный ввод\n";
                    continue;
                }
            }

            int exitCode = 0;
            std::string role = user->getRole();

            if (role == "receiver") {
                std::cout << "\nДобро пожаловать!\n";
                ReceiverMenu menu(db, user->getId());
                exitCode = menu.run();
            } else {
                std::cout << "Меню для роли '" << role << "' в разработке\n";
                exitCode = 1;
            }

            if (exitCode == 2) break;
        }

        std::cout << "До свидания!\n";
    } catch (const std::exception& e) {
        std::cerr << "Критическая ошибка: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}