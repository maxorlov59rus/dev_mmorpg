#!/usr/bin/env python3
"""
Generate_Item_Data.py — детерминированный генератор 1200 предметов MMORPG «Чёрная Материя».

Вход : tools/item_gen_config.json (диапазоны StartID, Class/Subclass, MinLevel..MaxLevel, количества)
Выход: sql/upgrades/Insert_1200_Items.sql + data/mapping_displayid.csv

Формулы (согласованы с dm-server/src/game/Progression/ProgressionCore.h):
  Armor      = round(ilvl * 1.2 * slotMultiplier)
  WeaponDPS  = round(ilvl * 0.8)          -> dmg = DPS * speed
  StatValue  = round(ilvl * 0.9)           (основной стат предмета)
  Secondary  = round(ilvl * 0.45)          (crit/haste рейтинги, Rare+)
  BuyPrice   = ilvl^2 * qualityMult (медь)

Использование:
  python3 tools/Generate_Item_Data.py [--config PATH] [--out PATH] [--dry-run]
"""
import argparse
import csv
import json
import sys
from pathlib import Path

REPO = Path(__file__).resolve().parent.parent

# --- Enum'ы TrinityCore-style -------------------------------------------------
ITEM_CLASS = {"CONSUMABLE": 1, "WEAPON": 2, "ARMOR": 4, "TRADE_GOODS": 5, "QUEST": 6}
WEAPON_SUBCLASS = {  # subclass id -> (name, inventorytype, allowable_class_mask, delay)
    "sword":   (0, 25, 0x00000001 | 0x00000004 | 0x00000020, 2700),   # Warrior/Rogue/Paladin-like
    "dagger":  (3, 26, 0x00000004 | 0x00000008, 1700),                # Rogue/Mage
    "staff":   (7, 25, 0x00000008 | 0x00000010, 3300),                # Mage/Warlock-like
    "mace":    (5, 25, 0x00000001 | 0x00000010, 3000),
}
ARMOR_SUBCLASS = {"cloth": (2, 0x00000008 | 0x00000010), "leather": (1, 0x00000004 | 0x00000008),
                  "mail": (3, 0x00000001 | 0x00000010), "plate": (4, 0x00000001)}
# Слот -> (InventoryType, множитель брони, суффикс имени)
ARMOR_SLOTS = [("head", 1, 0.70, "Helm"), ("shoulder", 3, 0.60, "Pauldrons"),
               ("chest", 5, 1.00, "Chestguard"), ("legs", 6, 0.90, "Legguards"),
               ("hands", 7, 0.55, "Gloves")]
STAT_TYPE = {"STRENGTH": 3, "AGILITY": 4, "STAMINA": 5, "INTELLECT": 6, "SPIRIT": 7,
             "CRIT_RATING": 31, "HASTE_RATING": 32, "MASTERY_RATING": 36}
QUALITY = {"poor": 0, "common": 1, "uncommon": 2, "rare": 4, "epic": 5}
RACE_ALL, CLASS_ALL = -1, -1


def esc(s: str) -> str:
    return s.replace("\\", "\\\\").replace("'", "\\'")


START_ID = 10001


def displayid_for(entry: int, base: int) -> int:
    """Детерминированный mapping: DisplayID = base + (ItemID - StartID) + 1.
    ItemID 10001..11200 -> DisplayID 50001..51200 (ТЗ «Финализация»)."""
    return base + (entry - START_ID) + 1


def weapon_dmg(dps: float, delay_ms: int) -> tuple[int, int]:
    total = dps * (delay_ms / 1000.0)
    lo = max(1, int(total * 0.85))
    hi = max(lo + 1, int(total * 1.15))
    return lo, hi


QUALITY_BY_NUM = {v: k for k, v in QUALITY.items()}


def resolve_quality(q):
    """Config может задавать quality числом (ТЗ: Quality=0/1/4/5) или именем."""
    if isinstance(q, int):
        return q
    return QUALITY[q]


def gen_weapons(cfg, cfg_base, rows, mapping):
    next_entry = cfg["start_id"]
    for tier in cfg["tiers"]:
        q = resolve_quality(tier["quality"])
        names = tier.get("names", {})
        count = tier["count"]
        per = count // len(tier["types"])
        rem = count % len(tier["types"])
        stats_pool = tier.get("stats", [tier.get("stat_type", "STRENGTH")])
        for ti, wtype in enumerate(tier["types"]):
            sub, invt, allow, delay = WEAPON_SUBCLASS[wtype]
            n = per + (1 if ti < rem else 0)
            for i in range(n):
                lvl = tier["min_level"] + (i * (tier["max_level"] - tier["min_level"])) // max(1, n - 1) if n > 1 else tier["min_level"]
                ilvl = lvl
                dps = round(ilvl * 0.8)
                lo, hi = weapon_dmg(dps, delay)
                entry = next_entry
                next_entry += 1
                disp = displayid_for(entry, cfg_base)
                prefix = names.get(wtype, wtype.capitalize())
                suffix = ["of the Bear", "of Fury", "of Embers", "of the Void", "Reborn"][i % 5] if q >= 4 else ""
                name = f"{prefix} {['I','II','III','IV','V'][min(4,(lvl-tier['min_level'])//12)]}"
                if suffix:
                    name += f" {suffix}"
                sockets = min(2, (ilvl - 88) // 4 + 1) if tier.get("sockets") else 0
                proc_spell = 70000 + (entry % 400) if tier.get("proc") and i % 3 == 0 else 0
                stat1 = STAT_TYPE[stats_pool[i % len(stats_pool)]]
                statv = round(ilvl * 0.9)
                crit_h = round(ilvl * 0.45) if tier.get("secondary") else 0
                dur = 90 + ilvl * 3
                buy = ilvl * ilvl * (q + 1) * 10
                sell = buy // 10
                row = (entry, ITEM_CLASS["WEAPON"], sub, name, disp, q, 0, buy, sell, invt,
                       allow, RACE_ALL, ilvl, dur, lo, hi, delay,
                       stat1, statv,
                       STAT_TYPE["CRIT_RATING"] if crit_h else 0, crit_h,
                       STAT_TYPE["HASTE_RATING"] if crit_h else 0, crit_h,
                       sockets, proc_spell, 1 if proc_spell else 0)
                rows.append(row)
                mapping.append((entry, disp, f"/Game/Items/Weapons/{wtype.capitalize()}/SM_{wtype}_{disp}",
                                f"/Game/Items/Icons/T_{wtype}_{disp}"))


def gen_armor(cfg, cfg_base, rows, mapping):
    for family in cfg["families"]:
        armor_type = family["armor"]
        sub, allow = ARMOR_SUBCLASS[armor_type]
        cls_name = family["class_name"]
        count = family["count"]
        per_slot = count // len(ARMOR_SLOTS)
        rem = count % len(ARMOR_SLOTS)
        for si, (slot, invt, mult, suffix) in enumerate(ARMOR_SLOTS):
            n = per_slot + (1 if si < rem else 0)
            for i in range(n):
                lvl = family["min_level"] + (i * (family["max_level"] - family["min_level"])) // max(1, n - 1) if n > 1 else family["min_level"]
                ilvl = lvl
                q = QUALITY["common"] if ilvl < 21 else QUALITY["uncommon"] if ilvl < 45 else QUALITY["rare"] if ilvl < 80 else QUALITY["epic"]
                armor = round(ilvl * 1.2 * mult)
                stam = round(ilvl * 0.9)
                main_stat = STAT_TYPE[family["main_stat"]]
                main_v = round(ilvl * 0.6)
                sec = round(ilvl * 0.45) if ilvl >= 61 else 0
                entry = family["start_id"] + si * n + i
                disp = displayid_for(entry, cfg_base)
                name = f"{cls_name} {['Worn','Sturdy','Runed','Aetherium','Umbral'][min(4,(ilvl-1)//24)]} {suffix}"
                dur = 100 + ilvl * 4
                buy = ilvl * ilvl * (q + 1) * 6
                sockets = 1 if (ilvl >= 80 and i % 2 == 0) else 0
                # Layout как у оружия: ...durability, dmg_min(броня), dmg_max(0), delay(0), stat1..3, sockets, spellid, trigger
                row = (entry, ITEM_CLASS["ARMOR"], sub, name, disp, q, 0, buy, buy // 10, invt,
                       allow, RACE_ALL, ilvl, dur, armor, 0, 0,
                       STAT_TYPE["STAMINA"], stam, main_stat, main_v,
                       STAT_TYPE["HASTE_RATING"] if sec else 0, sec,
                       sockets, 0, 0)
                rows.append(row)
                mapping.append((entry, disp, f"/Game/Items/Armor/{armor_type.capitalize()}/{slot[:2].upper()}_{armor_type}_{slot}_{disp}",
                                f"/Game/Items/Icons/T_armor_{slot}_{disp}"))


def gen_consumables(cfg, cfg_base, rows, mapping):
    e = cfg["start_id"]
    spells = cfg.get("spell_map", {})
    for i in range(cfg["food"]["count"]):
        lvl = cfg["food"]["min_level"] + (i * (cfg["food"]["max_level"] - cfg["food"]["min_level"])) // max(1, cfg["food"]["count"] - 1)
        entry, disp = e, displayid_for(e, cfg_base); e += 1
        heal = round(lvl * 2.5)
        # entry,class,sub,name,disp,q,flags,buy,sell,invt(-1),allowC,allowR,ilvl,dur(0),dmg_min(heal),dmg_max,delay,stat1..3,sockets,spellid,trigger
        rows.append((entry, ITEM_CLASS["CONSUMABLE"], 4, f"Hearty Ration ({lvl})", disp, QUALITY["common"],
                     0, lvl * 40, lvl * 4, -1, CLASS_ALL, RACE_ALL, lvl, 0, heal, 0, 0,
                     0, 0, 0, 0, 0, 0, 0, 0, 0))
        mapping.append((entry, disp, "", f"/Game/Items/Icons/T_food_{disp}"))
    for i in range(cfg["scrolls"]["count"]):
        entry, disp = e, displayid_for(e, cfg_base); e += 1
        kind = "Teleport" if i % 2 == 0 else "Buff"
        spell = spells.get(kind.lower(), 60000 + i)
        rows.append((entry, ITEM_CLASS["CONSUMABLE"], 5, f"Scroll of {'Recall' if kind=='Teleport' else 'Empowerment'}",
                     disp, QUALITY["uncommon"], 0, 500 + i * 20, 50, -1, CLASS_ALL, RACE_ALL, 20 + i, 0, 0, 0, 0,
                     0, 0, 0, 0, 0, 0, 0, spell, 1))
        mapping.append((entry, disp, "", f"/Game/Items/Icons/T_scroll_{disp}"))
    for i in range(cfg["potions"]["count"]):
        entry, disp = e, displayid_for(e, cfg_base); e += 1
        is_mana = i % 2 == 1
        lvl = cfg["potions"]["min_level"] + (i * (cfg["potions"]["max_level"] - cfg["potions"]["min_level"])) // max(1, cfg["potions"]["count"] - 1)
        val = round(lvl * (3.0 if not is_mana else 2.2))
        name = f"{'Mana' if is_mana else 'Healing'} Elixir L{lvl}"
        rows.append((entry, ITEM_CLASS["CONSUMABLE"], 2, name, disp,
                     QUALITY["rare"] if lvl > 60 else QUALITY["uncommon"], 0, lvl * lvl * 3, lvl, -1,
                     CLASS_ALL, RACE_ALL, lvl, 0, val, 0, 0,
                     0, 0, 0, 0, 0, 0, 0, 55000 + (0 if not is_mana else 1), 0))
        mapping.append((entry, disp, "", f"/Game/Items/Icons/T_potion_{disp}"))


def gen_world(cfg, cfg_base, rows, mapping):
    e = cfg["start_id"]
    mats = ["Ore", "Herb", "Cloth Scrap", "Essence", "Eternium Shard"]
    for i in range(cfg["resources"]["count"]):
        entry, disp = e, displayid_for(e, cfg_base); e += 1
        m = mats[i % len(mats)]
        lvl = cfg["resources"]["min_level"] + (i * (cfg["resources"]["max_level"] - cfg["resources"]["min_level"])) // max(1, cfg["resources"]["count"] - 1)
        stack = [20, 20, 10, 5, 2][i % 5]
        rows.append((entry, ITEM_CLASS["TRADE_GOODS"], 7, f"{m} ({['Rough','Coarse','Fine','Solid','Mythic'][min(4,lvl//20)]})",
                     disp, QUALITY["common"] if lvl < 40 else QUALITY["uncommon"], 0, lvl * 15, lvl, -1,
                     CLASS_ALL, RACE_ALL, lvl, 0, 0, 0, 0,
                     0, 0, 0, 0, 0, 0, 0, 0, 0))
        mapping.append((entry, disp, "", f"/Game/Items/Icons/T_mat_{m.split()[0].lower()}_{disp}"))
    keys = cfg["world_items"]["count"] // 2
    ammo = cfg["world_items"]["count"] - keys
    for i in range(keys):
        entry, disp = e, displayid_for(e, cfg_base); e += 1
        rows.append((entry, ITEM_CLASS["QUEST"], 0, f"Key Item #{entry}", disp, QUALITY["common"], 0x10,
                     0, 0, -1, CLASS_ALL, RACE_ALL, 1, 0, 0, 0, 0,
                     0, 0, 0, 0, 0, 0, 0, 0, 0))
        mapping.append((entry, disp, "", f"/Game/Items/Icons/T_key_{disp}"))
    for i in range(ammo):
        entry, disp = e, displayid_for(e, cfg_base); e += 1
        kind = ["Arrow", "Bullet", "Thrown Axe"][i % 3]
        lvl = 1 + (i * 94) // max(1, ammo - 1)
        rows.append((entry, ITEM_CLASS["WEAPON"], 8, f"{kind} Bundle ({lvl})", disp, QUALITY["common"], 0,
                     lvl * 8, 1, 25, CLASS_ALL, RACE_ALL, lvl, 0, round(lvl * 0.8), 0, 0,
                     0, 0, 0, 0, 0, 0, 0, 0, 0))
        mapping.append((entry, disp, "", f"/Game/Items/Icons/T_ammo_{disp}"))


COLUMNS = ["entry", "class", "subclass", "name", "displayid", "quality", "flags", "BuyPrice", "SellPrice",
           "InventoryType", "AllowableClass", "AllowableRace", "ItemLevel", "durability",
           "dmg_min", "dmg_max", "delay",
           "stat_type1", "stat_value1", "stat_type2", "stat_value2", "stat_type3", "stat_value3",
           "stat_type4", "stat_value4", "stat_type5", "stat_value5", "stat_type6", "stat_value6",
           "stat_type7", "stat_value7", "stat_type8", "stat_value8", "stat_type9", "stat_value9",
           "stat_type10", "stat_value10",
           "socket_count", "spellid_1", "spelltrigger_1"]
NCOL = len(COLUMNS)  # 40 колонок: 26 базовых TRC-style + 7 дополнительных пар статов


def normalize_row(r):
    """Генераторы отдают компактные кортежи (24/26 полей с stat_type1..3).
    Распаковываем их в полный состав COLUMNS: поля после stat_value3 (socket_count, spellid_1,
    spelltrigger_1) переносятся в конец, stat_type4..10/stat_value4..10 заполняются нулями."""
    vals = list(r)
    if len(vals) <= 26:
        # layout generator'а: ... stat_type3, stat_value3, socket_count, spellid_1, spelltrigger_1
        base = vals[:23]                      # до stat_value3 включительно
        rest = vals[23:] if len(vals) > 23 else []
        rest = (rest + [0, 0, 0])[:3]         # socket_count, spellid_1, spelltrigger_1
        pad = [0] * (NCOL - len(base) - len(rest))
        return (base + pad + rest)[:NCOL]
    return vals[:NCOL]


def emit_sql(rows, out_path: Path):
    header = ("-- Insert_1200_Items.sql — сгенерировано tools/Generate_Item_Data.py\n"
              f"-- Предметов: {len(rows)}\n"
              "-- Не редактировать вручную: правьте конфиг и перегенерируйте.\n\n"
              "SET NAMES utf8mb4;\n\n")
    col_list = ", ".join(f"`{c}`" for c in COLUMNS)
    chunks = []
    BATCH = 100
    for i in range(0, len(rows), BATCH):
        batch = rows[i:i + BATCH]
        values = []
        for r in batch:
            r = normalize_row(r)
            cells = []
            for idx, val in enumerate(r):
                if COLUMNS[idx] == "name":
                    cells.append(f"'{esc(str(val))}'")
                else:
                    cells.append(str(int(val)))
            values.append("(" + ",".join(cells) + ")")
        chunks.append(f"INSERT INTO `item_template` ({col_list}) VALUES\n" + ",\n".join(values) + ";\n")
    out_path.write_text(header + "\n".join(chunks), encoding="utf-8")


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--config", default=str(REPO / "tools" / "item_gen_config.json"))
    ap.add_argument("--out", default=str(REPO / "sql" / "upgrades" / "Insert_1200_Items.sql"))
    ap.add_argument("--mapping", default=str(REPO / "data" / "mapping_displayid.csv"))
    ap.add_argument("--dry-run", action="store_true")
    args = ap.parse_args()

    cfg = json.loads(Path(args.config).read_text(encoding="utf-8"))
    cfg_base = cfg["displayid_base"]
    rows, mapping = [], []

    gen_weapons(cfg["weapons"], cfg_base, rows, mapping)
    gen_armor(cfg["armor"], cfg_base, rows, mapping)
    gen_consumables(cfg["consumables"], cfg_base, rows, mapping)
    gen_world(cfg["world"], cfg_base, rows, mapping)

    entries = sorted(r[0] for r in rows)
    assert len(entries) == len(set(entries)), "Duplicate item entries!"
    assert entries[0] == cfg["StartID"] and entries[-1] == cfg["StartID"] + cfg["expected_total"] - 1, \
        f"Entry range must be {cfg['StartID']}..{cfg['StartID'] + cfg['expected_total'] - 1}, got {entries[0]}..{entries[-1]}"
    assert all(b - a == 1 for a, b in zip(entries, entries[1:])), "Entries must be contiguous"
    total_expected = cfg["expected_total"]
    print(f"Generated {len(rows)} items (expected {total_expected})")
    if len(rows) != total_expected:
        print("ERROR: count mismatch", file=sys.stderr)
        return 2

    if args.dry_run:
        by_kind = {}
        for r in rows:
            key = (r[1], r[2])
            by_kind[key] = by_kind.get(key, 0) + 1
        for k, v in sorted(by_kind.items()):
            print(f"  class={k[0]} subclass={k[1]}: {v}")
        return 0

    emit_sql(rows, Path(args.out))
    with open(args.mapping, "w", newline="", encoding="utf-8") as f:
        w = csv.writer(f)
        w.writerow(["ItemID", "DisplayID", "UE5MeshPath", "UE5IconPath"])
        w.writerows(mapping)
    print(f"Wrote {args.out} and {args.mapping}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
