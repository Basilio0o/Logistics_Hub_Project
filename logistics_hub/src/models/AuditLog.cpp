#include "models/AuditLog.h"

AuditLog::AuditLog()
    : id(0),
      entity_type(""),
      entity_id(0),
      operation(""),
      performed_by(std::nullopt),
      created_at(""),
      details("{}") {}

AuditLog::AuditLog(int id, const std::string& entity_type, int entity_id,
                   const std::string& operation, std::optional<int> performed_by,
                   const std::string& created_at, const std::string& details)
    : id(id),
      entity_type(entity_type),
      entity_id(entity_id),
      operation(operation),
      performed_by(performed_by),
      created_at(created_at),
      details(details) {}

int AuditLog::getId() const {
    return id;
}

const std::string& AuditLog::getEntityType() const {
    return entity_type;
}

int AuditLog::getEntityId() const {
    return entity_id;
}

const std::string& AuditLog::getOperation() const {
    return operation;
}

std::optional<int> AuditLog::getPerformedBy() const {
    return performed_by;
}

const std::string& AuditLog::getCreatedAt() const {
    return created_at;
}

const std::string& AuditLog::getDetails() const {
    return details;
}