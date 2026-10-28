// Copyright BlackMatter Team. All Rights Reserved.

#include "MMOGameInstance.h"
#include "MMOCore.h"

#include "Engine/Engine.h"
#include "Containers/Ticker.h"
#include "Misc/ConfigCacheIni.h"

AMMOGameInstance::AMMOGameInstance() {
    // Дефолты — dev-loop (dm-authserver слушает 8085 по умолчанию, docs/02)
    AuthServer.Host = TEXT("127.0.0.1");
    AuthServer.Port = 8085;
    LoginServer.Host = TEXT("127.0.0.1");
    LoginServer.Port = 8091;
    ChatServer.Host = TEXT("127.0.0.1");
    ChatServer.Port = 8093;
}

void AMMOGameInstance::Init() {
    Super::Init();

    // 1. Переопределение endpoints из ini: [/MMOCore.Network] AuthHost=... AuthPort=...
    FString CfgHost;
    int32 CfgPort = 0;
    if (GConfig &&
        GConfig->GetString(TEXT("/MMOCore.Network"), TEXT("AuthHost"), CfgHost, GEngineIni)) {
        AuthServer.Host = CfgHost;
    }
    if (GConfig &&
        GConfig->GetInt(TEXT("/MMOCore.Network"), TEXT("AuthPort"), CfgPort, GEngineIni) &&
        CfgPort > 0) {
        AuthServer.Port = CfgPort;
    }
    if (GConfig) {
        if (GConfig->GetString(TEXT("/MMOCore.Network"), TEXT("LoginHost"), CfgHost, GEngineIni))
            LoginServer.Host = CfgHost;
        if (GConfig->GetInt(TEXT("/MMOCore.Network"), TEXT("LoginPort"), CfgPort, GEngineIni) &&
            CfgPort > 0)
            LoginServer.Port = CfgPort;
        if (GConfig->GetString(TEXT("/MMOCore.Network"), TEXT("ChatHost"), CfgHost, GEngineIni))
            ChatServer.Host = CfgHost;
        if (GConfig->GetInt(TEXT("/MMOCore.Network"), TEXT("ChatPort"), CfgPort, GEngineIni) &&
            CfgPort > 0)
            ChatServer.Port = CfgPort;
    }

    // 2. Создаём клиент (shared_ptr! деструктор join'ит сетевой поток)
    NetworkClient = MakeShared<FMMONetworkClient>();

    OnPktHandle = NetworkClient->OnPacketReceived.AddLambda(
        [this](EMMOOpcode Op, const FMMOPacket &P) { HandlePacket(Op, P); });
    OnStateHandle = NetworkClient->OnConnectionStateChanged.AddLambda(
        [this](EMMONetConnectionState Prev, EMMONetConnectionState Now) {
            BroadcastState(Prev, Now);
        });

    // 3. Game-thread ticker для дренажа очередей сети
    NetTickerHandle = FTSTicker::GetCoreTicker().AddTicker(
        FTickerDelegate::CreateUObject(this, &AMMOGameInstance::OnNetTick), 0.0f);

    UE_LOG(LogMMOCore, Log, TEXT("AMMOGameInstance::Init — auth=%s:%d login=%s:%d chat=%s:%d"),
           *AuthServer.Host, AuthServer.Port, *LoginServer.Host, LoginServer.Port, *ChatServer.Host,
           ChatServer.Port);

    // 4. Автоподключение к authserver при старте (отключаемо конфигом)
    bool bAutoConnect = true;
    if (GConfig) {
        GConfig->GetBool(TEXT("/MMOCore.Network"), TEXT("AutoConnect"), bAutoConnect, GEngineIni);
    }
    if (bAutoConnect && AuthServer.IsValid()) {
        ConnectTo(AuthServer.Host, AuthServer.Port);
    }
}

void AMMOGameInstance::Shutdown() {
    // Убираем ticker ДО уничтожения клиента (иначе OnNetTick дёрнет мёртвый shared_ptr)
    if (NetTickerHandle.IsValid()) {
        FTSTicker::GetCoreTicker().RemoveTicker(NetTickerHandle);
        NetTickerHandle.Reset();
    }

    if (NetworkClient.IsValid()) {
        NetworkClient->OnPacketReceived.Remove(OnPktHandle);
        NetworkClient->OnConnectionStateChanged.Remove(OnStateHandle);
        NetworkClient->Disconnect(TEXT("GameInstance shutdown"));
        // reset -> деструктор дождётся выхода ReadLoop (WaitForCompletion)
        NetworkClient.Reset();
    }

    Super::Shutdown();
}

// ============================================================================
// Blueprint API
// ============================================================================

bool AMMOGameInstance::ConnectTo(const FString &Host, int32 Port) {
    if (!NetworkClient.IsValid())
        return false;
    return NetworkClient->ConnectToServer(Host, static_cast<uint16>(Port));
}

void AMMOGameInstance::BeginLogin(const FString &Username, const FString &Password) {
    if (!NetworkClient.IsValid())
        return;

    PendingAuth = FMMOAuthRequest();
    PendingAuth.Username = Username;
    PendingAuth.PasswordLocal = Password; // в памяти, на провод не кладётся

    FMMOPacket Start;
    PendingAuth.BuildAuthStart(Start);
    NetworkClient->SendMessage(Start);
    bAuthInFlight = true;

    UE_LOG(LogMMOCore, Log, TEXT("BeginLogin: sent CmdAuthStart for '%s'"), *Username);
}

void AMMOGameInstance::DisconnectFromServer(const FString &Reason) {
    if (NetworkClient.IsValid()) {
        NetworkClient->Disconnect(Reason);
    }
    bAuthInFlight = false;
}

EMMONetConnectionState AMMOGameInstance::GetConnectionState() const {
    return NetworkClient.IsValid() ? NetworkClient->GetState()
                                   : EMMONetConnectionState::Disconnected;
}

float AMMOGameInstance::GetPingMs() const {
    return NetworkClient.IsValid() ? NetworkClient->GetLastRoundTripMs() : -1.f;
}

void AMMOGameInstance::SendPacketByOpcode(EMMOOpcode Opcode, const TArray<uint8> &Payload) {
    if (!NetworkClient.IsValid())
        return;
    FMMOPacket Pkt;
    Pkt.Opcode = static_cast<uint16>(Opcode);
    Pkt.Payload = Payload;
    NetworkClient->SendMessage(Pkt);
}

// ============================================================================
// Internals
// ============================================================================

bool AMMOGameInstance::OnNetTick(float /*DeltaTime*/) {
    if (NetworkClient.IsValid()) {
        NetworkClient->TickGame();
    }
    return true; // keep alive
}

void AMMOGameInstance::HandlePacket(EMMOOpcode Opcode, const FMMOPacket &Pkt) {
    switch (Opcode) {
    case EMMOOpcode::SrvAuthChallenge:
        HandleAuthChallenge(Pkt);
        break;
    case EMMOOpcode::SrvAuthResponse:
        HandleAuthResponse(Pkt);
        break;
    case EMMOOpcode::SrvWorldServerInfo:
        HandleWorldInfo(Pkt);
        break;
    default:
#if MMOCORE_NETWORK_DEBUG
        UE_LOG(LogMMONet, Verbose, TEXT("Unhandled opcode 0x%04X (%d bytes)"),
               static_cast<uint16>(Opcode), Pkt.Payload.Num());
#endif
        break;
    }
}

void AMMOGameInstance::HandleAuthChallenge(const FMMOPacket &Pkt) {
    if (!bAuthInFlight)
        return; // сервер прислал challenge без нашей инициативы — рвём бы
    if (!PendingAuth.ParseAuthChallenge(Pkt)) {
        UE_LOG(LogMMONet, Error, TEXT("Malformed SrvAuthChallenge"));
        OnAuthResultBP.Broadcast(EMMOAuthStatus::Failed, TEXT("Malformed auth challenge"));
        DisconnectFromServer(TEXT("protocol_error"));
        return;
    }
    PendingAuth.GenerateProofs();

    FMMOPacket Confirm;
    PendingAuth.BuildAuthConfirm(Confirm);
    NetworkClient->SendMessage(Confirm);
    UE_LOG(LogMMONet, Log, TEXT("Sent CmdAuthConfirm (proof %d bytes)"), Confirm.Payload.Num());
}

void AMMOGameInstance::HandleAuthResponse(const FMMOPacket &Pkt) {
    FMMOAuthResponse Resp;
    if (!FMMOAuthResponse::Parse(Pkt, Resp)) {
        OnAuthResultBP.Broadcast(EMMOAuthStatus::Failed, TEXT("Malformed auth response"));
        return;
    }
    bAuthInFlight = false;

    const FString Msg = Resp.IsSuccess() ? TEXT("Auth OK")
                                         : FString::Printf(TEXT("Auth failed (status=%d)"),
                                                           static_cast<int32>(Resp.Status));
    UE_LOG(LogMMOCore, Log, TEXT("%s"), *Msg);
    OnAuthResultBP.Broadcast(Resp.Status, Msg);

    // После успеха у нас есть Ticket -> переподключаемся к loginserver'у
    // (M1: тот же FMMONetworkClient, второй коннект после Disconnect;
    //  M2: вынесем в отдельный persistent-коннект на чат/map).
    if (Resp.IsSuccess() && NetworkClient.IsValid()) {
        FMMOPacket TicketPkt;
        TicketPkt.Opcode = static_cast<uint16>(EMMOOpcode::CmdAuthSessionTicket);
        TicketPkt.WriteU32(Resp.Ticket.Num());
        TicketPkt.Payload.Append(Resp.Ticket);
        NetworkClient->SendMessage(TicketPkt);
    }
}

void AMMOGameInstance::HandleWorldInfo(const FMMOPacket &Pkt) {
    // SrvWorldServerInfo: { string ip, uint16 port, uint64 worldSession }
    FMMOPacket::FReader R(Pkt.Payload);
    FString MapIp;
    uint16 MapPort = 0;
    uint64 Session = 0;
    if (R.ReadString(MapIp) && R.ReadU16(MapPort) && R.ReadU64(Session)) {
        UE_LOG(LogMMOCore, Log, TEXT("World assigned: %s:%u session=%llu"), *MapIp, MapPort,
               Session);
        // TODO(M1): разорвать auth-коннект, ConnectTo(MapIp, MapPort), затем CmdPlayerLogin
    }
}

void AMMOGameInstance::BroadcastState(EMMONetConnectionState Prev, EMMONetConnectionState Now) {
    OnConnectionChangedBP.Broadcast(Prev, Now);

    // UX-мост: потеря соединения во время auth-flow отменяет его
    if (Now == EMMONetConnectionState::Disconnected && bAuthInFlight) {
        bAuthInFlight = false;
        OnAuthResultBP.Broadcast(EMMOAuthStatus::Failed, TEXT("Connection lost during login"));
    }
}
