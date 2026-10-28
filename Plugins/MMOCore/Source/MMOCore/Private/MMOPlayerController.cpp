// =============================================================================
// «Чёрная Материя» — MMOPlayerController.cpp
// Роутинг SMSG_INITIALIZE_STATS / SMSG_LEARN_SPELL / SMSG_CLASS_INFO.
// =============================================================================
#include "MMOPlayerController.h"

#include "Classes/ClassFXManager.h"
#include "Data/ClassInfoDataTable.h"
#include "Engine/GameInstance.h"
#include "MMOGameInstance.h"
#include "MMONetworkClient.h"

DEFINE_LOG_CATEGORY_STATIC(LogPlayerCtl, Log, All);

void AMMOPlayerController::BeginPlay() {
    Super::BeginPlay();

    UMMOGameInstance *DMGI = Cast<UMMOGameInstance>(GetGameInstance());
    if (!DMGI)
        return;
    TSharedPtr<FMMONetworkClient> Client = DMGI->GetNetworkClient();
    if (Client.IsValid())
        PacketHandle = Client->OnPacketReceived.AddUObject(this, &AMMOPlayerController::HandleServerPacket);
}

void AMMOPlayerController::EndPlay(const EEndPlayReason::Type Reason) {
    if (UMMOGameInstance *DMGI = Cast<UMMOGameInstance>(GetGameInstance())) {
        if (TSharedPtr<FMMONetworkClient> Client = DMGI->GetNetworkClient())
            Client->OnPacketReceived.Remove(PacketHandle);
    }
    Super::EndPlay(Reason);
}

void AMMOPlayerController::HandleServerPacket(EMMOOpcode Opcode, const FMMOPacket &Packet) {
    switch (Opcode) {
    case EMMOOpcode::SrvInitializeStats: {
        FMMOInitializeStatsPacket Stats;
        if (FMMOInitializeStatsPacket::FromPacket(Packet, Stats)) {
            PowerTypeRaw = static_cast<uint8>(Stats.PowerType);
            UpdateStats(Stats);
            OnStatsInitialized.Broadcast(Packet.Payload); // BP-виджеты HP/mana-бара
        }
        break;
    }
    case EMMOOpcode::SrvLearnSpell: {
        FMMOLearnSpellPacket Learn;
        if (FMMOLearnSpellPacket::FromPacket(Packet, Learn))
            OnSpellLearned(Learn.SpellId, Learn.Source);
        break;
    }
    case EMMOOpcode::SrvClassInfo: {
        FMMOClassInfoPacket Info;
        if (FMMOClassInfoPacket::FromPacket(Packet, Info)) {
            ActiveClass = Info.Class;
            ActiveSpec = Info.Spec;
            UE_LOG(LogPlayerCtl, Log, TEXT("Active class=%u spec=%u"), Info.Class, Info.Spec);
        }
        break;
    }

    // ---- Quests 6-10: журнал (wire 0x2001/0x2004/0x2005) ----
    case EMMOOpcode::SrvQuestList: {
        FMMOQuestListPacket List;
        if (FMMOQuestListPacket::FromPacket(Packet, List)) {
            QuestStates = MoveTemp(List.Quests);
            RebuildQuestTrackers();
        }
        break;
    }
    case EMMOOpcode::SrvQuestAccepted: {
        FMMOQuestAcceptedPacket Accepted;
        if (FMMOQuestAcceptedPacket::FromPacket(Packet, Accepted)) {
            QuestMeta.Add(Accepted.QuestId, Accepted);
            RebuildQuestTrackers();
        }
        break;
    }
    case EMMOOpcode::SrvQuestProgress: {
        FMMOQuestProgressPacket Progress;
        if (FMMOQuestProgressPacket::FromPacket(Packet, Progress)) {
            for (FMMOQuestStateEntry &E : QuestStates) {
                if (E.QuestId == Progress.QuestId) {
                    E.Progress = Progress.Progress;
                    break;
                }
            }
            RebuildQuestTrackers();
        }
        break;
    }

    // ---- Spec selection (wire 0x2023/0x2025/0x2026) ----
    case EMMOOpcode::SrvSpecSelectionOpen: {
        FMMOSpecSelectionOpenPacket Open;
        if (FMMOSpecSelectionOpenPacket::FromPacket(Packet, Open)) {
            LastSpecIds = Open.SpecIds;
            OpenSpecModal(Open.ClassId, Open.SpecIds);
        }
        break;
    }
    case EMMOOpcode::SrvSpecChosen: {
        FMMOSpecChosenPacket Chosen;
        if (FMMOSpecChosenPacket::FromPacket(Packet, Chosen)) {
            ActiveSpec = Chosen.SpecId;
            if (IsValid(ActiveSpecModal))
                ActiveSpecModal->OnServerConfirmed(Chosen.SpecId, Chosen.LfgRoleMask);
            OnSpecChosen.Broadcast(Chosen.SpecId, Chosen.LfgRoleMask); // LFGQueueWidget роль
        }
        break;
    }
    case EMMOOpcode::SrvLearnedSpellsBatch: {
        FMMOLearnedSpellsBatchPacket Batch;
        if (FMMOLearnedSpellsBatchPacket::FromPacket(Packet, Batch)) {
            const EMMOSpellSource Src = static_cast<EMMOSpellSource>(Batch.Source);
            for (const uint32 SpellId : Batch.SpellIds)
                OnSpellLearned(SpellId, Src);
            OnSpellsBatchLearned.Broadcast(Batch.SpellIds, Batch.Source); // BP: подсветка слотов
        }
        break;
    }

    // ---- LFG (wire 0x2021/0x2022/0x2029) ----
    case EMMOOpcode::SrvLfgQueueState: {
        FMMOLfgQueueStatePacket State;
        if (FMMOLfgQueueStatePacket::FromPacket(Packet, State))
            UE_LOG(LogPlayerCtl, Log, TEXT("LFG state=%d dungeon=%u est=%ums"),
                   static_cast<int32>(State.State), State.DungeonId, State.EstWaitMs);
        // TODO(M1): dedicated delegate OnLfgQueueChanged для LFGQueueWidget.
        break;
    }
    case EMMOOpcode::SrvLfgRoleCheck: {
        FMMOLfgRoleCheckPacket Role;
        if (FMMOLfgRoleCheckPacket::FromPacket(Packet, Role))
            UE_LOG(LogPlayerCtl, Log, TEXT("LFG role check dungeon=%u req=%u ok=%u"), Role.DungeonId,
                   Role.RequiredRoles, Role.Ok);
        break;
    }
    case EMMOOpcode::SrvLfgProposal: {
        FMMOLfgProposalPacket Proposal;
        if (FMMOLfgProposalPacket::FromPacket(Packet, Proposal)) {
            // Таймаут акцепта — серверный контракт (LFG_PROPOSAL_TIMEOUT_SEC=60).
            OnLfgProposal.Broadcast(Proposal.DungeonId, Proposal.MapId, Proposal.LeaderGuid, 60);
        }
        break;
    }

    default:
        break;
    }
}

// ------------------------------------------------------------------ //
// Spec / LFG / Quest хелперы (UI_QuestLogWidget, LFGQueueWidget BP)   //
// ------------------------------------------------------------------ //

void AMMOPlayerController::OpenSpecModal(uint8 ClassId, const TArray<uint8> &SpecIds) {
    if (!SpecSelectionWidgetClass) {
        UE_LOG(LogPlayerCtl, Warning,
               TEXT("SpecSelectionWidgetClass не назначен — SMSG_SPEC_SELECTION_OPEN проигнорирован"));
        return;
    }
    if (!IsValid(ActiveSpecModal))
        ActiveSpecModal = CreateWidget<UUI_SpecSelectionWidget>(this, SpecSelectionWidgetClass);
    if (!ActiveSpecModal)
        return;
    if (!ActiveSpecModal->IsInViewport())
        ActiveSpecModal->AddToViewport(10);
    ActiveSpecModal->OpenForClass(ClassId, SpecIds);
}

void AMMOPlayerController::RequestChooseSpec(uint8 SpecId) {
    UMMOGameInstance *DMGI = Cast<UMMOGameInstance>(GetGameInstance());
    if (!DMGI)
        return;
    if (TSharedPtr<FMMONetworkClient> Client = DMGI->GetNetworkClient()) {
        FMMOChooseSpecPacket Out;
        Out.SpecId = SpecId;
        Client->SendMessage(Out.ToPacket());
    }
}

void AMMOPlayerController::RespondLfgProposal(bool bAccept) {
    UMMOGameInstance *DMGI = Cast<UMMOGameInstance>(GetGameInstance());
    if (!DMGI)
        return;
    if (TSharedPtr<FMMONetworkClient> Client = DMGI->GetNetworkClient()) {
        FMMOPacket P;
        P.Opcode = static_cast<uint16>(EMMOOpcode::CmdLfgProposalAccept);
        P.WriteU8(bAccept ? 1 : 0);
        Client->SendMessage(P);
    }
}

void AMMOPlayerController::RebuildQuestTrackers() {
    static constexpr uint32 QUEST_SPEC_SELECTION = 1104; // q_1104 «Путь избранного»

    QuestTrackers.Reset();
    for (const FMMOQuestStateEntry &E : QuestStates) {
        if (E.State != EMMOQuestState::Active && E.State != EMMOQuestState::Completable)
            continue; // в журнале только активные/готовые к сдаче

        FQuestTrackerView View;
        View.QuestId = E.QuestId;
        View.ObjectivesCompleted = E.Progress;
        View.bSpecChoiceQuest = (E.QuestId == QUEST_SPEC_SELECTION);
        if (const FMMOQuestAcceptedPacket *Meta = QuestMeta.Find(E.QuestId)) {
            View.Title = FText::FromString(Meta->Title);
            View.ObjectiveText = Meta->Objectives;
        }
        QuestTrackers.Add(View);
    }
    OnQuestLogUpdated.Broadcast(QuestTrackers);
}

void AMMOPlayerController::UpdateStats(const FMMOInitializeStatsPacket &Stats) {
    // TODO(GAS): APawn -> ACharacter -> AbilitySystemComponent -> AttributeSet
    //   BaseStrength/Agility/... (Ubase-DmCharacterAttributes). Пока сеть
    //   authoritative только для UI: пишем в кэш контроллера и бродкастим.
    UE_LOG(LogPlayerCtl, Log,
           TEXT("INITIALIZE_STATS hp=%u/%u pow=%u type=%u str=%u agi=%u sta=%u int=%u spi=%u armor=%u sp=%u"),
           Stats.HealthCur, Stats.HealthMax, Stats.PowerCur, static_cast<uint8>(Stats.PowerType), Stats.Strength,
           Stats.Agility, Stats.Stamina, Stats.Intellect, Stats.Spirit, Stats.Armor, Stats.SpellPower);
}

void AMMOPlayerController::OnSpellLearned(uint32 SpellId, EMMOSpellSource Source) {
    OnLearnSpell.Broadcast(SpellId, static_cast<uint8>(Source));
    // Иконка в action bar появляется мгновенно; proc-VFX регистрируется лениво —
    // ClassFXManager сам найдёт ключ (class,spec,moment) при первом касте.
    if (UClassFXManager *FX = UClassFXManager::Get(this))
        FX->TriggerScreenFlash(FLinearColor(0.1f, 0.9f, 1.f), 0.15f, 0.4f); // мягкая вспышка «выучено»
}
