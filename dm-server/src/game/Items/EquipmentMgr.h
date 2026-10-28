#pragma once
// EquipmentMgr.h — серверная авторитетная экипировка: слоты, пересчёт статов,
// отправка SMSG_ITEM_EQUIP_UPDATE (0x2037), расчёт proc-оружия для боевого пайплайна.
#include "ItemTemplate.h"
#include <array>
#include <cstdint>
#include <vector>

namespace dm {

using uint8 = std::uint8_t; using uint16 = std::uint16_t; using uint32 = std::uint32_t; using int32 = std::int32_t;

enum EquipSlot : uint8 { SLOT_MAINHAND = 0, SLOT_OFFHAND = 1, SLOT_HEAD = 2, SLOT_SHOULDER = 3,
                         SLOT_CHEST = 4, SLOT_LEGS = 5, SLOT_HANDS = 6, SLOT_COUNT = 7 };

struct EquippedItem { uint32 Entry = 0; uint16 CurDurability = 0; };

class PlayerRoster; // fwd (TODO(KERNEL): связать с реальным Player)

// Снимок агрегированных статов экипировки — уходит клиенту в SMSG_ITEM_EQUIP_UPDATE
struct EquipStatSnapshot {
    uint32 Armor = 0;
    std::array<int32, 3> StatTypes{};
    std::array<int32, 3> StatTotals{};   // просуммированные STR/AGI/... рейтинги
    uint32 ProcSpellId = 0;              // активный proc оружия (TRIGGER_PROC_ON_HIT)
    uint32 DisplayMainhand = 0, DisplayOffhand = 0;
    uint8 MainhandQuality = 0;
    uint8 EnchantElementHint = 0;        // 0 none, 1 fire, 2 ice, 3 holy, 4 shade, 5 storm (клиент выбирает NS_*)
};

class EquipmentMgr {
public:
    explicit EquipmentMgr(uint32 ownerGuid) : _owner(ownerGuid) {}

    bool Equip(uint32 itemEntry, uint8 slot, uint8 playerClass, uint32 playerLevel, uint16& outErr);
    bool Unequip(uint8 slot);
    const EquippedItem& GetSlot(uint8 slot) const { return _slots[slot]; }

    // Пересчёт: armor = sum ilvl*1.2*slotMult (ProgressionCore), стат-суммы, proc оружия
    EquipStatSnapshot BuildSnapshot() const;

    // Вызывается из боевого пайплайна после удара; возвращает spellId proc'а (0 — нет прока)
    uint32 RollProcOnHit(int32 hitCritRating, uint32 rollSeed) const;

    // TODO(KERNEL): WorldPacket p(SMSG_ITEM_EQUIP_UPDATE); p << snapshot; session->SendPacket(&p);
    static void SerializeSnapshot(const EquipStatSnapshot& s, std::vector<uint8>& outWire);

private:
    uint32 _owner;
    std::array<EquippedItem, SLOT_COUNT> _slots{};
};

} // namespace dm
