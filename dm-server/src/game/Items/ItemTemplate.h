#pragma once
// ItemTemplate.h — кэш item_template (dm_items) + валидация экипировки.
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace dm {

using uint8 = std::uint8_t;
using uint16 = std::uint16_t;
using uint32 = std::uint32_t;
using int32 = std::int32_t;

enum ItemClass : uint8 {
    ITEM_CONSUMABLE = 1,
    ITEM_WEAPON = 2,
    ITEM_ARMOR = 4,
    ITEM_TRADE = 5,
    ITEM_QUEST = 6
};
enum ItemQuality : uint8 {
    QUALITY_POOR = 0,
    QUALITY_COMMON = 1,
    QUALITY_UNCOMMON = 2,
    QUALITY_RARE = 4,
    QUALITY_EPIC = 5,
    QUALITY_LEGENDARY = 6
};
// Итоговый контракт «Финализация»: spelltrigger_1 = 1 => PROC (согласовано с SQL).
enum SpellTriggerType : uint8 { TRIGGER_USE = 0, TRIGGER_PROC_ON_HIT = 1, TRIGGER_EQUIP_CAST = 2 };

struct ItemStatBlock {
    static constexpr int kMaxStats = 10;
    uint8 Type[kMaxStats]{}; // stat_type1..10 (3=STR 4=AGI 5=STA 6=INT 7=SPI 31=CritRating 32=Haste
                             // 36=Mastery)
    int32 Value[kMaxStats]{};
};

struct ItemTemplate {
    uint32 Entry = 0;
    uint8 Class = 0;
    uint8 Subclass = 0;
    std::string Name;
    uint32 DisplayId = 0; // клиентский ID (mapping_displayid.csv)
    uint8 Quality = QUALITY_COMMON;
    uint32 Flags = 0;
    uint32 BuyPrice = 0, SellPrice = 0;
    uint8 InventoryType = 0;
    int32 AllowableClassMask = -1; // битовая маска классов
    int32 AllowableRaceMask = -1;
    uint16 ItemLevel = 0;
    uint16 Durability = 0;
    uint32 DmgMin = 0, DmgMax = 0, DelayMs = 0;
    ItemStatBlock Stats;
    uint8 SocketCount = 0;
    uint32 SpellId1 = 0;
    uint8 SpellTrigger1 = TRIGGER_USE;

    [[nodiscard]] double WeaponDps() const {
        return DelayMs ? (double(DmgMin + DmgMax) / 2.0) / (DelayMs / 1000.0) : 0.0;
    }
    [[nodiscard]] bool AllowsClass(uint8 classId) const {
        return AllowableClassMask < 0 || (AllowableClassMask & (1 << (classId - 1))) != 0;
    }
    [[nodiscard]] bool IsEquippable() const {
        return Class == ITEM_WEAPON || Class == ITEM_ARMOR;
    }
    [[nodiscard]] bool HasProcEnchant() const {
        return SpellTrigger1 == TRIGGER_PROC_ON_HIT && SpellId1 != 0;
    }
};

class ItemTemplateStore {
public:
    static ItemTemplateStore &Instance();

    // TODO(KERNEL): SELECT ... FROM dm_items.item_template через DatabaseWorker -> LoadFromQuery
    void LoadFromQuery(const std::vector<ItemTemplate> &rows);
    const ItemTemplate *Get(uint32 entry) const;

    struct EquipCheck {
        bool Ok = false;
        uint16 Reason = 0;
    }; // 1=level 2=race 3=class 4=slot 5=unknown
    EquipCheck CanEquip(const ItemTemplate &tpl, uint8 playerClass, uint32 playerLevel,
                        uint8 slot) const;

    size_t Count() const {
        return _byEntry.size();
    }

private:
    std::unordered_map<uint32, ItemTemplate> _byEntry;
};

} // namespace dm
