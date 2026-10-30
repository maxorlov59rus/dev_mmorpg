// ExtendedIconMgr.h — Расширенные иконки: навыки, пассивки, вещи, оружие, броня
#pragma once
#include <cstdint>
#include <unordered_map>
#include <string>
#include <vector>

namespace dm {

struct ExtendedIcon {
    uint32_t id;
    std::string name;
    std::string filePath;
    std::string category;
    std::string subcategory;
    uint32_t sizePx;
    uint8_t quality;
    std::string descriptionRu;
    std::string descriptionEn;
};

class ExtendedIconMgr {
public:
    static ExtendedIconMgr &Instance();

    const ExtendedIcon *GetIcon(uint32_t id) const;
    const ExtendedIcon *GetIconByName(const std::string &name) const;
    std::vector<ExtendedIcon> GetIconsByCategory(const std::string &category) const;
    std::vector<ExtendedIcon> GetIconsBySubcategory(const std::string &subcategory) const;
    bool HasIcon(uint32_t id) const;

    void LoadAllIcons();

    bool IsEnabled() const;
};

} // namespace dm