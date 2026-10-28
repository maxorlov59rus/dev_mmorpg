// =============================================================================
// «Чёрная Материя» — MMOPlayerController.h/cpp (артефакт 3 промпта
// «Система классов»: сетевой роутер классовых пакетов).
//
// Подписывается на FMMONetworkClient::OnPacketReceived (через UMMOGameInstance)
// и маршрутизирует:
//   SMSG_INITIALIZE_STATS -> FMMOCharacterData::UpdateStats() (GAS AttributeSet)
//   SMSG_LEARN_SPELL      -> делегаты action bar + ClassFXManager proc-иконки
//   SMSG_CLASS_INFO       -> кэш активной спеки для FX/UI
// =============================================================================
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MMOProtocol.h"
#include "MMOPlayerController.generated.h"

class UUI_CharacterCreate;
class UClassFXManager;
struct FMMOInitializeStatsPacket;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMMOOnStatsInited, const TArray<uint8> &, RawPayload);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FMMOOnSpellLearned, uint32, SpellID, uint8, Source);

UCLASS(Blueprintable, Config = MMOCore)
class MMOCORE_API AMMOPlayerController : public APlayerController {
    GENERATED_BODY()

public:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type Reason) override;

    /** Вызывается из HandlePacket(SMSG_INITIALIZE_STATS): базовые статы перса. */
    UFUNCTION(BlueprintCallable, Category = "MMO|Class")
    void UpdateStats(const FMMOInitializeStatsPacket &Stats);

    /** Вызывается из HandlePacket(SMSG_LEARN_SPELL). */
    UFUNCTION(BlueprintCallable, Category = "MMO|Class")
    void OnSpellLearned(uint32 SpellId, EMMOSpellSource Source);

    UPROPERTY(BlueprintAssignable, Category = "MMO|Events")
    FMMOOnStatsInited OnStatsInitialized;

    UPROPERTY(BlueprintAssignable, Category = "MMO|Events")
    FMMOOnSpellLearned OnLearnSpell;

    UPROPERTY(BlueprintReadOnly, Category = "MMO|State") uint8 ActiveClass = 0;
    UPROPERTY(BlueprintReadOnly, Category = "MMO|State") uint8 ActiveSpec = 0;
    UPROPERTY(BlueprintReadOnly, Category = "MMO|State") uint8 PowerTypeRaw = 0;

protected:
    UFUNCTION()
    void HandleServerPacket(EMMOOpcode Opcode, const FMMOPacket &Packet);

private:
    FDelegateHandle PacketHandle;
};
