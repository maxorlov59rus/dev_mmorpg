// DbPool.h — пул соединений базы данных (заглушка)
#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace dm {
namespace common {
namespace database {

class DbConnection {
public:
    bool Execute(const std::string &sql) { return true; }
    bool Query(const std::string &sql) { return true; }
};

class DbPool {
public:
    static DbPool &Instance();
    void Init(const std::string &host, uint16_t port, const std::string &user,
              const std::string &password, const std::string &database, size_t poolSize);
    DbConnection *Acquire();
    void Release(DbConnection *conn);
    void Shutdown();
    size_t GetPoolSize() const { return m_poolSize; }

private:
    DbPool() = default;
    ~DbPool();
    std::vector<DbConnection *> m_connections;
    size_t m_poolSize{0};
};

} // namespace database
} // namespace common
} // namespace dm