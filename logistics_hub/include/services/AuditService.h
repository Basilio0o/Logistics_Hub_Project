#pragma once
#include <optional>
#include <string>
#include <vector>

#include "dal/AuditLogRepository.h"
#include "models/AuditLog.h"

class AuditService {
   public:
    explicit AuditService(Db& db) : auditrepo(db) {}

    bool log(const std::string& entity_type, int entity_id, const std::string& operation,
             const std::string& details);
    std::vector<AuditLog> getLogs(std::optional<int> user_id,
                                  const std::optional<std::string>& operation,
                                  const std::optional<std::string>& date_from,
                                  const std::optional<std::string>& date_to);

   private:
    AuditLogRepository auditrepo;
    static bool isValidEntityType(const std::string& type);
    static bool isValidOperation(const std::string& op);
    static bool isValidTimestamp(const std::string& time);
};