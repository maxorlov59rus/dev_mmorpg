#include "EquipmentMgr.h"
#include "../Progression/ProgressionCore.h"
#include <cmath>
#include <cstring>

namespace dm {

static uint8 SlotToInventoryType(uint8 slot) {
    switch (slot) {
        case SLOT_MAINHAND: return 25; case SLOT_OFFHAND: return 26;
        case SLOT_HEAD: return 1; case SLOT_SHOULDER: return 3; case SLOT_CHEST: return 5;
        case SLOT_LEGS: return 6; case SLOT_HANDS: return 7; default: return 0;
    }
}

bool EquipmentMgr::Equip(uint32 itemEntry, uint8 slot, uint8 playerClass, uint32 playerLevel, uint16& outErr) {
    if (slot >= SLOT_COUNT) { outErr = 4; return false; }
    const ItemTemplate* tpl = ItemTemplateStore::Instance().Get(itemEntry);
    if (!tpl) { outErr = 5 /*unknown entry*/; return false; }
    auto chk = ItemTemplateStore::Instance().CanEquip(*tpl, playerClass, playerLevel, slot);
    if (!chk.Ok) { outErr = chk.Reason; return false; }
    if (tpl->InventoryType != SlotToInventoryType(slot)) { outErr = 4; return false; }
    _slots[slot] = EquippedItem{itemEntry, tpl->Durability};
    outErr = 0;
    return true;
}

bool EquipmentMgr::Unequip(uint8 slot) {
    if (slot >= SLOT_COUNT) return false;
    _slots[slot] = {};
    return true;
}

EquipStatSnapshot EquipmentMgr::BuildSnapshot() const {
    EquipStatSnapshot s;
    const auto& store = ItemTemplateStore::Instance();
    for (uint8 sl = 0; sl < SLOT_COUNT; ++sl) {
        const auto& eq = _slots[sl];
        if (!eq.Entry) continue;
        const ItemTemplate* tpl = store.Get(eq.Entry);
        if (!tpl) continue;
        if (tpl->Class == ITEM_ARMOR) {
            float mult = (tpl->InventoryType == 5) ? 1.0f : (tpl->InventoryType == 1 ? 0.70f : 0.60f);
            s.Armor += uint32(std::round(tpl->ItemLevel * 1.2f * mult));
        }
        for (int i = 0; i < 3; ++i) {
            if (!tpl->Stats.Type[i]) continue;
            s.StatTypes[i] = tpl->Stats.Type[i];
            s.StatTotals[i] += tpl->Stats.Value[i];
        }
        if (sl == SLOT_MAINHAND) {
            s.DisplayMainhand = tpl->DisplayId;
            s.MainhandQuality = tpl->Quality;
            if (tpl->HasProcEnchant()) {
                s.ProcSpellId = tpl->SpellId1;
                // Element hint: контракт с dm_items.enchant_effects (1 fire, 2 ice, 3 holy, 4 shade, 5 storm)
                s.EnchantElementHint = uint8(1 + (tpl->SpellId1 % 5));
            }
        } else if (sl == SLOT_OFFHAND) {
            s.DisplayOffhand = tpl->DisplayId;
        }
    }
    return s;
}

uint32 EquipmentMgr::RollProcOnHit(int32 hitCritRating, uint32 rollSeed) const {
    if (!_slots[SLOT_MAINHAND].Entry) return 0;
    const ItemTemplate* tpl = ItemTemplateStore::Instance().Get(_slots[SLOT_MAINHAND].Entry);
    if (!tpl || !tpl->HasProcEnchant()) return 0;
    // Детерминированный LCG вместо rand(): P(proc) ~ 5% + 0.05% * rating
    uint32 v = (rollSeed * 1103515245u + 12345u) >> 8;
    uint32 pctBase = 500 + uint32(hitCritRating) * 5; // в сотых долях %
    if ((v % 10000) < pctBase) return tpl->SpellId1;
    return 0;
}

void EquipmentMgr::SerializeSnapshot(const EquipStatSnapshot& s, std::vector<uint8>& out) {
    auto put = [&](const void* p, size_t n) {
        const uint8* b = static_cast<const uint8*>(p);
        out.insert(out.end(), b, b + n);
    };
    put(&s.Armor, 4);
    for (int i = 0; i < 3; ++i) put(&s.StatTypes[i], 4);
    for (int i = 0; i < 3; ++i) put(&s.StatTotals[i], 4);
    put(&s.ProcSpellId, 4);
    put(&s.DisplayMainhand, 4);
    put(&s.DisplayOffhand, 4);
    put(&s.MainhandQuality, 1);
    put(&s.EnchantElementHint, 1);
}

} // namespace dm
