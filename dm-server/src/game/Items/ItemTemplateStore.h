// ItemTemplateStore.h
#pragma once
#include <cstdint>
#include <vector>
#include <map>
#include "game/Items/ItemTemplate.h"

namespace dm {

class ItemTemplateStore {
public:
    static ItemTemplateStore &Instance();
    void LoadFromQuery(void *queryResult);
    const ItemTemplate *GetTemplate(uint32_t entry) const;
private:
    std::map<uint32_t, ItemTemplate> m_templates;
};

} // namespace dm
