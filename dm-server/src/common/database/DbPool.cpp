#include "common/database/DbPool.h"
#include "common/Logger.h"
#include <iostream>

namespace dm {
namespace common {
namespace database {

DbPool &DbPool::Instance() {
    static DbPool inst;
    return inst;
}

DbPool::~DbPool() {
    Shutdown();
}

void DbPool::Init(const std::string &host, uint16_t port, const std::string &user,
                  const std::string &password, const std::string &database, size_t poolSize) {
    auto& log = dm::common::Logger::Instance();
    log.Info("Creating database pool with " + std::to_string(poolSize) + " connections...");
    log.Info("DB Host: " + host + ":" + std::to_string(port));
    log.Info("DB User: " + user);
    log.Info("DB Name: " + database);
    m_poolSize = poolSize;
}

DbConnection *DbPool::Acquire() {
    return nullptr;
}

void DbPool::Release(DbConnection *conn) {
}

void DbPool::Shutdown() {
}

} // namespace database
} // namespace common
} // namespace dm
