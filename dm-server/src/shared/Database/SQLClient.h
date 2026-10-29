// SQLClient.h — заглушка
#pragma once

namespace dm {
namespace shared {
namespace database {
class SQLClient {
public:
    static SQLClient &Instance();
};
} // namespace database
} // namespace shared
} // namespace dm
