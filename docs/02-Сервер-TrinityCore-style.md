# Серверная архитектура «Чёрная Материя» — аналог TrinityCore

## 0. Соответствие ролей TrinityCore → DM-стек

| TrinityCore | Наш процесс | Обязанности |
|---|---|---|
| `authserver` | **dm-authserver** | SRP6(a/6h) аутентификация, токены сессий, hardware/фиче-баны, раздача списка шардов |
| `login server (realms)` | **dm-loginserver** | HTTP/WS API: character list/create/delete, выбор шарда, биллинг-шлюз, матчмейкинг |
| `worldserver` | **dm-worldserver** | Глобальное состояние шарда: игрок↔session, группы/рейды/гильдии, каналы, аукцион, квест-движок, рассылка world-state, скрипты Lua/C++ |
| `maps (внутри worldserver)` | **dm-mapserver × N** | Тик карты: движение, AOI,combat-расчёты, спавн, рейд-инстансы. Один процесс = группа карт |
| `DB: auth / characters / world / hotfixes` | MySQL 8 кластер + Redis | Те же 4 логические БД + кэш/очереди |
| `dbc (client dbcs)` | **dm-contentd** | Раздача статического контента клиенту (manifest + CDN), валидация версий |
| `HookScript / Eluna LuaEngine` | **dm-script-vm** | LuaJIT-машина внутри world/map для хот-патчей геймплея |

Философия сохранена из TRC: **long-running процессы, общая MySQL, минимум RPC между
процессами** (TRC использует один process на шард; мы выносим mapserver'ы отдельно —
это эволюция модели «one worldserver per realm» до масштаба mobile MMO).

## 1. Топология продакшн-кластера (один шард)

```
                        [Internet]
                            │
              ┌─────────────┼──────────────────┐
              │             │                  │
        dm-loginserver  dm-authserver      CDN (assets)
        (HTTP :8443)    (TCP :8091 SRP)         
              │             │ token           
              └──────┬──────┘                   
                     ▼                          
               dm-worldserver (TCP :8092)      
        session mgmt, guilds, raids, quests,   
        mail, AH, world state, script VM       
                     │ spawn/assign             
        ┌────────────┼────────────┐            
        ▼            ▼            ▼            
   dm-mapserver dm-mapserver dm-mapserver  …   (TCP :8093+, по 1..8 карт)
        (open world)(instances pool)(frontline) 
                     │                            
     MySQL(1 primary + 2 replica) · Redis(Sentinel) · Kafka(логи/аналитика)
```

Правило маршрутизации игрока: authserver выдаёт `realm_token`; loginserver выбирает
шард (по региону/фракции/наполнению); worldserver принимает connection и назначает
mapserver (`map_registry` в Redis: `map:{id} -> host:port`). Клиент держит **два**
сокета: world (команды/UI-состояние) и map (непрерывный поток состояний) — как в TRC
клиент имел один сокет worldserver, но у нас разделение для load-balancing.

## 2. Модульная структура репозитория (C++20, CMake)

```
dm-server/
├── CMakeLists.txt
├── dep/                      # сторонние: mysql-connector-c++, rabbitmq? no → kafka client
│   ├── srp6/                 # SRP6a реализация (порт TrinityCore <AuthServer>/SRP6.cpp)
│   ├── protobuf-lite? NO     # свой бинарный формат (см. docs/03)
│   └── lua-jit/
├── src/
│   ├── common/               # логирование (spdlog), конфиг (toml11), байт-порядок,
│   │   ├── Network/          # epoll/io_context обёртка, TCP+UDP(KCP), TLS (mbedtls)
│   │   ├── Database/         # пул коннектов MySQL, prepared stmt cache, миграктор
│   │   ├── Scripting/        # LuaVM wrapper (per-map state, sandboxed)
│   │   └── Hotfix/           # наложение патчей таблиц (аналог hotfixes DB TRC)
│   ├── authserver/
│   ├── loginserver/
│   ├── worldserver/
│   │   ├── Sessions/         # WorldSession (аналог TRC): очередь пакетов, права GM
│   │   ├── Guilds/ Raids/ Groups/
│   │   ├── Quests/           # движок квестов (state machine per player)
│   │   ├── Auction/ Mail/ Chat/ Channels/
│   │   ├── Faction/          # frontline control, seasons, seepage scheduler
│   │   └── Scripts/          # C++ scripted* hooks + Lua registry
│   ├── mapserver/
│   │   ├── Maps/             # Map/Cell/Zone (grid like TRC GridRefManager)
│   │   ├── Movement/         # navmesh recast-detour, валидация движения
│   │   ├── Combat/           # auras, threat, damage pipeline
│   │   ├── AI/               # creature AI (SM + BT), bosses scripts
│   │   ├── Instances/        # LifeCycle: create/destroy, difficulty, lock (TRS-style)
│   │   └── Replication/      # AOI, delta-encoding, send queues
│   └── shared/
│       ├── Protocol/         # генератор из proto/*.defs → C++/Lua/C# structs
│       ├── DataStores/       # dbc-менеджеры: Item.dbc, Spell.dbc, Creature.dbc…
│       └── Formulas/         # общие боевые формулы (клиент тоже их зеркалит!)
├── sql/
│   ├── create/               # fresh install DDL (dm_auth, dm_characters, dm_world)
│   ├── upgrades/             # incremental (как TRC /sql/upgrades)
│   └── dbc_import/           # импорт авторских CSV → runtime .dbc
├── etc/                      # *.toml примеры конфигов (аналог TRC *.conf.dist)
└── tools/
    ├── dm-config-loader      # dump dbc для клиента (manifest)
    ├── mapextractor          # конвертация UE5 levels → collision/navmesh blob
    └── hotfix-packager
```

## 3. Ключевые подсистемы и их TRC-аналоги

### 3.1 Сессии и пакеты (WorldSession)
Как в TRC: каждый сокет → `WorldSession` с очередью входящих пакетов, обрабатываемых
в main loop worldserver'а (однопоточная логика шарда → нет гонок в UI-слое).
Отличие: тяжёлые операции (запись в БД, хеш паролей) уходят в thread pool
(`AsyncService`, он же в TRC есть).

### 3.2 Maps & Cells
TRC: `Map` → `Grids(256x256)` → cells. Мы сохраняем сетку, но:
- open-world карты режем на **стриминг-секторы 128×128 м**, совпадающие с UE World
  Partition cells (чтобы клиентский streaming и серверный AOI использовали одну сетку);
- instance-карты — лёгкие (без стриминга), живут ≤ 2 ч после wipe, потом unload;
- перенос игрока между mapserver'ами: `TransferRequest` → worldserver сериализует
  snapshot (позиция, баны, касты) → новый mapserver `EnterWorldAck`. Окно black screen
  ≤ 1.5 c (телепорт-эффект маскирует).

### 3.3 Скриптинг
- C++ virtual hooks: `OnLogin`, `OnCreatureKill`, `OnSpellHit`, `OnZoneEvent` —
  компилируются в map/world (как TRC `CustomScript` + HookMgr AIO).
- Lua (LuaJIT) слой: все квесты, диалоги, boss-механики v1 пишутся на Lua и грузятся
  hot-reload'ом (`lua_reload 'quest_123'`). Sandbox: без io/os, лимит инструкций
  (hook на debug line count) — защита от бесконечных циклов в tick.

### 3.4 Боевая pipeline (mapserver)
```
CastRequest → validate (GCD, mana, LOS, range, faction) → spell effects queue
→ apply aura(s) → threat update → damage calc (shared/Formulas, детерминирован fixed-point)
→ death check → lootable creation → combat log packet (AOI-filtered)
```
Все расчёты в **fixed-point int32/int64** (Q16.16) — требование кроссплатформенной
детерминизации предсказаний клиента (UE float не совпадёт bit-to-bit на ARM/x86).

### 3.5 Фракционная система (worldserver/Faction/)
- Таблицы: `faction_def`, `frontline_zone`, `frontline_point`, `frontline_control`,
  `season`, `season_results`, `seepage_state`.
- Тикер раз в 60 с: пересчёт контроля точек (стоимость удержания = живые игроки фракции
  на точке), триггер осады, планировщик Seepage (weighted random по «греху» зоны:
  сумма PvP-убийств/смертей за неделю).
- Итоги сезона: cron-событие → запись `season_results`, выдача наград через почтовый
  сервис (Mail subsystem), сброс frontline.

### 3.6 ГМ-инструменты
REST API loginserver + привилегированные команды worldserver (уровни 1–4 как в TRC:
`Player/GM/Admin/Console`). Веб-панель (React) читает те же эндпоинты: баны, поиск
персонажа, live-статистика шарда (Redis info + metrics).

## 4. База данных — схемы (DDL в /sql/create)

Три базы (четвёртая hotfixes — поверх world):

- **dm_auth**: accounts, account_password (verifier/salt SRP), access_levels, bans
  (account/ip/hardware), uptime, auto_ban_rules, session_tokens.
- **dm_characters**: characters, char_stats, char_skills, char_spells, char_auras,
  char_inventory (container/item json-free нормализация), char_equipment, corruption,
  guilds, guild_members, guild_ranks, groups, raids, raid_resets, quest_status,
  reputation, mail, cooldowns, currencies (Lumen/Umbra/Splinters), titles, settings.
- **dm_world**: creature_respawn, gameobject_respawn, area_trigger_state, weather,
  world_state, frontline_*, seepage_*, auction_items (+bid), instance_locks,
  dynamic_events_log, season_results.
- **dm_hotfixes**: patches (checksum, payload) — накладываются поверх dbc в рантайме.

Redis используется для: presence (`online:{acc}`), map routing, rate limits, chat
channel fanout, matchmaker queues. Kafka — только события аналитики/античита
(player_death, gold_gain, item_create…), никогда игровой логики.

## 5. Конфигурация процессов (пример `worldserver.toml`)

```toml
[Network]
World.BindToAddr = "0.0.0.0"
World.Port = 8092
MapsPool.MinIdle = 4

[Login]
AuthHost = "auth.internal:8091"
TokenSecretEnv = "DM_TOKEN_SECRET"

[Sessions]
TickTimeMs = 20
MaxPacketPerTick = 512

[Rates]
XpRate = 1.0
CorpRate = 1.0        # скорость Порчи
SeepageIntervalMin = 90

[Faction]
SeasonLengthDays = 90
FrontlineCaptureHoldSec = 300

[Database]
MySQLMode = "async"
Characters = { host="db-primary", port=3306, db="dm_characters", pool=24 }
World      = { host="db-primary", port=3306, db="dm_world", pool=32 }
```

## 6. Жизненный цикл игрока (sequence)

```
Client(PC/Android)
  │ 1. HTTPS POST /auth (email/pass) ─────────────► loginserver
  │ 2. SRP challenge/response ─────────────────────► authserver (:8091)
  │ 3. realm_ticket (JWT-like, TTL 60s) ◄─────────── authserver
  │ 4. GET /realms?ticket ─────────────────────────► loginserver → список шардов
  │ 5. TCP connect world :8092 + ticket ───────────► worldserver (валидирует с auth)
  │ 6. SMSG_AUTH_OK {char list} ◄────────────────── worldserver
  │ 7. CMSG_CHAR_SELECT ► worldserver → выбирает mapserver (Redis routing)
  │ 8. SMSG_MAP_ASSIGN {host,port,map_session_key} ◄┘
  │ 9. UDP/KCP connect mapserver :8093+n ─────────► mapserver
  │ 10. SMSG_ENTER_WORLD {snapshot, AOI init} ◄──── mapserver
  │ 11. тики: CMSG_MOVE_* ► / SMSG_DELTA ◄ (20 Гц), события через world socket
```

Kick/disconnect: worldserver получает heartbeat-loss от mapserver → save+logout flow
(аналог TRC `HandleLogoutRequest`), персонаж сохраняется в течение 10 мин («offline
jail» для reconnect без полной загрузки).

## 7. Сохранение состояния и отказы

- Каждые 5 мин — bulk-save dirty-объектов (флаги как TRC `SaveToDB`), критичные
  события (лот, смерть, трейд) — write-through в ту же транзакцию.
- mapserver crash: worldserver видит потерю heartbeat → помечает карты down, игроков —
  «reconnecting», после рестарта процесса карта загружает last-saved snapshot;
  потери ≤ 5 мин рутинного состояния (допускается, как в TRC-эпохе).
- worldserver crash: редирект клиентов через loginserver на тот же шард (stateless
  auth), персонажи reload'ятся с БД; инстансы переживают за счёт `instance_locks`
  и снапшотов боссов в dm_world.
