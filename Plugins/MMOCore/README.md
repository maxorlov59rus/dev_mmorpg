# MMOCore — сетевой клиент UE5.8 (плагин)

Runtime-плагин кастомного TCP-протокола для MMORPG «Чёрная Материя».
Соответствует `docs/03-Сеть-и-протокол.md` и `/proto/opcodes.defs`.

## Структура

```
Plugins/MMOCore/
├── MMOCore.uplugin                  # 2 модуля: MMOCore (Runtime), MMOCoreEditor (Editor)
├── Config/DefaultEngine.ini         # [/MMOCore.Network] endpoints auth/login/chat
├── Source/MMOCore/
│   ├── MMOCore.Build.cs             # Win64 / Android-arm64 ветки, ThirdParty include-пути
│   ├── Public/
│   │   ├── MMOCore.h                # FMMOCoreModule + log-категории
│   │   ├── MMOProtocol.h            # EMMOOpcode, FMMOPacket (Serialize/TryDeserialize, LE reader/writer)
│   │   ├── MMOAuthPackets.h         # FMMOServerEndpoint, FMMOAuthRequest/Response (SRP-like handshake)
│   │   ├── MMONetworkClient.h       # FMMONetworkClient : FRunnable (TSharedPtr-владение!)
│   │   └── MMOGameInstance.h        # AMMOGameInstance: Init->connect, BP API, ticker-дренаж
│   └── Private/                     # реализации
└── Source/MMOCoreThirdParty/
    ├── boost_stub/include/          # header-only заглушка Boost.Asio (реальный boost кладётся сюда же)
    └── openssl_stub/include/        # заглушка OpenSSL (TLS-апгрейд — этап M1)
```

## Модель потоков

* **Game thread** — `AMMOGameInstance::Init()` создаёт `MakeShared<FMMONetworkClient>()`,
  подписывается на делегаты, регистрирует `FTSTicker` (`OnNetTick` → `Client->TickGame()`).
* **UI/GAME → сеть**: `SendMessage(const FMMOPacket&)` сериализует кадр `[u16 Size|u16 Opcode|payload]`
  в MPSC-очередь `OutQueue` + будит `FEvent`. Никаких блокировок сокета из UI.
* **ReadLoop thread** (`FRunnable::Run`) — connect с таймаутом, цикл `HasPendingData→Recv`,
  инкрементальный frame-decode (resync по битому потоку рвёт соединение), send-pump под `SendLock`.
* **Сеть → GAME**: декодированные пакеты в `InQueue`; `TickGame()` выгружает за бюджет и
  бродкастит `OnPacketReceived`. Смена состояния из сетевого потока приходит служебным маркером
  и резолвится на game thread.

## Auth flow (реализован end-to-end на клиенте)

```
ConnectToServer(auth) -> state Connected
BeginLogin(u,p)       -> CmdAuthStart{login}
SrvAuthChallenge      -> Parse(salt,B) -> GenerateProofs() -> CmdAuthConfirm{A,M1}
SrvAuthResponse       -> status/ticket; Success -> CmdAuthSessionTicket
```

Хеш-заглушка (SHA1-цепочки вместо SRP big-number) помечена `NOTE(M0)` — заменяется
без смены wire-формата, когда dm-authserver будет готов.

## Подключение в проект

1. Скопировать папку в `<Project>/Plugins/`, включить в `.uproject`:
   `"Plugins": [ { "Name": "MMOCore", "Enabled": true } ]`
2. В `DefaultEngine.ini` проекта задать GameInstance:
   `[/Script/Engine.GameEngine]` не нужен — достаточно в BP GameMode указать
   родительский класс `AMMOGameInstance` (или `GlobalUnrealEdSettings` → DefaultGameInstance).
3. Endpoints: секция `[/MMOCore.Network]` (см. Config плагина); для Android переопределять
   через `-ini:Engine:[/MMOCore.Network]:AuthHost=...` при packaging.

## Проверка в редакторе

Консоль: `mmo.netstatus` — состояние/RTT/uptime/байты (PIE или standalone).

## Дальше (M1+)

* TLS поверх OpenSSLLWrapper/реального libssl на auth+login каналах (map остаётся UDP/KCP).
* Второй persistent-клиент для chat-сервера; migration conn'а на mapserver по `SrvWorldServerInfo`.
* Генерация `EMMOOpcode` из `/proto/opcodes.defs` (proto-gen tool) вместо ручного зеркала.
