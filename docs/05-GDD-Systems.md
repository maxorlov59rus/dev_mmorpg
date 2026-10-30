# GDD: геймплейные системы «Чёрная Материя»

## 1. Прогрессия

- **Уровень 1–60** (классический MMO-спираль): стартовый регион фракции → общий
  континент → endgame-зоны. Темп: 60 уровень за ~40–60 ч активной игры (XP rates
  настраиваются `Rates.XpRate` per-shard).
- **Ренч/гонка уровней не цель** — ключевая валюта прогрессии после 45: **Познание
  (Insight)** — аккаунт-валюта за first-kills, seasons, collection.
- **Power system:** экипировка (item level) + **Contract** (фракционный контракт-квест
  линия даёт пассивки ветки) + **Corruption talents** (рискованная ветка силы).

## 2. Персонаж и классы

5 классов × 3 фракционные ветки (см. docs/01 §3). Атрибуты: STR/AGI/INT/VIT/SPI —
авто-распределение по классу + 5 свободных очков/level (глубина без meta-trap).
Ресурс боя: у всех **Focus** (комбо-очки 0–4) + уникальный второй ресурс ветки:
Lumen / Umbra / Splinters. Umbra тратится с побочным эффектом Corruption+.

## 3. Боевая система

- Target-based action combat (клик-таргет + skill shots для части умений) — выбор ради
  паритета тач/клавиатура (hardcore tab-target скучен на таче, full action не играбелен
  на 6" без стика).
- Глобал-КД 1.2 с; school-резисты (Holy/Shadow/Matter); позиционные бонусы (backstab/flank).
- Урон (детерминированная формула, shared/Formulas, fixed-point Q16.16):
```
raw = base_spell * (1 + stats_mod) * crit ? crit_mult : 1
mit = raw * armor_curve(target_ilvl, attacker_ilvl) * school_resist
final = clamp(mit * variance(±5% seeded), 1, …)   # seed = server tick+guid — replayable
```
- Death: resurrection window 60 с (ally res / self-res item), иначе ghost-run к кладбищу;
  durability loss + Corruption+ если смерть в Истечении.

## 4. PvE-контент

| Тип | Размер | Частота | Примечание |
|---|---|---|---|
| World events («Seepage surge») | open-zone, все | каждые 90 мин/зона | динамический спавн через worldserver Faction ticker |
| Dungeons (Normal/Hard) | 4–5 p | matchmaking | heroics daily lockout |
| Raid «Глотка» | 10 p, 8 bosses | weekly lockout per difficulty | boss-механики Lua-scripted |
| Nightmare Depths | roguelite 1–4 p | unlimited | scaling, сезонные модификаторы |
| Contracts (фракц. кампании) | solo/party | live ops | storyline эндгейма |

## 5. PvP

- **Frontline**: 3 зоны, постоянный open PvP между враждебными фракциями; capture points
  (hold = живые игроки, как TRC outdoor pvp + AV-style). Награда: season score.
- **Skirmish**: BG 10v10 (осада каравана Порога — нейтральная территория, все фракции).
- **Arena 3v3**: рейтинги solo queue/premade, trans-faction только через Порог-сервис.
- Gear: normalized stat template в PvP (как TRC Battleground item scaling) — защита от
  gear gap на launch-этапах.

## 6. Экономика

- Soft currencies: Gold (лут/крафт/квесты), Lumen/Umbra/Splinters (фракц.), Insight (account).
- **Auction House** шардовый (worldserver/Auction) — один рынок на шард; mobile-friendly UI
  (поиск/ленты уведомлений push через loginserver WS).
- Craft: 8 профессий, gathering nodes respawn (mapserver), **quality tiers** +
  invention-gated recipes (не BfA-mystery: gate = knowledge unlock, не RNG-лотерея).
- Sink mechanics: repair, contract fees, housing (M4+), cosmetics (cash shop НЕ продаёт power).
- Anti-RMT: gold caps by source/day, trade anomaly detection (Kafka consumer).

## 7. Социальные системы

Гильдии (bank, achievements, raid calendar sync через API), группы/рейды с loot
(Master/Loot council/RNG-per-player), friends/follow, **Caravan** — временный cross-faction
party-инструмент Порога (за Splinters) — легальный социальный мост между врагами.

## 8. Квест-движок (сервер)

Состояния: Available→Accepted→Objective(n)→Complete→Rewarded; chain via prerequisites;
shared quest state parties; scripting API Lua:
```lua
quest.on("accept", function(p) ... end)
quest.objective_kill("creature", 12001, 8)
quest.on("complete", function(p) p:add_faction_reputation(2, 150) p:grant_title("Шепчущий") end)
```
Хранение: `characters.quest_status` (guid, state, timer json blob).

## 9. Монетизация (F2P с косметикой)

Battle pass сезонный, cosmetic shop, service-товары (rename, faction migrate, boost).
Cross-platform purchases через loginserver (Steam wallet / Google Play billing).
Rule: никаких pay-to-win ресурсов; Corruption-ветка бесплатна (риск = цена, не деньги).

## 10. LiveOps

- Season calendar (90 дней): фичи-волны, balance patches через hotfixes (dbc overlay)
  без рестарта сервера.
- Metrics-driven tuning: retention D1/D7/D30 per faction/class/platform; economy
  inflation dashboard; frontline dominance alert (авто-buff отстающей фракции).
