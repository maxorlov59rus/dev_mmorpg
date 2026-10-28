// Copyright BlackMatter Team. All Rights Reserved.
//
// FMMONetworkClient — потокобезопасный TCP-клиент кастомного бинарного протокола.
//
// Модель потоков:
//   * Game/UI thread : public API (Connect/SendMessage/TickGame). Отправка пишется
//     в MPSC-очередь OutQueue; приёмные пакеты выгружаются из InQueue в TickGame().
//   * ReadLoop thread(FFRunnable): блокирующий recv() с таймаутом, фрейминг
//     [Size|Opcode|...], декод в InQueue.
//   * WriteDrain (внутри того же потока или отдельного — здесь совмещён SendPump
//     в ReadLoop через select-style poll на write-ready не делаем: пишем напрямую
//     под lock'ом SocketSendLock; отправка из UI-треда только в очередь).
//
// Свой POSIX/BSD-сокет слой (а не Boost.Asio) выбран сознательно: ноль внешних
// нативных зависимостей на клиенте, одинаковый код Win64/Android/Linux.
// Заглушки Boost/OpenSSL подключаются в Build.cs для последующего TLS-апгрейда.

#pragma once

#include "CoreMinimal.h"
#include "HAL/Runnable.h"
#include "HAL/ThreadSafeBool.h"
#include "Containers/Queue.h"
#include "Containers/Ticker.h"
#include "MMOProtocol.h"

class FSocket;
class FSocketSubsystem;

/**
 * Runnable-поток чтения. Экземпляр создаётся владелицей через
 * TSharedPtr<FMMONetworkClient> (наружу отдавать только shared_ptr!).
 */
class FMMONetworkClient : public FRunnable, public TSharedFromThis<FMMONetworkClient> {
public:
    // ---- Delegates (вызываются на GAME THREAD внутри TickGame) ----
    DECLARE_MULTICAST_DELEGATE_TwoParams(FOnPacketReceived, EMMOOpcode /*Opcode*/,
                                         const FMMOPacket & /*Packet*/);
    DECLARE_MULTICAST_DELEGATE_TwoParams(FOnConnectionStateChanged, EMMONetConnectionState /*Prev*/,
                                         EMMONetConnectionState /*Now*/);

    FOnPacketReceived OnPacketReceived;
    FOnConnectionStateChanged OnConnectionStateChanged;

public:
    FMMONetworkClient();
    virtual ~FMMONetworkClient() override;

    /** Запрет копирования (атомарные состояния + поток). */
    FMMONetworkClient(const FMMONetworkClient &) = delete;
    FMMONetworkClient &operator=(const FMMONetworkClient &) = delete;

    /**
     * Асинхронно устанавливает TCP-соединение ServerIP:ServerPort и запускает ReadLoop.
     * @return false если уже соединён/подключаемся или endpoint невалиден.
     */
    bool ConnectToServer(const FString &ServerIP, uint16 ServerPort,
                         FTimespan ConnectTimeout = FTimespan::FromSeconds(5.0));

    /** Инициирует graceful disconnect (шлёт CmdDisconnect, затем валит транспорт). */
    void Disconnect(const FString &Reason = TEXT("User requested"));

    /**
     * ПУБЛИЧНЫЙ API ИЗ UI/GAME ТРЕДА: ставит пакет в очередь отправки.
     * Неблокирующ: MPSC-очередь + event для пробуждения send-pump.
     */
    void SendMessage(const FMMOPacket &Packet);

    /**
     * Вызывать каждый кадр из GameThread (AMMOGameInstance привязывает к ticker'у).
     * Дренажит InQueue -> OnPacketReceived, считает RTT/ping-таймеры.
     */
    void TickGame();

    // ---- Интроспекция (потокобезопасные read-only snapshot'ы) ----
    EMMONetConnectionState GetState() const {
        return static_cast<EMMONetConnectionState>(static_cast<uint8>(ConnectionState.Load()));
    }
    float GetLastRoundTripMs() const {
        return LastRttMs;
    }
    uint64 GetBytesSent() const {
        return BytesSent.load(std::memory_order_relaxed);
    }
    uint64 GetBytesReceived() const {
        return BytesReceived.load(std::memory_order_relaxed);
    }
    FTimespan GetUptime() const;

private:
    // ---- FRunnable ----
    virtual uint32 Run() override; // ReadLoop (+ send pump)
    virtual void Stop() override;  // вызывается из деструктора/Disconnect
    virtual void Exit() override {}

    // ---- internals ----
    bool OpenSocket(const FString &Host, uint16 Port, FTimespan Timeout);
    void CloseSocket();
    void SetState(EMMONetConnectionState NewState);
    bool EnqueueOutgoing(TArray<uint8> &&FrameBytes);
    int32 PumpOutgoing(); // drain OutQueue -> socket (под send-lock)
    void HandleIncomingFrame(const FMMOPacket &Pkt);
    void SchedulePingIfDue(); // game-thread side keepalive

    /** Максимум байт за один recv() — 64 KiB стек-буфер. */
    static constexpr int32 RECV_CHUNK = 64 * 1024;
    /** Ограничение глубины очереди исходящих (backpressure, защита от OOM при лаге сервера). */
    static constexpr int32 MAX_OUT_QUEUE = 256;
    static constexpr int32 MAX_IN_QUEUE = 1024;

private:
    // Транспорт
    FSocket *Socket = nullptr;
    FString RemoteHost;
    uint16 RemotePort = 0;
    FTimespan ConnectTimeoutValue;

    // Поток чтения + runable-обёртка
    FRunnableThread *ReadThread = nullptr;
    FThreadSafeBool bRunning = false;
    FThreadSafeBool bStopRequested = false;

    // Состояние соединения (atomic для cross-thread reads)
    std::atomic<uint8> ConnectionState{static_cast<uint8>(EMMONetConnectionState::Disconnected)};

    // Очереди: UI -> ReadLoop(send) ; ReadLoop -> UI(recv)
    TQueue<TArray<uint8>, EQueueMode::Mpsc> OutQueue; // сериализованные кадры
    TQueue<FMMOPacket, EQueueMode::Mpsc> InQueue;     // декодированные пакеты
    FEvent *OutReadyEvent = nullptr;                  // auto-reset: будит send-pump
    std::atomic<int32> OutQueueDepth{0};              // backpressure-счётчик

    // Send serialization: сокет может писаться только из одного потока (ReadLoop),
    // но guarded lock'ом на случай будущего write-thread.
    FCriticalSection SendLock;

    // Приёмный накопитель кадра (только ReadLoop тред — без локов)
    TArray<uint8> RecvBuffer;

    // Статы / keepalive
    std::atomic<uint64> BytesSent{0};
    std::atomic<uint64> BytesReceived{0};
    float LastRttMs = 0.f;
    double LastPingSentTime = 0.0; // FPlatformTime::Seconds
    double NextPingDueTime = 0.0;
    double ConnectStartTime = 0.0;
    static constexpr double PING_INTERVAL_SEC = 10.0;
    static constexpr double CONNECT_TIMEOUT_SEC_FALLBACK = 5.0;
};
