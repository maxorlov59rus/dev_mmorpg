// =============================================================================
// MMOCore — UDialogueSubsystem.cpp (UE 5.8)
// Wire-парсинг SrvGossipShow / SrvGossipReply / SrvCreatureText
// (proto/opcodes.defs 0x2011/0x2013/0x2014, LE).
//
// Делегат FMMONetworkClient::OnPacketReceived вызывается на GAME THREAD внутри
// TickGame() — блокировки в подписчиках не нужны.
// =============================================================================
#include "Dialogue/DialogueSubsystem.h"

#include "MMONetworkClient.h"
#include "MMOProtocol.h"
#include "Engine/DataTable.h"
#include "UObject/UnrealType.h"

void UDialogueSubsystem::Initialize(FSubsystemCollectionBase &Collection) {
    Super::Initialize(Collection);
    LoadBroadcastTable();
}

void UDialogueSubsystem::Deinitialize() {
    if (Net.IsValid())
        Net->OnPacketReceived.RemoveAll(this);
    BroadcastCache.Empty();
    CurrentSession = FMMOGossipSession{};
    Super::Deinitialize();
}

void UDialogueSubsystem::BindNetwork(FMMONetworkClientPtr InClient) {
    Net = InClient; // non-owning weak: владелец — AMMOGameInstance
    if (!Net.IsValid())
        return;

    Net->OnPacketReceived.RemoveAll(this);
    Net->OnPacketReceived.AddLambda([WeakThis = TWeakObjectPtr<UDialogueSubsystem>(this)](
                                        EMMOOpcode Opcode, const FMMOPacket &Packet) {
        if (UDialogueSubsystem *Self = WeakThis.Get())
            Self->HandlePacket(Opcode, Packet);
    });
}

void UDialogueSubsystem::HandlePacket(EMMOOpcode Opcode, const FMMOPacket &Packet) {
    switch (Opcode) {
    case EMMOOpcode::SrvGossipShow:
        HandleGossipShow(Packet);
        break;
    case EMMOOpcode::SrvGossipReply:
        HandleGossipReply(Packet);
        break;
    case EMMOOpcode::SrvCreatureText:
        HandleCreatureText(Packet);
        break;
    default:
        break;
    }
}

void UDialogueSubsystem::LoadBroadcastTable() {
    // Пути фиксированы контрактом sql/upgrades/StarterContent_L1_5.sql §6
    // (экспортёр tools/dbc-export генерирует DataTable из broadcast_text/gossip_menu_option).
    BroadcastTable = Cast<UDataTable>(
        StaticFindObject(UDataTable::StaticClass(), nullptr,
                         TEXT("/Game/Data/Databases/DT_BroadcastText.DT_BroadcastText")));
    GossipOptionTable = Cast<UDataTable>(
        StaticFindObject(UDataTable::StaticClass(), nullptr,
                         TEXT("/Game/Data/Databases/DT_GossipOption.DT_GossipOption")));

    if (!BroadcastTable) {
        UE_LOG(LogTemp, Warning,
               TEXT("DialogueSubsystem: DT_BroadcastText не найден — включите pak/dbc экспорт "
                    "(docs/04 §3)"));
        return;
    }

    // Профиль строки экспорта: { int32 ID; FText Text; FName VoiceCue }
    UScriptStruct *RowStruct = BroadcastTable->GetRowStruct();
    const FIntProperty *IdP =
        RowStruct ? CastField<FIntProperty>(RowStruct->FindPropertyByName(TEXT("ID"))) : nullptr;
    const FTextProperty *TextP =
        RowStruct ? CastField<FTextProperty>(RowStruct->FindPropertyByName(TEXT("Text"))) : nullptr;
    const FNameProperty *CueP =
        RowStruct ? CastField<FNameProperty>(RowStruct->FindPropertyByName(TEXT("VoiceCue")))
                  : nullptr;
    if (!TextP) {
        UE_LOG(LogTemp, Error,
               TEXT("DialogueSubsystem: профиль DT_BroadcastText не соответствует экспортёру"));
        return;
    }

    for (const TPair<FName, uint8 *> &RowIt : BroadcastTable->GetRowMap()) {
        const int32 Id = IdP ? IdP->GetPropertyValue(RowIt.Value + IdP->GetOffset_ForInternal())
                             : FCString::Atoi(*RowIt.Key.ToString());
        FMMOBroadcastEntry Entry;
        Entry.Text = TextP->GetObjectPropertyValue(RowIt.Value + TextP->GetOffset_ForInternal());
        if (CueP)
            Entry.VoiceCue = CueP->GetValue(RowIt.Value + CueP->GetOffset_ForInternal());
        BroadcastCache.Add(Id, MoveTemp(Entry));
    }
    UE_LOG(LogTemp, Display, TEXT("DialogueSubsystem: broadcast_text кэш — %d строк"),
           BroadcastCache.Num());
}

void UDialogueSubsystem::RequestGossip(const FGuid &NpcGuid) {
    if (!Net.IsValid())
        return;
    FMMOPacket Packet;
    Packet.Opcode = MMOWire::ToWire(EMMOOpcode::CmdGossipHello);
    Packet.WriteU32(NpcGuid.A); // guid(u32) — контракт proto/opcodes.defs
    Net->SendMessage(Packet);
}

void UDialogueSubsystem::SelectOption(const FGuid &NpcGuid, uint8 OptionId) {
    if (!Net.IsValid())
        return;
    FMMOPacket Packet;
    Packet.Opcode = MMOWire::ToWire(EMMOOpcode::CmdGossipSelect);
    Packet.WriteU32(NpcGuid.A);
    Packet.WriteU8(OptionId);
    Net->SendMessage(Packet);
}

void UDialogueSubsystem::CloseGossip() {
    CurrentSession = FMMOGossipSession{};
}

FString UDialogueSubsystem::GetBroadcastText(UDialogueSubsystem *Sub, int32 TextId) {
    if (Sub && Sub->BroadcastCache.Contains(TextId))
        return Sub->BroadcastCache[TextId].Text.ToString();
    return FString::Printf(TEXT("[text:%d]"), TextId); // деградация вместо краша
}

void UDialogueSubsystem::HandleGossipShow(const FMMOPacket &Packet) {
    FMMOPacket::FReader R(Packet.Payload);
    CurrentSession = FMMOGossipSession{};

    uint32 NpcRaw = 0;
    if (!R.ReadU32(NpcRaw))
        return;
    CurrentSession.NpcGuid = FGuid(NpcRaw, NpcRaw ^ 0x0BADC0DEu, NpcRaw << 7, NpcRaw >> 3);
    uint32 MenuRaw = 0, GreetRaw = 0;
    R.ReadU32(MenuRaw);
    R.ReadU32(GreetRaw);
    CurrentSession.MenuId = static_cast<int32>(MenuRaw);
    CurrentSession.GreetingTextId = static_cast<int32>(GreetRaw);

    uint8 Count = 0;
    R.ReadU8(Count);
    for (uint8 i = 0; i < Count && i < 8; ++i) {
        FMMOGossipOption Opt;
        uint16 BcId = 0;
        uint8 HasQuest = 0;
        if (!R.ReadU8(Opt.OptionId) || !R.ReadU8(Opt.Icon))
            break;
        R.ReadU16(BcId);
        R.ReadU8(HasQuest);
        Opt.TextBroadcastId = BcId;
        Opt.bHasQuest = HasQuest != 0;

        // Локализованный текст опции: DT_GossipOption, RowName = "<MenuId>_<OptionId>"
        if (GossipOptionTable) {
            const FName RowName(
                *FString::Printf(TEXT("%d_%d"), CurrentSession.MenuId, Opt.OptionId));
            if (const uint8 *Raw = GossipOptionTable->GetRowPtr(RowName)) {
                if (const FTextProperty *Tp = CastField<FTextProperty>(
                        GossipOptionTable->GetRowStruct()->FindPropertyByName(TEXT("OptionText"))))
                    Opt.LocalizedText =
                        Tp->GetObjectPropertyValue(Raw + Tp->GetOffset_ForInternal());
            }
        }
        if (Opt.LocalizedText.IsEmpty())
            Opt.LocalizedText = FText::FromString(GetBroadcastText(this, Opt.TextBroadcastId));
        CurrentSession.Options.Add(Opt);
    }

    OnGossipShown.Broadcast(CurrentSession);
}

void UDialogueSubsystem::HandleGossipReply(const FMMOPacket &Packet) {
    FMMOPacket::FReader R(Packet.Payload);
    uint32 NpcRaw = 0, TextId = 0, PayloadId = 0;
    uint8 Action = 0;
    if (!R.ReadU32(NpcRaw) || !R.ReadU32(TextId))
        return;
    R.ReadU8(Action);
    R.ReadU32(PayloadId);

    const FGuid Npc(NpcRaw, NpcRaw ^ 0x0BADC0DEu, NpcRaw << 7, NpcRaw >> 3);
    UE_LOG(LogTemp, Display, TEXT("GOSSIP_REPLY npc=%s text=%d action=%u payload=%u"),
           *Npc.ToString(), TextId, Action, PayloadId);
    OnGossipReply.Broadcast(Npc, static_cast<int32>(Action));
}

void UDialogueSubsystem::HandleCreatureText(const FMMOPacket &Packet) {
    FMMOPacket::FReader R(Packet.Payload);
    uint32 CrRaw = 0, TextId = 0;
    uint8 Type = 0;
    if (!R.ReadU32(CrRaw) || !R.ReadU32(TextId))
        return;
    R.ReadU8(Type);

    // type: 0 say (bubble над головой), 1 yell (крупнее, без привязки к позиции),
    //       2 emote (курсив), 3 whisper-like (только целевому игроку)
    const FGuid Creature(CrRaw, CrRaw ^ 0x0BADC0DEu, CrRaw << 7, CrRaw >> 3);
    OnCreatureText.Broadcast(Creature, static_cast<int32>(TextId), Type);
}
