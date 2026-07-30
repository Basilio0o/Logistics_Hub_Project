#pragma once
#include <optional>
#include <string>

class Supplier {
   public:
    Supplier();
    Supplier(int id, const std::string& name, std::optional<std::string> phone,
             std::optional<std::string> address);
    ~Supplier() = default;

    int getId() const;
    const std::string& getName() const;
    std::optional<std::string> getPhone() const;
    std::optional<std::string> getAddress() const;

   private:
    int id;
    std::string name;
    std::optional<std::string> phone;
    std::optional<std::string> address;
};