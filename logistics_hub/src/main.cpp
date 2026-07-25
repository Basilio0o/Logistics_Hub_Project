#include <iostream>

#include "dal/Db.h"

int main() {
    try {
        Db db("dbname=logistics_hub");
        std::cout << "Success" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error:" << e.what() << std::endl;
    }
    return 0;
}