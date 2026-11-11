-- ============================================================================
-- Crafted_Items.sql -- Items created by crafting
-- Base: dm_world
-- ============================================================================

USE dm_world;

INSERT IGNORE INTO item_template (entry, class, subclass, name, displayid, Quality, BuyPrice, SellPrice, ItemLevel, AllowableClass, AllowableRace, Bonding, BuyCount, Material, dmg_min, dmg_max, delay, durability, InventoryType) VALUES
(14001, 2, 0, 'Iron Sword', 50001, 1, 50, 5, 5, 37, -1, 0, 1, 4, 5, 8, 2700, 40, 0),
(14002, 4, 0, 'Iron Shield', 50002, 1, 40, 4, 5, 37, -1, 0, 1, 4),
(14003, 2, 0, 'Steel Sword', 50003, 2, 150, 15, 15, 37, -1, 0, 1, 4),
(14004, 4, 0, 'Steel Shield', 50004, 2, 120, 12, 15, 37, -1, 0, 1, 4),
(14005, 2, 0, 'Mithril Sword', 50005, 3, 400, 40, 30, 37, -1, 0, 1, 4),
(14006, 4, 0, 'Mithril Shield', 50006, 3, 320, 32, 30, 37, -1, 0, 1, 4),
(14007, 2, 0, 'Titanium Sword', 50007, 4, 1000, 100, 50, 37, -1, 0, 1, 4),
(14008, 4, 0, 'Titanium Shield', 50008, 4, 800, 80, 50, 37, -1, 0, 1, 4);

INSERT IGNORE INTO item_template (entry, class, subclass, name, displayid, Quality, BuyPrice, SellPrice, ItemLevel, AllowableClass, AllowableRace, Bonding, BuyCount, Material) VALUES
(14010, 0, 0, 'Small Healing Potion', 51000, 1, 20, 2, 1, -1, -1, 0, 1, 0),
(14011, 0, 0, 'Medium Healing Potion', 51001, 2, 60, 6, 10, -1, -1, 0, 1, 0),
(14012, 0, 0, 'Large Healing Potion', 51002, 3, 150, 15, 25, -1, -1, 0, 1, 0),
(14013, 0, 0, 'Small Mana Potion', 51003, 1, 20, 2, 1, -1, -1, 0, 1, 0),
(14014, 0, 0, 'Medium Mana Potion', 51004, 2, 60, 6, 10, -1, -1, 0, 1, 0),
(14015, 0, 0, 'Large Mana Potion', 51005, 3, 150, 15, 25, -1, -1, 0, 1, 0),
(14016, 0, 0, 'Strength Potion', 51006, 2, 80, 8, 15, -1, -1, 0, 1, 0),
(14017, 0, 0, 'Agility Potion', 51007, 2, 80, 8, 15, -1, -1, 0, 1, 0),
(14018, 0, 0, 'Intellect Potion', 51008, 2, 80, 8, 15, -1, -1, 0, 1, 0),
(14019, 0, 0, 'Vitality Potion', 51009, 2, 80, 8, 15, -1, -1, 0, 1, 0),
(14020, 0, 0, 'Spirit Potion', 51010, 2, 80, 8, 15, -1, -1, 0, 1, 0),
(14021, 0, 0, 'Elixir of Immortality', 51011, 3, 300, 30, 40, -1, -1, 0, 1, 0);

INSERT IGNORE INTO item_template (entry, class, subclass, name, displayid, Quality, BuyPrice, SellPrice, ItemLevel, AllowableClass, AllowableRace, Bonding, BuyCount, Material) VALUES
(14030, 4, 0, 'Linen Shirt', 50010, 1, 30, 3, 5, 37, -1, 0, 1, 1),
(14031, 4, 0, 'Silk Shirt', 50011, 2, 90, 9, 15, 37, -1, 0, 1, 1),
(14032, 4, 0, 'Magic Shirt', 50012, 3, 240, 24, 30, 37, -1, 0, 1, 1),
(14033, 4, 0, 'Epic Shirt', 50013, 4, 600, 60, 50, 37, -1, 0, 1, 1),
(14040, 0, 0, 'Defibrillator', 50014, 2, 100, 10, 5, -1, -1, 0, 1, 0),
(14041, 0, 0, 'Explosive', 50015, 2, 50, 5, 10, -1, -1, 0, 1, 0),
(14042, 0, 0, 'Invisibility Gadget', 50016, 3, 200, 20, 20, -1, -1, 0, 1, 0),
(14043, 0, 0, 'Rocket Pack', 50017, 3, 400, 40, 30, -1, -1, 0, 1, 0),
(14044, 0, 0, 'Turret', 50018, 4, 800, 80, 40, -1, -1, 0, 1, 0),
(14050, 4, 0, 'Ring of Strength', 50019, 2, 60, 6, 5, 37, -1, 0, 1, 0),
(14051, 4, 0, 'Ring of Agility', 50020, 2, 60, 6, 5, 37, -1, 0, 1, 0),
(14052, 4, 0, 'Ring of Intellect', 50021, 2, 60, 6, 5, 37, -1, 0, 1, 0),
(14053, 4, 0, 'Amulet of Vitality', 50022, 2, 90, 9, 10, 37, -1, 0, 1, 0),
(14054, 4, 0, 'Amulet of Spirit', 50023, 2, 90, 9, 10, 37, -1, 0, 1, 0),
(14055, 4, 0, 'Ring of Power', 50024, 3, 300, 30, 30, 37, -1, 0, 1, 0),
(14056, 4, 0, 'Amulet of Immortality', 50025, 4, 800, 80, 50, 37, -1, 0, 1, 0);
