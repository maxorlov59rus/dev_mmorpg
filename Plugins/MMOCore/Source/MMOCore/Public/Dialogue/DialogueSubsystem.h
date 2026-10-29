// =============================================================================
// MMOCore — UDialogueSubsystem (клиент, UE 5.8)
// Приём SMSG_GOSSIP_SHOW / SMSG_GOSSIP_REPLY / SMSG_CREATURE_TEXT и
// SMSG_QUEST_*; хранение broadcast_text из DT_BroadcastText (DataTable <-
// sql/upgrades/StarterContent_L1_5.sql §6, экспорт в pak).
//
// ВАЖНО: список опций приходит с сервера УЖЕ отфильтрованным по dm_conditions.
// Клиент не знает правил скрытия веток (is_renegade/rep_rank_min...) — это
// server-authoritative (docs/03 §7), поэтому никакой локальной фильтрации тут нет.
// =============================================================================
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "UObject/ObjectPtr.h"
#include "DialogueSubsystem.generated.h"

class UMMONetworkClient;
class UTexture2D;
using FMMONetworkClientPtr = TSharedPtr<class FMMONetworkClient>;

/** Одна видимая опция gossip-меню (wire: GossipOption, opcode 0x2011). */
USTRUCT(BlueprintType)
struct FMMOGossipOption {
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    uint8 OptionId = 0;

    /** 0 talk | 1 quest! | 2 quest? | 3 vendor | 4 banker | 6 trainer | 7 rep */
    UPROPERTY(BlueprintReadOnly)
    uint8 Icon = 0;

    /** Ссылка на broadcast_text (подпись/озвучка реплики). */
    UPROPERTY(BlueprintReadOnly)
    int32 TextBroadcastId = 0;

    /** 1 = у NPC есть квест для игрока (!/? вместо talked). */
    UPROPERTY(BlueprintReadOnly)
    bool bHasQuest = false;

    /** Локализованный текст (из DT_GossipOption по MenuId+OptionId). */
    UPROPERTY(BlueprintReadOnly)
    FText LocalizedText;
};

/** Открытый диалог с NPC. */
USTRUCT(BlueprintType)
struct FMMOGossipSession {
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    FGuid NpcGuid;

    UPROPERTY(BlueprintReadOnly)
    int32 MenuId = 0;

    UPROPERTY(BlueprintReadOnly)
    int32 GreetingTextId = 0;

    UPROPERTY(BlueprintReadOnly)
    TArray<FMMOGossipOption> Options;
};

/** Кэш строки dialogue-локализации (broadcast_text). */
USTRUCT(BlueprintType)
struct FMMOBroadcastEntry {
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    FText Text;

    UPROPERTY(BlueprintReadOnly)
    FName VoiceCue = NAME_None; // MetaSound/akEvent id — опционально
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGossipShown, const FMMOGossipSession &, Session);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGossipReply, FGuid, NpcGuid, int32, Action);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnCreatureText, FGuid, CreatureGuid, int32, TextId,
                                               uint8, SayType);

UCLASS()
class MMOCORE_API UDialogueSubsystem : public UGameInstanceSubsystem {
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase &Collection) override;
    virtual void Deinitialize() override;

    /** Инициализация после установки world-соединения MMONetworkClient. */
    void BindNetwork(FMMONetworkClientPtr InClient);

    /** Отправить CMSG_GOSSIP_HELLO (по тапу/наведению на NPC). */
    UFUNCTION(BlueprintCallable, Category = "MMO|Dialogue")
    void RequestGossip(const FGuid &NpcGuid);

    /** Выбрать ветку -> CMSG_GOSSIP_SELECT. Сервер повторно проверит условие. */
    UFUNCTION(BlueprintCallable, Category = "MMO|Dialogue")
    void SelectOption(const FGuid &NpcGuid, uint8 OptionId);

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MMO|Dialogue")
    static FString GetBroadcastText(UDialogueSubsystem *Sub, int32 TextId);

    UFUNCTION(BlueprintCallable, Category = "MMO|Dialogue")
    void CloseGossip();

    UPROPERTY(BlueprintAssignable, Category = "MMO|Dialogue")
    FOnGossipShown OnGossipShown;

    UPROPERTY(BlueprintAssignable, Category = "MMO|Dialogue")
    FOnGossipReply OnGossipReply; // UI открывает vendor/trainer/quest-log по Action

    UPROPERTY(BlueprintAssignable, Category = "MMO|Dialogue")
    FOnCreatureText OnCreatureText; // надписи-«мыльные пузыри» над мобами lvl 1-5

private:
    void HandlePacket(EMMOOpcode Opcode, const FMMOPacket &Packet);
    void HandleGossipShow(const FMMOPacket &Packet);
    void HandleGossipReply(const FMMOPacket &Packet);
    void HandleCreatureText(const FMMOPacket &Packet);
    void LoadBroadcastTable();

    /** non-owning ссылка: владелец — AMMOGameInstance (TSharedPtr<FMMONetworkClient>). */
    TWeakPtr<FMMONetworkClient> Net;

    TMap<int32, FMMOBroadcastEntry> BroadcastCache; // text_id -> строка (DT_BroadcastText)
    FMMOGossipSession CurrentSession;

    UPROPERTY(Transient)
    TObjectPtr<UDataTable> BroadcastTable; // /Game/Data/Databases/DT_BroadcastText

    UPROPERTY(Transient)
    TObjectPtr<UDataTable> GossipOptionTable; // /Game/Data/Databases/DT_GossipOption
};
