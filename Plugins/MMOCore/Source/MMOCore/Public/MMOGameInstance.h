// Copyright BlackMatter Team. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MMOProtocol.h"
#include "MMOAuthPackets.h"
#include "MMONetworkClient.h"
#include "MMOGameInstance.generated.h"

class UWorld;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FMMOOnConnectionChanged, EMMONetConnectionState, Prev,
                                             EMMONetConnectionState, Now);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FMMOOnAuthResult, EMMOAuthStatus, Status,
                                             const FString &, Message);

/**
 * Игровой инстанс «Чёрной Материи».
 *
 * Жизненный цикл сети:
 *   Init() -> читаем endpoints из [/MMOCore.Network] в DefaultEngine.ini
 *          -> создаём TSharedPtr<FMMONetworkClient>
 *          -> ConnectToServer(AuthHost, AuthPort)  (authserver TRC-style)
 *   Tick(ticker) -> Client->TickGame(); маршрутизация Srv* пакетов по хендлерам
 *   OnAuthResponse(Success) -> шлём CmdAuthSessionTicket -> loginserver ticket
 *   Shutdown -> Disconnect + сброс shared_ptr (деструктор join'ит поток).
 */
UCLASS(BlueprintType, Blueprintable)
class AMMOGameInstance : public UGameInstance {
    GENERATED_BODY()

public:
    AMMOGameInstance();

    // --- UObject ---
    virtual void Init() override;
    virtual void Shutdown() override;

    // --- Blueprint API ---
    UFUNCTION(BlueprintCallable, Category = "MMO|Net", DisplayName = "Connect to MMO Server")
    bool ConnectTo(const FString &Host, int32 Port);

    UFUNCTION(BlueprintCallable, Category = "MMO|Net")
    void BeginLogin(const FString &Username, const FString &Password);

    UFUNCTION(BlueprintCallable, Category = "MMO|Net")
    void DisconnectFromServer(const FString &Reason);

    UFUNCTION(BlueprintPure, Category = "MMO|Net")
    EMMONetConnectionState GetConnectionState() const;

    UFUNCTION(BlueprintPure, Category = "MMO|Net")
    float GetPingMs() const;

    /** Отправка произвольного пакета из BP (через helper-структуру payload). */
    UFUNCTION(BlueprintCallable, Category = "MMO|Net")
    void SendPacketByOpcode(EMMOOpcode Opcode, const TArray<uint8> &Payload);

    UPROPERTY(BlueprintAssignable, Category = "MMO|Events")
    FMMOOnConnectionChanged OnConnectionChangedBP;

    UPROPERTY(BlueprintAssignable, Category = "MMO|Events")
    FMMOOnAuthResult OnAuthResultBP;

    /** Доступ к клиенту для C++-систем (AuthFlow, ChatBridge...). Null-safe. */
    TSharedPtr<FMMONetworkClient> GetNetworkClient() const {
        return NetworkClient;
    }

protected:
    // Endpoints из конфига (DefaultEngine.ini): см. Config/DefaultEngine.ini плагина
    UPROPERTY(Config, EditAnywhere, Category = "MMO|Endpoints")
    FMMOServerEndpoint AuthServer;

    UPROPERTY(Config, EditAnywhere, Category = "MMO|Endpoints")
    FMMOServerEndpoint LoginServer;

    UPROPERTY(Config, EditAnywhere, Category = "MMO|Endpoints")
    FMMOServerEndpoint ChatServer;

private:
    /** Тикер game-thread: выгружает входящие пакеты и гоняет keepalive. */
    bool OnNetTick(float DeltaTime);

    // ---- packet handlers (game thread) ----
    void HandlePacket(EMMOOpcode Opcode, const FMMOPacket &Pkt);
    void HandleAuthChallenge(const FMMOPacket &Pkt);
    void HandleAuthResponse(const FMMOPacket &Pkt);
    void HandleWorldInfo(const FMMOPacket &Pkt); // переход на mapserver (следующий коннект)

    void BroadcastState(EMMONetConnectionState Prev, EMMONetConnectionState Now);

private:
    TSharedPtr<FMMONetworkClient> NetworkClient;
    FTSTicker::FDelegateHandle NetTickerHandle;

    // Текущий auth-flow (живёт между Challenge и Confirm)
    FMMOAuthRequest PendingAuth;
    bool bAuthInFlight = false;

    // Храны нативных делегатов, подписанных на NetworkClient (для RemoveAll в Shutdown)
    FDelegateHandle OnPktHandle;
    FDelegateHandle OnStateHandle;
};
