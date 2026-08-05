#pragma once

#include <optional>
#include <pqxx/pqxx>
#include <string>
#include <vector>

#include "dal/Db.h"
#include "models/AuditLog.h"

class AuditLogRepository {
   public:
    explicit AuditLogRepository(Db& db) : db(db) {}

    void add(const std::string& entity_type, int entity_id, const std::string& operation,
             const std::string& details);
    std::vector<AuditLog> getLogs(std::optional<int> user_id,
                                  const std::optional<std::string>& operation,
                                  const std::optional<std::string>& date_from,
                                  const std::optional<std::string>& date_to);

   private:
    Db& db;
    AuditLog mapAuditLog(const pqxx::row& row);
};