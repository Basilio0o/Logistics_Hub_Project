#include "dal/AuditLogRepository.h"

void AuditLogRepository::add(const std::string& entity_type, int entity_id,
                             const std::string& operation, const std::string& details) {
    db.execute_params(
        "INSERT INTO audit_logs (entity_type, entity_id, operation, performed_by, details) "
        "VALUES ($1, $2, $3, current_setting('app.current_user_id', true)::int, "
        "NULLIF($4, '')::jsonb)",
        entity_type, entity_id, operation, details);
}

std::vector<AuditLog> AuditLogRepository::getLogs(std::optional<int> user_id,
                                                  const std::optional<std::string>& operation,
                                                  const std::optional<std::string>& date_from,
                                                  const std::optional<std::string>& date_to) {
    auto result = db.query_params(
        "SELECT id, entity_type, entity_id, operation, performed_by, created_at, "
        "COALESCE(details::text, '') AS details "
        "FROM audit_logs "
        "WHERE ($1::int IS NULL OR performed_by = $1) "
        "AND ($2::varchar IS NULL OR operation = $2) "
        "AND ($3::timestamptz IS NULL OR created_at >= $3) "
        "AND ($4::timestamptz IS NULL OR created_at <= $4) "
        "ORDER BY created_at DESC, id DESC",
        user_id, operation, date_from, date_to);

    std::vector<AuditLog> logs;
    logs.reserve(result.size());

    for (const auto& row : result) {
        logs.push_back(mapAuditLog(row));
    }
    return logs;
}

AuditLog AuditLogRepository::mapAuditLog(const pqxx::row& row) {
    return AuditLog(row["id"].as<int>(), row["entity_type"].as<std::string>(),
                    row["entity_id"].as<int>(), row["operation"].as<std::string>(),
                    row["performed_by"].as<std::optional<int>>(),
                    row["created_at"].as<std::string>(), row["details"].as<std::string>());
}