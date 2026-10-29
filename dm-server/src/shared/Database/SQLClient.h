// SQLClient.h — заглушка
#pragma once
#include <cstdint>
#include <string>

namespace dm {
namespace shared {
namespace database {

class SQLClient {
public:
    static SQLClient &Instance();
    bool Query(const std::string &sql);
    bool Execute(const std::string &sql);
};

} // namespace database
} // namespace shared
} // namespace dm
