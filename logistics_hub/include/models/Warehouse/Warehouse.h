#pragma once
#include <string>

class Warehouse {
   public:
    Warehouse();
    Warehouse(int id, const std::string& name, const std::string& address, double square);
    ~Warehouse() = default;

    int getId() const;
    const std::string& getName() const;
    const std::string& getAddress() const;
    double getSquare() const;

   private:
    int id;
    std::string name;
    std::string address;
    double square;
};