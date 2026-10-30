// Config.h — INI-парсер
#pragma once
#include <cstdint>
#include <string>
#include <map>
#include <vector>

namespace dm {
namespace common {
namespace config {

class Config {
public:
    static Config &Instance();
    bool Load(const std::string &path);
    std::string GetString(const std::string &section, const std::string &key, const std::string &def = "") const;
    int GetInt(const std::string &section, const std::string &key, int def = 0) const;
    bool GetBool(const std::string &section, const std::string &key, bool def = false) const;
private:
    std::map<std::string, std::map<std::string, std::string>> m_data;
};

} // namespace config
} // namespace common
} // namespace dm
