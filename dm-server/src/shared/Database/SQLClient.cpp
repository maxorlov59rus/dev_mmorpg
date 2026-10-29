#include "shared/Database/SQLClient.h"
#include <iostream>

namespace dm {
namespace shared {
namespace database {

SQLClient &SQLClient::Instance() {
    static SQLClient inst;
    return inst;
}

bool SQLClient::Query(const std::string &sql) {
    std::cout << "SQL Query: " << sql << std::endl;
    return true;
}

bool SQLClient::Execute(const std::string &sql) {
    std::cout << "SQL Execute: " << sql << std::endl;
    return true;
}

} // namespace database
} // namespace shared
} // namespace dm
