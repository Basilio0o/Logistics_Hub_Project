#pragma once
#include <string>

class Order {
   public:
    Order();
    Order(int id, int district_id, const std::string& priority, const std::string& status,
          const std::string& created_at);
    ~Order() = default;

    int getId() const;
    int getDistrictId() const;
    const std::string& getPriority() const;
    const std::string& getStatus() const;
    const std::string& getCreatedAt() const;

   private:
    int id;
    int district_id;
    std::string priority;
    std::string status;
    std::string created_at;
};