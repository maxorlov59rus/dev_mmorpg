// =============================================================================
// «Чёрная Материя» — MMOPlayerController.h/cpp (артефакт 3 промпта
// «Система классов»: сетевой роутер классовых пакетов; расширен маршрутизацией
// Spec/LFG/Quest-пакетов промпта «Квестовая система 6–10»).
//
// Подписывается на FMMONetworkClient::OnPacketReceived (через UMMOGameInstance)
// и маршрутизирует:
//   SMSG_INITIALIZE_STATS -> FMMOCharacterData::UpdateStats() (GAS AttributeSet)
//   SMSG_LEARN_SPELL      -> делегаты action bar + ClassFXManager proc-иконки
//   SMSG_CLASS_INFO       -> кэш активной спеки для FX/UI
//   SMSG_SPEC_SELECTION_OPEN -> открыть модалку (SpecSelectionWidgetClass)
//   SMSG_SPEC_CHOSEN      -> ActiveSpec + OnSpecChosen (LFGQueueWidget роль)
//   SMSG_LEARNED_SPELLS_BATCH -> OnLearnSpell x N + OnSpellsBatchLearned
//   SMSG_LFG_QUEUE_STATE / ROLE_CHECK / PROPOSAL -> делегаты LFGQueueWidget
//   SMSG_QUEST_LIST / ACCEPTED / PROGRESS -> кэш журнала UI_QuestLogWidget
// =============================================================================
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MMOProtocol.h"
#include "UI/UI_QuestLogWidget.h"
#include "MMOPlayerController.generated.h"

class UUI_CharacterCreate;
class UUI_SpecSelectionWidget;
class UClassFXManager;
struct FMMOInitializeStatsPacket;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMMOOnStatsInited, const TArray<uint8> &, RawPayload);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FMMOOnSpellLearned, uint32, SpellID, uint8, Source);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FMMOOnSpecChosenD, uint8, SpecId, uint8, LfgRoleMask);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FMMOOnSpellsBatch, const TArray<uint32> &, SpellIds, uint8, Source);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FMMOOnLfgProposalD, uint16, DungeonId, uint32, MapId,
                                              uint64, LeaderGuid, int32, TimeoutSec);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMMOOnQuestLogUpdated, const TArray<FQuestTrackerView> &, Trackers);

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

    // ---- Кэш для виджетов (UI_QuestLogWidget / UI_SpecSelectionWidget) ----

    /** UI-представление активных квестов (для журнала). */
    UFUNCTION(BlueprintPure, Category = "MMO|Quest")
    const TArray<FQuestTrackerView> &GetQuestTrackers() const { return QuestTrackers; }

    /** Whitelist спеков из последнего SMSG_SPEC_SELECTION_OPEN. */
    UFUNCTION(BlueprintPure, Category = "MMO|Spec")
    const TArray<uint8> &GetAvailableSpecIds() const { return LastSpecIds; }

    /** Отправить CMSG_CHOOSE_SPEC от имени открытой модалки (для BP-хелпера). */
    UFUNCTION(BlueprintCallable, Category = "MMO|Spec")
    void RequestChooseSpec(uint8 SpecId);

    /** Отправить CMSG_LFG_PROPOSAL_ACCEPT (кнопка «Принять/Отклонить»). */
    UFUNCTION(BlueprintCallable, Category = "MMO|LFG")
    void RespondLfgProposal(bool bAccept);

    UPROPERTY(BlueprintAssignable, Category = "MMO|Events")
    FMMOOnStatsInited OnStatsInitialized;

    UPROPERTY(BlueprintAssignable, Category = "MMO|Events")
    FMMOOnSpellLearned OnLearnSpell;

    UPROPERTY(BlueprintAssignable, Category = "MMO|Events")
    FMMOOnSpecChosenD OnSpecChosen;

    UPROPERTY(BlueprintAssignable, Category = "MMO|Events")
    FMMOOnSpellsBatch OnSpellsBatchLearned;

    UPROPERTY(BlueprintAssignable, Category = "MMO|Events")
    FMMOOnLfgProposalD OnLfgProposal;

    UPROPERTY(BlueprintAssignable, Category = "MMO|Events")
    FMMOOnQuestLogUpdated OnQuestLogUpdated;

    UPROPERTY(BlueprintReadOnly, Category = "MMO|State") uint8 ActiveClass = 0;
    UPROPERTY(BlueprintReadOnly, Category = "MMO|State") uint8 ActiveSpec = 0;
    UPROPERTY(BlueprintReadOnly, Category = "MMO|State") uint8 PowerTypeRaw = 0;

protected:
    UFUNCTION()
    void HandleServerPacket(EMMOOpcode Opcode, const FMMOPacket &Packet);

    /** Класс модалки выбора специализации (BP-дочерний UUI_SpecSelectionWidget). */
    UPROPERTY(EditAnywhere, Category = "MMO|Spec", config)
    TSubclassOf<UUI_SpecSelectionWidget> SpecSelectionWidgetClass;

private:
    /** Открыть/переоткрыть модалку спеков (роутинг SMSG_SPEC_SELECTION_OPEN). */
    void OpenSpecModal(uint8 ClassId, const TArray<uint8> &SpecIds);

    /** Пересобрать UI-трекеры из кэша состояний квестов. */
    void RebuildQuestTrackers();

    FDelegateHandle PacketHandle;

    /** Сырой кэш SMSG_QUEST_LIST + тексты из SMSG_QUEST_ACCEPTED. */
    TArray<FMMOQuestStateEntry> QuestStates;
    TMap<uint32, FMMOQuestAcceptedPacket> QuestMeta;
    TArray<FQuestTrackerView> QuestTrackers;
    TArray<uint8> LastSpecIds;              // whitelist из SMSG_SPEC_SELECTION_OPEN
    TObjectPtr<UUI_SpecSelectionWidget> ActiveSpecModal = nullptr;
};
