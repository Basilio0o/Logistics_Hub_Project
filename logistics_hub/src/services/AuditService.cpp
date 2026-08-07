#include "services/AuditService.h"

#include <iomanip>
#include <iostream>
#include <sstream>

bool AuditService::log(const std::string& entity_type, int entity_id, const std::string& operation,
                       const std::string& details) {
    if (isValidEntityType(entity_type) && isValidOperation(operation)) {
        auditrepo.add(entity_type, entity_id, operation, details);
        return true;
    }
    std::cout << "Ошибка: неверное название сущности или неверное название операции" << std::endl;

    return false;
}

std::vector<AuditLog> AuditService::getLogs(std::optional<int> user_id,
                                            const std::optional<std::string>& operation,
                                            const std::optional<std::string>& date_from,
                                            const std::optional<std::string>& date_to) {
    if (operation.has_value() && !isValidOperation(*operation)) return {};
    if ((date_from.has_value() && !isValidTimestamp(*date_from)) ||
        (date_to.has_value() && !isValidTimestamp(*date_to))) {
        return {};
    }

    return auditrepo.getLogs(user_id, operation, date_from, date_to);
}

bool AuditService::isValidEntityType(const std::string& type) {
    if (type != "shipment" && type != "order" && type != "parcel" && type != "item" &&
        type != "cell")
        return false;
    return true;
}

bool AuditService::isValidOperation(const std::string& op) {
    if (op != "receive" && op != "assemble" && op != " dispatch " && op != " move " &&
        op != "update" && op != "delete")
        return false;
    return true;
}

bool AuditService::isValidTimestamp(const std::string& time) {
    const char* formats[] = {"%Y-%m-%d %H:%M:%S", "%Y-%m-%d"};
    for (const char* fmt : formats) {
        std::tm tm{};
        std::istringstream iss(time);
        iss >> std::get_time(&tm, fmt);
        if (!iss.fail() && iss.eof()) return true;
    }
    return false;
}