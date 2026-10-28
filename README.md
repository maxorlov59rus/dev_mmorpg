# ЧЁРНАЯ МАТЕРИЯ (Dark Matter) — MMORPG, UE5.8 + TrinityCore-style сервер

Кроссплатформенная (PC + Android) фракционная MMORPG в тёмном фэнтези.
Серверное ПО — собственная реализация по образцу архитектуры **TrinityCore**
(auth/world/map/realms + MySQL), клиент — **Unreal Engine 5.8**.

## Быстрый старт для команды

1. Прочитать документы по порядку: `docs/00` → `docs/06`.
2. Поднять локальный стек БД: `mysql < sql/create/01_dm_auth.sql && ... 02 ... && ... 03 ...`
3. Протокол правится только в `proto/opcodes.defs` → кодоген (`tools/proto-gen`) —
   ручное редактирование сгенерированных файлов запрещено (CI-drift тест).

## Карта репозитория

| Путь | Содержимое |
|---|---|
| `DMClient.uproject` | **Файл проекта UE5.8** (клиент). Обновляется с КАЖДЫМ изменением структуры/плагинов/версии протокола — см. правило ниже |
| `Source/` | Target-файлы UBT (`DMClient.Target.cs`, `DMClientEditor.Target.cs`) + модули проекта |
| `Config/DefaultEngine.ini`, `Config/DefaultGame.ini` | Конфиг проекта (RHI Win64/Android, maps, GameInstance из MMOCore) |
| `Plugins/MMOCore/` | Runtime-плагин сетевого клиента (FMMONetworkClient, FMMOPacket, auth-пакеты) |
| `docs/00-Общая-концепция.md` | резюме, метрики, ADR, roadmap M0–M6 |
| `docs/01-Лор-и-фракции.md` | Чёрная Материя, 3 фракции, Corruption, Seepage, эндгейм |
| `docs/02-Сервер-TrinityCore-style.md` | процессы auth/login/world/map, модули C++, схемы БД, секвенции |
| `docs/03-Сеть-и-протокол.md` | транспорт (TCP+KCP), фрейминг, opcodes, AOI/репликация, античит |
| `docs/04-Клиент-UE5.8.md` | структура UE-проекта, пайплайн контента, Android-оптимизация |
| `docs/05-GDD-системы.md` | прогрессия, бой, PvE/PvP, экономика, LiveOps |
| `docs/06-DevOps-и-инфраструктура.md` | K8s, CI/CD, мониторинг, DR, стоимость |
| `docs/07-CI-параллельная-сборка.md` | BuildOrchestrator.py, .gitlab-ci.yml, UAT/CMake wrappers |
| `sql/create/0X_*.sql` | DDL: dm_auth / dm_characters / dm_world (аналог TRC sql/create) |
| `sql/upgrades/*.sql` | Миграции v2: DB_Schema.sql (ренегаты/gossip/item/лут), StarterQuests.sql (q_1001–q_1003), NPC_Config.sql (госсипы, мобы faction 1650, Этерниум) |
| `dm-server/src/game/Reputation/` | ReputationMgr.h/cpp — ядро репутации + state-machine «Ренегат» (repA<-5000 && repB<0) |
| `dm-server/src/game/AI/SmartAI.cpp` | Реакция NPC: агрессия стражей к изгоям на входе в город, taunt по Hated, gossip-conditions, kill-credit |
| `dm-server/scripts/lua/smartai/` | Lua-хуки репутации за убийства мобов (kill_reputation.lua) |
| `proto/opcodes.defs` | единый источник истины протокола |

## Правило: актуальность DMClient.uproject

`DMClient.uproject` — манифест клиентского проекта. Он **обязательно обновляется в том же коммите**, если изменение затронуло:

1. любой модуль в `Source/` или `Plugins/` (добавление/удаление/переименование);
2. список плагинов или их `Enabled`-состояние;
3. версию протокола (`@version` в `proto/opcodes.defs` ↔ `DMClientBuild::ProtocolVersion`);
4. целевые платформы / настройки packaging (Android ASTC/ETC2, Win64 DX12);
5. `EngineAssociation` (апгрейд патча UE 5.8.x).

CI-задача `uproject-drift` (добавляется на M0): парсит `.uproject`, сверяет модули/плагины с реальным деревом `Source/`+`Plugins/` и `@version` протокола — рассинхрон = красный билд.

## Текущее состояние

- [x] Концепт-документация v0.1 (этот набор)
- [x] Файл проекта `DMClient.uproject` (v0.1.0, protocol v1) + Targets/модули `Source/` + `Config/`
- [x] Плагин `Plugins/MMOCore` scaffold (Build.cs, NetworkClient, Protocol, AuthPackets)
- [x] CI/CD v0.1: параллельная сборка (`build/BuildOrchestrator.py`, `.gitlab-ci.yml`, `build_config.json`, wrappers BuildClient.bat / ServerBuild.sh / ServerBuild.bat) — docs/07
- [x] Фракционная система v1.2: сервер (ReputationMgr, SmartAI, Lua-хук) + SQL (схема, стартовые квесты, конфиг NPC) + новые опкоды SMSG_REP_UPDATE/RENEGADE_STATE/FACTION_VISUAL/CREATURE_AGGRO
- [x] Единый стиль C++: корневой `.clang-format` (LLVM + IndentWidth 4), весь код отформатирован; CI `lint:cpp-format` детерминирован (конфиг из репо, версия clang-format зафиксирована)
- [ ] `dm-server` scaffold (CMake + common/network) — цель M0
- [ ] `proto-gen` tool + golden packet fixtures
- [ ] Content-папка `Content/Maps/Login` (нужен хотя бы пустой уровень перед открытием в редакторе)

Замечания/решения фиксируются как новые ADR в `docs/00` §4 или отдельными файлами `docs/adr/*.md`.
