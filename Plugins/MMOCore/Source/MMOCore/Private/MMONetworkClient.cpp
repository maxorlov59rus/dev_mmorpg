// Copyright BlackMatter Team. All Rights Reserved.

#include "MMONetworkClient.h"
#include "MMOCore.h"
#include "MMOAuthPackets.h"

#include "HAL/Event.h"
#include "HAL/PlatformProcess.h"
#include "HAL/UnrealMemory.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "IPAddress.h"
#include "Misc/Timespan.h"
#include "Modules/ModuleManager.h"

// ============================================================================
// Construction / Destruction
// ============================================================================

FMMONetworkClient::FMMONetworkClient() {
    OutReadyEvent = FPlatformProcess::GetSynchEventFromPool(false /*auto-reset*/);
    RecvBuffer.Reserve(RECV_CHUNK * 2);
}

FMMONetworkClient::~FMMONetworkClient() {
    Stop();
    if (ReadThread) {
        ReadThread->WaitForCompletion();
        delete ReadThread;
        ReadThread = nullptr;
    }
    CloseSocket();
    if (OutReadyEvent) {
        FPlatformProcess::ReturnSynchEventToPool(OutReadyEvent);
        OutReadyEvent = nullptr;
    }
}

FTimespan FMMONetworkClient::GetUptime() const {
    const EMMONetConnectionState S = GetState();
    if ((S == EMMONetConnectionState::Connected || S == EMMONetConnectionState::Authenticated) &&
        ConnectStartTime > 0.0) {
        return FTimespan::FromSeconds(FPlatformTime::Seconds() - ConnectStartTime);
    }
    return FTimespan::Zero();
}

// ============================================================================
// Public API — Game/UI thread
// ============================================================================

bool FMMONetworkClient::ConnectToServer(const FString &ServerIP, uint16 ServerPort,
                                        FTimespan ConnectTimeout) {
    const EMMONetConnectionState Cur = GetState();
    if (Cur == EMMONetConnectionState::Connecting || Cur == EMMONetConnectionState::Connected ||
        Cur == EMMONetConnectionState::Authenticating ||
        Cur == EMMONetConnectionState::Authenticated) {
        UE_LOG(LogMMONet, Warning, TEXT("ConnectToServer ignored: already in state %d"),
               static_cast<int32>(Cur));
        return false;
    }
    if (ServerIP.IsEmpty() || ServerPort == 0) {
        UE_LOG(LogMMONet, Error, TEXT("Invalid endpoint %s:%u"), *ServerIP, ServerPort);
        return false;
    }

    RemoteHost = ServerIP;
    RemotePort = ServerPort;
    ConnectTimeoutValue = ConnectTimeout;
    bStopRequested = false;

    SetState(EMMONetConnectionState::Connecting);

    // Собственный поток: connect() может блокировать до таймаута — на game thread нельзя.
    ReadThread = FRunnableThread::Create(
        this, *FString::Printf(TEXT("MMONetRead_%s:%d"), *ServerIP, ServerPort), 0, TPri_Normal);
    if (!ReadThread) {
        UE_LOG(LogMMONet, Error, TEXT("Failed to spawn network read thread"));
        SetState(EMMONetConnectionState::Disconnected);
        return false;
    }
    return true;
}

void FMMONetworkClient::Disconnect(const FString &Reason) {
    UE_LOG(LogMMONet, Log, TEXT("Disconnect requested: %s"), *Reason);
    FMMOPacket Bye;
    Bye.Opcode = static_cast<uint16>(EMMOOpcode::CmdDisconnect);
    Bye.WriteString(Reason);
    SendMessage(Bye); // best-effort: встанет в очередь, pump отправит если ещё живо

    bStopRequested = true;
    SetState(EMMONetConnectionState::Failing);
    if (OutReadyEvent)
        OutReadyEvent->Trigger();
    // Поток сам завершится и выставит Disconnected в Exit-фазе Run().
    // Деструктор гарантированно join'ит.
}

void FMMONetworkClient::SendMessage(const FMMOPacket &Packet) {
    TArray<uint8> Frame;
    if (!Packet.Serialize(Frame)) {
        UE_LOG(LogMMONet, Error,
               TEXT("SendMessage: packet too big (%d bytes payload, opcode 0x%04X)"),
               Packet.Payload.Num(), Packet.Opcode);
        return;
    }
    const int32 CurDepth = OutQueueDepth.Increment();
    if (CurDepth > MAX_OUT_QUEUE) {
        OutQueueDepth.Decrement(); // дропнули кадр до enqueue — откатываем счётчик
        UE_LOG(LogMMONet, Warning, TEXT("OutQueue overflow (backpressure) — dropped opcode 0x%04X"),
               Packet.Opcode);
        return;
    }
    if (!OutQueue.Enqueue(MoveTemp(Frame))) {
        OutQueueDepth.Decrement(); // в очередь не попал — тоже откат
        UE_LOG(LogMMONet, Error, TEXT("OutQueue.Enqueue failed"));
        return;
    }
    if (OutReadyEvent)
        OutReadyEvent->Trigger(); // разбудить send-pump в сетевом потоке
}

void FMMONetworkClient::TickGame() {
    check(IsInGameThread()); // делегаты должны резолвиться на game thread

    // 1. Выгрузить принятые пакеты (ограничение за кадр — не зависнем на шторме)
    int32 Budget = MAX_IN_QUEUE;
    FMMOPacket Pkt;
    while (Budget-- > 0 && InQueue.Dequeue(Pkt)) {
        HandleIncomingFrame(Pkt);
        OnPacketReceived.Broadcast(MMOWire::FromWire(Pkt.Opcode), Pkt);
    }

    // 2. Keepalive ping по таймеру
    SchedulePingIfDue();
}

// ============================================================================
// Internals — queues & state
// ============================================================================

void FMMONetworkClient::SetState(EMMONetConnectionState NewState) {
    const EMMONetConnectionState Prev =
        static_cast<EMMONetConnectionState>(ConnectionState.exchange(static_cast<uint8>(NewState)));
    if (Prev != NewState) {
        UE_LOG(LogMMONet, Log, TEXT("State: %d -> %d"), static_cast<int32>(Prev),
               static_cast<int32>(NewState));
        // ВНИМАНИЕ: SetState может вызываться из сетевого потока (Run). Делегат
        // рассылаем только если мы на game thread; иначе состояние прочитается в TickGame.
        if (IsInGameThread()) {
            OnConnectionStateChanged.Broadcast(Prev, NewState);
        } else {
            // отложенный broadcast: кинем событие в InQueue служебным пакетом
            FMMOPacket Marker;
            Marker.Opcode = 0x7FFF; // internal marker: state changed
            Marker.WriteU16(static_cast<uint16>(Prev));
            Marker.WriteU16(static_cast<uint16>(NewState));
            InQueue.Enqueue(MoveTemp(Marker));
        }
    }
}

bool FMMONetworkClient::EnqueueOutgoing(TArray<uint8> &&FrameBytes) {
    // служебный путь (state-marker и т.п.) — без backpressure-счётчика UI
    return OutQueue.Enqueue(MoveTemp(FrameBytes));
}

int32 FMMONetworkClient::PumpOutgoing() {
    int32 Sent = 0;
    TArray<uint8> Frame;
    FScopeLock Lock(&SendLock);
    if (!Socket || !Socket->IsValid())
        return 0;

    while (OutQueue.Dequeue(Frame)) {
        OutQueueDepth.Decrement();
        int32 Offset = 0;
        bool bOk = true;
        while (Offset < Frame.Num()) {
            int32 NumSent = 0;
            bOk = Socket->Send(Frame.GetData() + Offset, Frame.Num() - Offset, NumSent);
            if (!bOk || NumSent <= 0)
                break;
            Offset += NumSent;
        }
        if (bOk && Offset == Frame.Num()) {
            BytesSent.fetch_add(static_cast<uint64>(Offset), std::memory_order_relaxed);
            ++Sent;
        } else {
            UE_LOG(LogMMONet, Warning, TEXT("Send failed mid-frame (%d/%d) — link down?"), Offset,
                   Frame.Num());
            return -1; // сигнал потоку валить соединение
        }
    }
    return Sent;
}

// ============================================================================
// FRunnable — ReadLoop (+ send pump), сетевой поток
// ============================================================================

bool FMMONetworkClient::OpenSocket(const FString &Host, uint16 Port, FTimespan Timeout) {
    check(!Socket); // вызывается только из нашего сетевого потока

    ISocketSubsystem *SS = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
    if (!SS) {
        UE_LOG(LogMMONet, Error, TEXT("No socket subsystem on this platform"));
        return false;
    }

    // Разрешение адреса: сырой IP или DNS-имя (Android/iOS резолвят внутри UE-подсистемы)
    TSharedPtr<FInternetAddr> Addr;
    bool bResolved = false;
    Addr = SS->GetAddressFromURL(*FString::Printf(TEXT("%s:%u"), *Host, Port), bResolved);
    if (!bResolved || !Addr.IsValid() || !Addr->IsValid()) {
        Addr = SS->GetAddressFromString(Host);
        if (!Addr.IsValid()) {
            UE_LOG(LogMMONet, Error, TEXT("Cannot resolve host '%s'"), *Host);
            return false;
        }
        Addr->SetPort(Port);
    }

    Socket = SS->CreateSocket(NAME_Stream, TEXT("MMOCoreTcp"), false /*blocking*/);
    if (!Socket) {
        UE_LOG(LogMMONet, Error, TEXT("CreateSocket failed"));
        return false;
    }

    // Буферы сокета: 256 KiB recv / 128 KiB send — типично для MMO-клиента
    uint32 Opt = 256 * 1024;
    Socket->SetRecvSize(Opt);
    Opt = 128 * 1024;
    Socket->SetSendSize(Opt);
    Socket->SetNoDelay(true); // latency важнее трафика на заголовках
    Socket->SetReuseAddress(TEXT("MMOCoreClient"));

    const double Deadline = FPlatformTime::Seconds() + Timeout.GetTotalSeconds();
    EAcceptConnection AcceptResult = Socket->Connect(*Addr);
    while (AcceptResult == EAcceptConnection::Pending) {
        if (bStopRequested)
            return false;
        if (FPlatformTime::Seconds() > Deadline) {
            UE_LOG(LogMMONet, Warning, TEXT("TCP connect timeout to %s:%u"), *Host, Port);
            return false;
        }
        FPlatformProcess::Sleep(0.01f);
        // неблокирующий connect: проверяем завершение через getsockopt(SO_ERROR) внутри
        // HasPendingData? UE не даёт "poll for connect", поэтому повторяем Connect — он вернёт
        // Accepted/Rejected когда сокет доразрешится (реализация-зависимо, но стабильно на
        // Win/BSD/Linux).
        AcceptResult = Socket->Connect(*Addr);
    }
    if (AcceptResult != EAcceptConnection::Accepted) {
        UE_LOG(LogMMONet, Error, TEXT("TCP connect refused by %s:%u (err=%d)"), *Host, Port,
               static_cast<int32>(SS->GetLastErrorCode()));
        return false;
    }

    ConnectStartTime = FPlatformTime::Seconds();
    UE_LOG(LogMMONet, Log, TEXT("TCP connected to %s:%u"), *Host, Port);
    return true;
}

void FMMONetworkClient::CloseSocket() {
    if (Socket) {
        ISocketSubsystem *SS = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
        Socket->Close();
        if (SS)
            SS->DestroySocket(Socket);
        Socket = nullptr;
    }
}

uint32 FMMONetworkClient::Run() {
    bRunning = true;

    // ---- Phase 1: connect ----
    if (!OpenSocket(RemoteHost, RemotePort, ConnectTimeoutValue)) {
        CloseSocket();
        SetState(EMMONetConnectionState::Disconnected);
        bRunning = false;
        return 1;
    }
    SetState(EMMONetConnectionState::Connected);

    // ---- Phase 2: read/write loop ----
    RecvBuffer.Reset();
    TArray<uint8> RecvChunk;
    RecvChunk.SetNumUninitialized(RECV_CHUNK);

    while (!bStopRequested) {
        // A) drain outgoing queue
        const int32 PumpRes = PumpOutgoing();
        if (PumpRes < 0)
            break; // транспорт сдох при записи

        // B) read with 50ms slice timeout
        if (Socket->HasPendingData(RecvChunk.Num() /*hint*/)) {
            int32 Read = 0;
            const bool bGot =
                Socket->Recv(RecvChunk.GetData(), RecvChunk.Num(), Read, ESocketReceiveFlags::None);
            if (!bGot || Read <= 0) {
                UE_LOG(LogMMONet, Log, TEXT("recv() signalled close/error on %s:%u"), *RemoteHost,
                       RemotePort);
                break;
            }
            BytesReceived.fetch_add(static_cast<uint64>(Read), std::memory_order_relaxed);
            RecvBuffer.Append(RecvChunk.GetData(), Read);

            // C) frame decode: сколько полных кадров — столько декодируем
            int32 Consumed = 0;
            while (true) {
                FMMOPacket Pkt;
                if (!FMMOPacket::TryDeserialize(RecvBuffer.GetData(), RecvBuffer.Num(), Pkt,
                                                Consumed)) {
                    // различаем "недокадр" vs "битый поток": если первый size-заголовок
                    // уже распарсился но тела нет — ждём; если FrameSize вне лимита — рвём.
                    if (RecvBuffer.Num() >= 4) {
                        const uint32 Fs = static_cast<uint32>(RecvBuffer[0]) |
                                          (static_cast<uint32>(RecvBuffer[1]) << 8);
                        if (Fs < MMOCORE_HEADER_SIZE || Fs > MMOCORE_MAX_PACKET_SIZE) {
                            UE_LOG(LogMMONet, Error,
                                   TEXT("Corrupt stream (frame size %u) — dropping connection"),
                                   Fs);
                            bStopRequested = true;
                        }
                    }
                    break;
                }
                RecvBuffer.RemoveAt(0, Consumed, EAllowShrinking::No);
                InQueue.Enqueue(MoveTemp(Pkt));
            }
        } else {
            // нет данных: короткий сон вместо busy-wait; просыпаемся на event
            if (OutReadyEvent) {
                OutReadyEvent->Wait(FTimespan::FromMilliseconds(50));
            } else {
                FPlatformProcess::Sleep(0.05f);
            }
        }
    }

    // ---- Phase 3: teardown ----
    CloseSocket();
    SetState(EMMONetConnectionState::Disconnected);
    bRunning = false;
    return 0;
}

void FMMONetworkClient::Stop() {
    bStopRequested = true;
    if (OutReadyEvent)
        OutReadyEvent->Trigger();
    if (Socket)
        Socket->AsyncShutdown(); // разблокирует HasPendingData/Recv если завис
}

void FMMONetworkClient::HandleIncomingFrame(const FMMOPacket &Pkt) {
    switch (static_cast<EMMOOpcode>(Pkt.Opcode)) {
    case EMMOOpcode::SrvPong: {
        // payload: echo u64 timestamp seconds*1000
        FMMOPacket::FReader R(Pkt.Payload);
        uint64 EchoMs;
        if (R.ReadU64(EchoMs)) {
            const uint64 NowMs = static_cast<uint64>(FPlatformTime::Seconds() * 1000.0);
            LastRttMs = static_cast<float>(NowMs - EchoMs);
        }
    } break;
    case EMMOOpcode::SrvAuthResponse:
        if (GetState() == EMMONetConnectionState::Authenticating) {
            FMMOAuthResponse Resp;
            if (FMMOAuthResponse::Parse(Pkt, Resp) && Resp.IsSuccess()) {
                SetState(EMMONetConnectionState::Authenticated);
            }
        }
        break;
    default:
        break;
    }
}

void FMMONetworkClient::SchedulePingIfDue() {
    const EMMONetConnectionState S = GetState();
    if (S != EMMONetConnectionState::Connected && S != EMMONetConnectionState::Authenticated)
        return;

    const double Now = FPlatformTime::Seconds();
    if (NextPingDueTime == 0.0) {
        NextPingDueTime = Now + PING_INTERVAL_SEC;
        return;
    }
    if (Now < NextPingDueTime)
        return;

    FMMOPacket Ping;
    Ping.Opcode = static_cast<uint16>(EMMOOpcode::CmdPing);
    Ping.WriteU64(static_cast<uint64>(Now * 1000.0)); // эхо-метка для RTT
    SendMessage(Ping);
    LastPingSentTime = Now;
    NextPingDueTime = Now + PING_INTERVAL_SEC;
}
