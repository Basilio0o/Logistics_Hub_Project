#include "models/Supply/Supply.h"

Supply::Supply()
    : id(0),
      supplier_id(0),
      received_by(std::nullopt),
      received_at(std::nullopt),
      status("pending") {}

Supply::Supply(int id, int supplier_id, std::optional<int> received_by,
               std::optional<std::string> received_at, const std::string& status)
    : id(id),
      supplier_id(supplier_id),
      received_by(received_by),
      received_at(received_at),
      status(status) {}

int Supply::getId() const {
    return id;
}

int Supply::getSupplierId() const {
    return supplier_id;
}

std::optional<int> Supply::getReceivedBy() const {
    return received_by;
}

std::optional<std::string> Supply::getReceivedAt() const {
    return received_at;
}

const std::string& Supply::getStatus() const {
    return status;
}