// ItemTemplateStore.h
#pragma once
#include "game/Items/ItemTemplate.h"
#include <cstdint>
#include <map>
#include <vector>

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