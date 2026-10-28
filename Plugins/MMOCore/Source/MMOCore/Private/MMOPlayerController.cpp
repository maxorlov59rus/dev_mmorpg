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
    default:
        break;
    }
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
