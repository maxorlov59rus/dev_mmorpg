#include "common/config/Config.h"
#include <fstream>
#include <sstream>
#include <algorithm>

namespace dm {
namespace common {
namespace config {

Config &Config::Instance() {
    static Config inst;
    return inst;
}

bool Config::Load(const std::string &path) {
    std::ifstream file(path);
    if (!file.is_open()) return false;
    std::string line, section;
    while (std::getline(file, line)) {
        // trim
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);
        if (line.empty() || line[0] == ';' || line[0] == '#') continue;
        if (line[0] == '[' && line.back() == ']') {
            section = line.substr(1, line.size() - 2);
            continue;
        }
        size_t eq = line.find('=');
        if (eq == std::string::npos) continue;
        std::string key = line.substr(0, eq);
        std::string val = line.substr(eq + 1);
        // trim key/val
        key.erase(0, key.find_first_not_of(" \t"));
        key.erase(key.find_last_not_of(" \t") + 1);
        val.erase(0, val.find_first_not_of(" \t"));
        val.erase(val.find_last_not_of(" \t") + 1);
        m_data[section][key] = val;
    }
    return true;
}

std::string Config::GetString(const std::string &section, const std::string &key, const std::string &def) const {
    auto sit = m_data.find(section);
    if (sit == m_data.end()) return def;
    auto kit = sit->second.find(key);
    if (kit == sit->second.end()) return def;
    return kit->second;
}

int Config::GetInt(const std::string &section, const std::string &key, int def) const {
    std::string val = GetString(section, key, "");
    if (val.empty()) return def;
    try { return std::stoi(val); } catch (...) { return def; }
}

bool Config::GetBool(const std::string &section, const std::string &key, bool def) const {
    std::string val = GetString(section, key, "");
    if (val.empty()) return def;
    if (val == "true" || val == "1" || val == "yes") return true;
    if (val == "false" || val == "0" || val == "no") return false;
    return def;
}

} // namespace config
} // namespace common
} // namespace dm
