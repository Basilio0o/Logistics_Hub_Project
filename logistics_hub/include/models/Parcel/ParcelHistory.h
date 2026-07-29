#pragma once
#include <optional>
#include <string>

class ParcelHistory {
   public:
    ParcelHistory();
    ParcelHistory(int id, int parcel_id, std::optional<std::string> old_status,
                  const std::string& new_status, const std::string& changed_at,
                  std::optional<int> changed_by);
    ~ParcelHistory() = default;

    int getId() const;
    int getParcelId() const;
    std::optional<std::string> getOldStatus() const;
    const std::string& getNewStatus() const;
    const std::string& getChangedAt() const;
    std::optional<int> getChangedBy() const;

   private:
    int id;
    int parcel_id;
    std::optional<std::string> old_status;
    std::string new_status;
    std::string changed_at;
    std::optional<int> changed_by;
};