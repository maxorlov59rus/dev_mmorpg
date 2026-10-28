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
| `docs/00-Общая-концепция.md` | резюме, метрики, ADR, roadmap M0–M6 |
| `docs/01-Лор-и-фракции.md` | Чёрная Материя, 3 фракции, Corruption, Seepage, эндгейм |
| `docs/02-Сервер-TrinityCore-style.md` | процессы auth/login/world/map, модули C++, схемы БД, секвенции |
| `docs/03-Сеть-и-протокол.md` | транспорт (TCP+KCP), фрейминг, opcodes, AOI/репликация, античит |
| `docs/04-Клиент-UE5.8.md` | структура UE-проекта, пайплайн контента, Android-оптимизация |
| `docs/05-GDD-системы.md` | прогрессия, бой, PvE/PvP, экономика, LiveOps |
| `docs/06-DevOps-и-инфраструктура.md` | K8s, CI/CD, мониторинг, DR, стоимость |
| `sql/create/0X_*.sql` | DDL: dm_auth / dm_characters / dm_world (аналог TRC sql/create) |
| `proto/opcodes.defs` | единый источник истины протокола |

## Текущее состояние

- [x] Концепт-документация v0.1 (этот набор)
- [ ] `dm-server` scaffold (CMake + common/network) — цель M0
- [ ] `proto-gen` tool + golden packet fixtures
- [ ] UE-проект `DMClient` (5.8.x pinned) — цель M0

Замечания/решения фиксируются как новые ADR в `docs/00` §4 или отдельными файлами `docs/adr/*.md`.
