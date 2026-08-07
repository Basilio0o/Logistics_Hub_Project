#include <iostream>

#include "dal/Db.h"
#include "services/AuditService.h"
#include "services/UserService.h"

int main() {
    try {
        Db db("dbname=logistics_hub");
        std::cout << "Success" << std::endl;

        UserService userService(db);
        AuditService audit(db);

        try {
            std::cout << "created id=" << userService.createUser("tester", "1234", "receiver")
                      << std::endl;
        } catch (const std::exception& e) {
            std::cout << "createUser: " << e.what() << "\n";
        }

        auto user = userService.login("tester", "1234");
        if (!user) {
            std::cout << "login failed" << std::endl;
            return 1;
        }

        bool ok = audit.log("item", 1, "updae", R"({"note":"smoke test"})");
        std::cout << "audit.log -> " << std::boolalpha << ok << std::endl;

        for (const auto& l :
             audit.getLogs(user->getId(), std::nullopt, std::nullopt, std::nullopt)) {
            std::cout << l.getCreatedAt() << " | " << l.getEntityType() << "/" << l.getEntityId()
                      << " | " << l.getOperation() << " | by="
                      << (l.getPerformedBy() ? std::to_string(*l.getPerformedBy()) : "NULL")
                      << " | " << l.getDetails() << std::endl;
        }

    } catch (const std::exception& e) {
        std::cerr << "Error:" << e.what() << std::endl;
    }
    return 0;
}