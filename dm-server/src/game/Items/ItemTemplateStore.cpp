#include "game/Items/ItemTemplateStore.h"

namespace dm {

ItemTemplateStore &ItemTemplateStore::Instance() {
    static ItemTemplateStore inst;
    return inst;
}

void ItemTemplateStore::LoadFromQuery(void *queryResult) {
    // TODO: SELECT * FROM item_template
    // for each row: ItemTemplate t; t.entry = row.entry; ...; m_templates[t.entry] = t;
}

const ItemTemplate *ItemTemplateStore::GetTemplate(uint32_t entry) const {
    auto it = m_templates.find(entry);
    if (it == m_templates.end()) return nullptr;
    return &it->second;
}

} // namespace dm
