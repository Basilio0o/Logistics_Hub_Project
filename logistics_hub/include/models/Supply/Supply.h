#pragma once
#include <optional>
#include <string>

class Supply {
   public:
    Supply();
    Supply(int id, int supplier_id, std::optional<int> received_by,
           std::optional<std::string> received_at, const std::string& status);
    ~Supply() = default;

    int getId() const;
    int getSupplierId() const;
    std::optional<int> getReceivedBy() const;
    std::optional<std::string> getReceivedAt() const;
    const std::string& getStatus() const;

   private:
    int id;
    int supplier_id;
    std::optional<int> received_by;
    std::optional<std::string> received_at;
    std::string status;
};