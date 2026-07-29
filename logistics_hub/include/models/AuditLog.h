#pragma once
#include <optional>
#include <string>

class AuditLog {
   public:
    AuditLog();
    AuditLog(int id, const std::string& entity_type, int entity_id, const std::string& operation,
             std::optional<int> performed_by, const std::string& created_at,
             const std::string& details);
    ~AuditLog() = default;

    int getId() const;
    const std::string& getEntityType() const;
    int getEntityId() const;
    const std::string& getOperation() const;
    std::optional<int> getPerformedBy() const;
    const std::string& getCreatedAt() const;
    const std::string& getDetails() const;

   private:
    int id;
    std::string entity_type;
    int entity_id;
    std::string operation;
    std::optional<int> performed_by;
    std::string created_at;
    std::string details;
};