#include "models/Parcel/ParcelHistory.h"

ParcelHistory::ParcelHistory()
    : id(0),
      parcel_id(0),
      old_status(std::nullopt),
      new_status(""),
      changed_at(""),
      changed_by(std::nullopt) {}

ParcelHistory::ParcelHistory(int id, int parcel_id, std::optional<std::string> old_status,
                             const std::string& new_status, const std::string& changed_at,
                             std::optional<int> changed_by)
    : id(id),
      parcel_id(parcel_id),
      old_status(old_status),
      new_status(new_status),
      changed_at(changed_at),
      changed_by(changed_by) {}

int ParcelHistory::getId() const {
    return id;
}

int ParcelHistory::getParcelId() const {
    return parcel_id;
}

std::optional<std::string> ParcelHistory::getOldStatus() const {
    return old_status;
}

const std::string& ParcelHistory::getNewStatus() const {
    return new_status;
}

const std::string& ParcelHistory::getChangedAt() const {
    return changed_at;
}

std::optional<int> ParcelHistory::getChangedBy() const {
    return changed_by;
}