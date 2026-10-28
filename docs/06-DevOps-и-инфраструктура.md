# DevOps и инфраструктура «Чёрная Материя»

## 1. Среда

- Облако: гибридная модель — выделенные bare-metal для map/world (стабильный single-thread
  perf важен для tick loop) + cloud (AWS/GCP) для login/auth/CDN/auto-scaling инстансов.
- Регионы присутствия launch: EU-West, US-East, SEA (Android-heavy рынок). Каждый регион =
  набор шардов; data residency для аккаунтов (GDPR).

## 2. Контейнеризация и оркестрация

| Компонент | Упаковка | Оркестрация |
|---|---|---|
| dm-authserver, dm-loginserver | Docker | K8s (deployment, HPA по RPS) |
| dm-worldserver | Docker, stateful set (per-shard affinity) | K8s StatefulSet: `world-{shard_id}` |
| dm-mapserver | Docker | K8s Deployment per shard pool + custom scheduler (по locality карт) |
| MySQL | Percona operator / managed RDS | primary + replicas, backups PITR |
| Redis | Sentinel | 1 master + 2 replica на регион |
| Kafka | Strimzi/MSK | analytics + anti-cheat events |
| CDN assets | S3 + CloudFront/Cloudflare | manifest.json versioned |

Конфиги процессов (`*.toml`) — из ConfigMap + secrets (Vault); миграции БД запускаются
Job'ом перед rolling update (см. §4).

## 3. CI/CD

```
GitHub Actions / GitLab CI:
  server:   cmake build (gcc13/clang17, asan+ubsan job) → unit tests → integration
            (docker-compose: mysql+redis+all procs, boot smoke test: SRP login→enter world)
  client:   UE Commandlet builds (Windows, Android APK/AAB) nightly; cook on GPU runners;
            automated perf gate: frame-time regression > 5% fail MR
  proto:    codegen drift check (golden packets) — общий job для обоих репо
Release:
  server: canary shard (staging realm) → 1 prod shard → all (Argo Rollouts style manual)
  client: staged rollout Play Console 10%→50%→100%; PC: Steam beta branch first
Hotfix:   dbc-overlay + Lua patch pipeline (minutes, no restart) — отдельный канал LiveOps
```

## 4. Версионирование БД (как TRC sql/upgrades)

- Каталог `sql/upgrades/revNNNN_*.sql`, таблица `db_version` (схема как в TRC:
  world/characters/auth versions + required trans).
- Запрет: worldserver/mapserver версии ≠ schema → abort startup (fail-fast).
- Zero-downtime политика: additive migrations обязательны; breaking — через dual-write
  step releases (expand → migrate → contract).

## 5. Наблюдаемость

- Metrics: Prometheus + Grafana. Ключевые SLI:
  `world_tick_lag_ms` (p99 < 8), `map_delta_send_rate`, `auth_latency_p95`,
  `player_pop_per_map`, `frontline_faction_ratio`, `db_query_p99`.
- Tracing: OpenTelemetry между login→world→map (session id сквозной).
- Логи: Loki/ELK, structured spdlog JSON; core dumps → Sentry symbols.
- Игрок-facing статус: status page (шарды, latency по регионам) от loginserver API.

## 6. Автомасштабирование игрового процесса

- Пороги: players/mapserver > capacity_soft → worldserver останавливает выдачу новых
  transfer'ов на карту; queue/wait или **auto-fission**: клон карты (phasing instance)
  для новичков до конца сессии? Нет — вместо этого перемещение игроков на parallel
  channel-версию зоны (DCU-style layering): `zone_layers` таблица, routing через
  `SMSG_MAP_ASSIGN`.
- Instance pool: mapserver'ы instances создаются под demand матчмейкера (queue depth),
  убиваются после lockout idle 2 ч.

## 7. Backup / DR

- MySQL: WAL/backups hourly + PITR binlog; restore drill ежемесячно (game day sim).
- Игрок-данные критичны: double-write journal для транзакций трейда/AH (Kafka compact topic)
  как страховка последнего часа.
- RTO: auth/login < 15 мин; шард world+maps < 30 мин (redeploy + DB reload);
  клиентский CDN — мультирегион, RPO ~0 (stateless).

## 8. Безопасность инфраструктуры

- Сегментация: game VLAN без ingress кроме 8091–8093/8443; admin SSH через bastion + audit.
- DDoS: L3/4 scrubbing (Cloudflare Magic Transit/GameShield), rate limit auth (SRP
  expensive op — proof-of-work опционально при атаке).
- Секреты: Vault, ротация токенов подписи realm_ticket каждые 24 ч.
- Пентест перед OBT; bug bounty program.

## 9. Стоимость (порядок, M5 beta)

Штатный расчёт на 1 шард × 10k CCU: 2× world-class VM (32 vCPU), 6× map VM (16 vCPU),
MySQL r6g.2xlarge + replicas, Redis, Kafka 3 broker, CDN 20 ТБ/мес → ~$6–9k/мес/регион.
Цель юнит-экономики: ≤ $0.60 MAU/мес к лаунчу (за счёт консолидации шардов).
