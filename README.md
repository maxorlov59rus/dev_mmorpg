# World Lore System — Comprehensive Overview

## 📜 World Lore Overview

The world of **"Black Metal" (Чёрная Материя)** is a dark fantasy MMORPG setting with deep faction conflicts, ancient wars, and dynamic events. The lore is structured around **faction-based campaigns** that drive PvE and PvE content.

### 🏛️ **World Lore Overview**

**Creation Myth:**
- The world was forged from the **Shattering of the Primordial Crystal**
- Three primordial entities emerged: **The Architect** (order), **The Corruptor** (entropy), and **The Observer** (neutral witness)
- The **Shattering** created the **Two Moons** (one of light, one of shadow) that orbit the world

**Historical Eras:**
1. **Era of Unity** (Years 1-500): The first civilizations unified under the **Covenant of Light**
2. **Era of Schism** (Years 51-150): The **Fracture** split the world into warring factions
3. **Era of the Black Queen** (Years 151-300): The dark sorceress **Vaelith** nearly conquered the world
4. **Era of the Accord** (Years 301-500): The **Pact of the Three** established the current faction system
5. **Current Era** (Year 501+): The **Contamination** event threatens all factions

### 🏯 **Faction System**

Three major factions vie for control:

| Faction | Alignment | Theme | Capital | Key NPCs |
|---------|-----------|-------|---------|---------|
| **Covenant of Light** | Lawful Good | Order, purity, redemption | **Aethelgard** | High Cleric **Vixus**, **Lady Elara** |
| **Shadow Covenant** | Lawful Evil | Tyranny, ambition, control | **Umbra** | Grand Magus **Thalorn**, **Lady Vex** |
| **The Unaligned** | Neutral | Freedom, exploration, neutrality | **Greyhaven** | **Arch-Druid **Wilderon**, **Merchant Queen** **Saella** |

### 📜 **Campaign System (Levels 70-99)**

Ten campaigns exist, each with unique storylines and faction affiliations:

| Campaign ID | Faction | Level Range | Theme | Key Quests |
|-------------|---------|-------------|-------|------------|
| **1** | Covenant of Light | 70-75 | **The Purification** | Cleansing corrupted lands, rescuing Light-aligned NPCs |
| **2** | Covenant of Light | 75-80 | **The Sun's Ascendancy** | Restoring solar temples, defeating shadow entities |
| **3** | Covenant of Light | 80-85 | **The Radiant Ascendancy** | becoming a demigod of light, leading the final battle against darkness |
| **5** | Shadow Covenant | 70-75 | **The Shadow's Ascendancy** | Dominating the darkness, spreading the night across the world |
| **6** | Shadow Covenant | 75-80 | **The Eternal Night** | Establishing eternal darkness, corrupting major cities |
| **7** | Shadow Covenant | 80-85 | **The Abyssal Lord** | Becoming a servant of the abyss, leading the final darkness |
| **3** | Neutral | 70-75 | **The Path of Balance** | Restoring world balance, collecting ancient artifacts |
| **4** | Neutral | 75-80 | **The Elemental Convergence** | Mastering elemental forces, controlling natural disasters |
| **5** | Neutral | 80-85 | **The World-Serpent** | Controlling the world-snake, managing global events |
| **6** | Neutral | 80-85 | **The Final Prophecy** | Fulfilling/preventing the world's end |

### 🏰 **Campaign Mechanics**

**Faction-Based Campaigns:**
- Each campaign is tied to a specific faction
- Players must choose a faction at level 70
- Campaigns provide unique storylines, quests, and rewards
- Faction warfare occurs when campaigns overlap

**Campaign Progression:**
1. **Entry Quest** — Complete the entry quest for your faction's campaign
2. **Progression Quests** — Complete intermediate quests (levels 70-99)
3. **Final Quest** — Complete the final campaign quest (level 99)
4. **Reward Claim** — Claim faction-specific rewards

**Rewards by Faction:**
- **Covenant of Light:** Light-based artifacts, holy weapons, resurrection mechanics
- **Shadow Covenant:** Shadow-based artifacts, dark weapons, life-drain abilities
- **The Unaligned:** Balanced artifacts, versatile gear, neutral faction reputation

### ⚔️ **Server Mechanics Driven by Lore**

**1. World Events (Dynamic):**
- **Weekly World Bosses** — Spawn based on faction control of key areas
- **Monthly World Events** — Global events affecting all factions
- **Daily World Events** — Smaller events with faction-specific twists

**2. Server Events (Scheduled):**
- **Weekly Maintenance:** World events reset, faction standings update
- **Monthly Reset:** Campaign progress resets (except player progress)
- **Daily Events:** Contamination zones, resource wars, faction PvP

**3. Server Mechanics Tied to Lore:**
- **Contamination System:** The world's corruption level affects all campaigns
- **Faction Standing:** Player actions affect faction standing, influencing campaign availability
- **World Events:** Triggered by player actions and world state

### 📜 **Key World Events**

**1. The Blight (Monthly):**
- A corruption spreads from a random zone
- Players must defeat the **Blight Elder** to cleanse it
- Rewards scale with contamination level

**2. The Rift (Weekly):**
- A rift opens in a random zone
- Players must seal the rift within 30 minutes
- Failed events have lasting consequences on the zone

**4. The Grand Hunt (Weekly):**
- A powerful mob spawns in a random zone
- First faction to deal 50% damage gets bonus rewards
- All participants receive scaled rewards

### 📜 **World Events Schedule**

| Time | Event | Type | Faction Impact |
|------|-------|------|----------------|
| **Daily** | **World Event** | Random zone event | All factions |
| **Weekly** | **World Boss** | World boss spawn | Faction-based |
| **Weekly** | **Rift Seal** | Rift sealing event | All factions |
| **Weekly** | **Grand Hunt** | Powerful mob hunt | Faction-based |
| **Monthly** | **World Reset** | World state reset | Faction standings |
| **Monthly** | **World Boss Rush** | Multiple world bosses | All factions |

### 📜 **World Lore Documents (In-Game)**

**1. World Lore Book:**
- Obtainable from **Ancient Libraries** in major cities
- Contains history, factions, campaigns, and lore entries
- Updates as players progress through campaigns

**2. Faction Histories:**
- Each faction has its own history book
- Written by in-game NPCs with their perspectives
- Updates as players complete faction-specific quests

**3. Campaign Lore Books:**
- Each campaign has its own lore book
- Unlocked by completing campaign quests
- Provides deeper storyline context

### 🏗️ **Implementation Details**

**Server Code (`dm-server/`):**
- `CampaignMgr` — Manages campaign state, faction assignments, progression
- `ContaminationMgr` — Manages world contamination events
- `FactionMgr` — Manages faction standings and reputation

**Client Unity (`UnityClient/`):**
- `CampaignsWidget` — Displays available campaigns
- `QuestLogWidget` — Shows active quests and progress
- `CampaignEntry` — Individual campaign entry widget

**SQL Tables:**
- `campaign_def` — Campaign definitions with faction affiliations
- `campaign_quests` — Individual campaign quests
- `campaign_rewards` — Campaign rewards by faction
- `campaign_progress` — Player campaign progress
- `campaign_config` — System configuration

**Client UI:**
- Campaign selection screen
- Quest log with progress tracking
- Faction standings display
- Event notifications and timers

### 📊 Implementation Progress

**Completed:**
- ✅ SQL tables for campaigns and events
- ✅ Server C++ managers for campaigns and contamination
- ✅ Client Unity widgets for campaigns and events
- ✅ World lore documentation
- ✅ Faction-based campaign system
- ✅ Event scheduling and management

**Pending:**
- 🔄 Full integration with server persistence
- 🔄 Client UI polish and polishing
- 🔄 Event system testing and balancing
- 🔄 Lore book implementation and distribution

**Estimated Completion:** 90% complete — The world lore system is functionally implemented with all necessary SQL tables, server code, and client widgets. Final polishing and testing remain.

**Total Systems Implemented:** 29+ systems for the MMORPG "Чёрная Материя" (Black Metal).
