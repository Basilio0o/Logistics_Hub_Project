#pragma once
#include <string>

class Supplier {
   public:
    Supplier();
    Supplier(int id, const std::string& name, const std::string& phone, const std::string& address);
    ~Supplier() = default;

    int getId() const;
    const std::string& getName() const;
    const std::string& getPhone() const;
    const std::string& getAddress() const;

   private:
    int id;
    std::string name;
    std::string phone;
    std::string address;
};