#include "ItemTemplate.h"

namespace dm {

ItemTemplateStore& ItemTemplateStore::Instance() {
    static ItemTemplateStore s;
    return s;
}

void ItemTemplateStore::LoadFromQuery(const std::vector<ItemTemplate>& rows) {
    _byEntry.clear();
    _byEntry.reserve(rows.size());
    for (const auto& r : rows)
        _byEntry.emplace(r.Entry, r);
}

const ItemTemplate* ItemTemplateStore::Get(uint32 entry) const {
    auto it = _byEntry.find(entry);
    return it == _byEntry.end() ? nullptr : &it->second;
}

ItemTemplateStore::EquipCheck ItemTemplateStore::CanEquip(const ItemTemplate& tpl, uint8 playerClass,
                                                          uint32 playerLevel, uint8 slot) const {
    EquipCheck c;
    if (!tpl.IsEquippable()) { c.Reason = 4; return c; }
    if (playerLevel < tpl.ItemLevel) { c.Reason = 1; return c; }
    if (!tpl.AllowsClass(playerClass)) { c.Reason = 3; return c; }
    // Проверка слота по InventoryType (упрощённо: оружие 25/26, броня 1..7)
    bool weaponSlot = (slot == 0 || slot == 1); // 0 main, 1 offhand/shield
    bool armorSlot = slot >= 2 && slot <= 9;
    if ((tpl.Class == ITEM_WEAPON && !weaponSlot) || (tpl.Class == ITEM_ARMOR && !armorSlot)) {
        c.Reason = 4; return c;
    }
    c.Ok = true;
    return c;
}

} // namespace dm
