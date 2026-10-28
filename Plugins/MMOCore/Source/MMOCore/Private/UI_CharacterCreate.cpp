// =============================================================================
// «Чёрная Материя» — UI_CharacterCreate.cpp
// Логика выбора класса/расы/специализации и отправки CMSG_CHAR_CREATE.
// (Артефакт 3 промпта «Система классов»; интеграция с FMMONetworkClient.)
// =============================================================================
#include "UI_CharacterCreate.h"

#include "Blueprint/GameViewportSubsystem.h"
#include "CharacterCustomization.h"
#include "Data/ClassInfoDataTable.h"
#include "Engine/DataTable.h"
#include "FactionFXManager.h"
#include "MMOGameInstance.h"
#include "MMONetworkClient.h"
#include "Components/Button.h"
#include "Components/ScrollBox.h"
#include "Engine/GameInstance.h"

DEFINE_LOG_CATEGORY_STATIC(LogCharCreate, Log, All);

// ------------------------------------------------------------------ //
// Жизненный цикл                                                     //
// ------------------------------------------------------------------ //

void UUI_CharacterCreate::NativeConstruct() {
    Super::NativeConstruct();

    if (Button_Create)
        Button_Create->OnClicked.AddDynamic(this, &UUI_CharacterCreate::OnCreateClicked);

    // Подписка на серверные пакеты (Success/Error -> снять блокировку кнопки).
    if (UGameInstance *GI = GetGameInstance()) {
        if (UMMOGameInstance *DMGI = Cast<UMMOGameInstance>(GI)) {
            if (TSharedPtr<FMMONetworkClient> Client = DMGI->GetNetworkClient()) {
                PacketHandle = Client->OnPacketReceived.AddUObject(this, &UUI_CharacterCreate::HandleServerPacket);
                // Запрос канонической таблицы доступности Race×Class с сервера:
                FMMOPacket Q;
                Q.Opcode = static_cast<uint16>(EMMOOpcode::CmdClassInfoQuery);
                Q.WriteU8(0); // for_race=0 -> полный дамп
                Client->SendMessage(Q);
            }
        }
    }

    RebuildRaceAvailability();
    RecalculateStatsPreview();
}

void UUI_CharacterCreate::NativeDestruct() {
    if (UGameInstance *GI = GetGameInstance()) {
        if (UMMOGameInstance *DMGI = Cast<UMMOGameInstance>(GI)) {
            if (TSharedPtr<FMMONetworkClient> Client = DMGI->GetNetworkClient())
                Client->OnPacketReceived.Remove(PacketHandle);
        }
    }
    Super::NativeDestruct();
}

// ------------------------------------------------------------------ //
// Выбор класса / расы / спеки                                        //
// ------------------------------------------------------------------ //

void UUI_CharacterCreate::SelectClass(uint8 InClassId) {
    SelectedClass = InClassId;

    // Если текущая раса стала недоступна для нового класса — сброс на первую допустимую.
    const FDMClassRow *Row = DMClassData::FindRowById<FDMClassRow>(ClassInfoTable, InClassId);
    if (Row && Row->AllowedRaces.Num() > 0 && !Row->AllowedRaces.Contains(SelectedRace))
        SelectedRace = Row->AllowedRaces[0];

    // Спека принадлежит классу — при смене класса берём первую из списка.
    SelectedSpec = (Row && Row->SpecIDs.Num() > 0) ? Row->SpecIDs[0] : 0;

    RebuildRaceAvailability(); // блокировка кнопок несовместимых рас
    RebuildSpecButtons();      // 3 кнопки специализаций с иконками
    RecalculateStatsPreview(); // live-блок «Базовые характеристики»
    ApplyFactionVisual();
}

void UUI_CharacterCreate::SelectRace(uint8 InRaceId) {
    // Двойная проверка на клиенте (UX) — authoritative всё равно сервер.
    if (const FDMClassRow *Row = DMClassData::FindRowById<FDMClassRow>(ClassInfoTable, SelectedClass))
        if (!Row->AllowedRaces.Contains(InRaceId)) {
            TextBlock_NameError->SetText(FText::FromString(
                FString::Printf(TEXT("Раса недоступна для этого класса (см. lore: фракционные ограничения)."))));
            return;
        }
    SelectedRace = InRaceId;
    RecalculateStatsPreview();
    ApplyFactionVisual();
}

void UUI_CharacterCreate::SelectSpec(uint8 InSpecId) {
    if (const FDMClassRow *Row = DMClassData::FindRowById<FDMClassRow>(ClassInfoTable, SelectedClass))
        if (!Row->SpecIDs.Contains(InSpecId))
            return;
    SelectedSpec = InSpecId;
    RecalculateStatsPreview(); // primary_stat спеки подсвечивается в превью
}

// ------------------------------------------------------------------ //
// Фильтрация UI                                                      //
// ------------------------------------------------------------------ //

void UUI_CharacterCreate::RebuildRaceAvailability() {
    if (!ScrollBox_RaceButtons || !RaceInfoTable)
        return;
    ScrollBox_RaceButtons->ClearChildren();

    const FDMClassRow *ClassRow = DMClassData::FindRowById<FDMClassRow>(ClassInfoTable, SelectedClass);

    for (const auto &Pair : RaceInfoTable->GetRowMap()) {
        const uint8 RaceId = static_cast<uint8>(FCString::Atoi(*Pair.Key.ToString()));
        UButton *Btn = NewObject<UButton>(this);
        Btn->SetIsEnabled(!ClassRow || ClassRow->AllowedRaces.Contains(RaceId));
        // BP-график WBP_RaceButton заполняет Label/Icon по RaceId; клик -> SelectClass/Race:
        Btn->OnClicked.AddDynamic(this, &UUI_CharacterCreate::OnCreateClicked); // placeholder binding
        UScrollBoxSlot *Slot = ScrollBox_RaceButtons->AddChild(Btn);
        Slot->SetPadding(FMargin(2.f));
    }
}

void UUI_CharacterCreate::RebuildSpecButtons() {
    if (!HorizontalBox_SpecButtons)
        return;
    HorizontalBox_SpecButtons->ClearChildren();

    const FDMClassRow *ClassRow = DMClassData::FindRowById<FDMClassRow>(ClassInfoTable, SelectedClass);
    if (!ClassRow)
        return;

    for (uint8 SpecId : ClassRow->SpecIDs) {
        UButton *Btn = NewObject<UButton>(this);
        Btn->SetBackgroundColor(SpecId == SelectedSpec ? FLinearColor(0.9f, 0.75f, 0.2f) : FLinearColor::White);
        HorizontalBox_SpecButtons->AddChild(Btn);
    }
}

// ------------------------------------------------------------------ //
// Превью статов (live, до сети)                                      //
// ------------------------------------------------------------------ //

void UUI_CharacterCreate::RecalculateStatsPreview() {
    if (!TextBlock_StatsPreview)
        return;

    TMap<EMMOBaseStat, float> Effective;
    const FDMClassRow *ClassRow = DMClassData::FindRowById<FDMClassRow>(ClassInfoTable, SelectedClass);
    if (!ClassRow) {
        TextBlock_StatsPreview->SetText(FText::FromString(TEXT("— выберите класс —")));
        return;
    }
    // Базовые значения расы читаются из DT_RaceInfo (BP-строка с TArray<float> BaseStats),
    // здесь — произведение на множители класса (эвристика превью; сеть authoritative).
    static const EMMOBaseStat Order[] = {EMMOBaseStat::Strength, EMMOBaseStat::Agility, EMMOBaseStat::Stamina,
                                         EMMOBaseStat::Intellect, EMMOBaseStat::Spirit};
    const float RaceBase[] = {14.f, 14.f, 14.f, 14.f, 14.f}; // усреднённая раса до загрузки DT
    for (int32 i = 0; i < 5; ++i) {
        const float Mult = ClassRow->StatMultipliers.Contains(Order[i]) ? ClassRow->StatMultipliers[Order[i]] : 1.f;
        Effective.Add(Order[i], RaceBase[i] * Mult);
    }
    TextBlock_StatsPreview->SetText(FText::FromString(FormatStatsBlock(Effective)));
}

FString UUI_CharacterCreate::FormatStatsBlock(const TMap<EMMOBaseStat, float> &Effective) {
    static const TMap<EMMOBaseStat, FString> Names = {
        {EMMOBaseStat::Strength, TEXT("Сила")},       {EMMOBaseStat::Agility, TEXT("Ловкость")},
        {EMMOBaseStat::Stamina, TEXT("Выносливость")}, {EMMOBaseStat::Intellect, TEXT("Интеллект")},
        {EMMOBaseStat::Spirit, TEXT("Дух")},
    };
    FString Out;
    for (const auto &KV : Effective)
        Out += FString::Printf(TEXT("%s: %.0f\n"), *Names.FindRef(KV.Key), KV.Value);
    return Out;
}

// ------------------------------------------------------------------ //
// Отправка + ответы                                                  //
// ------------------------------------------------------------------ //

bool UUI_CharacterCreate::SubmitCreation(const FString &InName, uint8 InGender, uint8 InFaction) {
    if (SelectedClass == 0 || SelectedRace == 0) {
        TextBlock_NameError->SetText(FText::FromString(TEXT("Выберите класс и расу.")));
        return false;
    }
    PendingFaction = InFaction;

    // Blob каноничен (CharacterCustomization.h): морфы из UI + прическа/цвета.
    FCharacterCustomization Custom;
    Custom.Morphs.Reset();
    for (const auto &M : MorphValues) {
        FMMMorphValue V;
        V.Slider = static_cast<EFaceSlider>(M.Key);
        V.ValueX1000 = FMath::Clamp(FMath::RoundToInt(M.Value * 1000.f), 0, 1000);
        Custom.Morphs.Add(V);
    }
    Custom.Normalize();
    CachedCustomizationBlob.Reset();
    Custom.Serialize(CachedCustomizationBlob);

    FMMOCharCreatePacket Pkt;
    Pkt.Name = InName;
    Pkt.Race = SelectedRace;
    Pkt.Class = SelectedClass;
    Pkt.Gender = InGender;
    Pkt.Faction = InFaction;
    Pkt.CustomizationBlob = CachedCustomizationBlob;

    UGameInstance *GI = GetGameInstance();
    UMMOGameInstance *DMGI = Cast<UMMOGameInstance>(GI);
    if (!DMGI)
        return false;
    TSharedPtr<FMMONetworkClient> Client = DMGI->GetNetworkClient();
    if (!Client)
        return false;

    Button_Create->SetIsEnabled(false); // дубль-защита до ответа сервера
    return Client->SendMessage(Pkt.ToPacket());
}

void UUI_CharacterCreate::OnCreateClicked() {
    // BP переопределяет точное имя поля; default-путь читает Text_Field_Name из BP.
    // Здесь — no-op: реальный вызов идёт через SubmitCreation из графика.
}

void UUI_CharacterCreate::HandleServerPacket(EMMOOpcode Opcode, const FMMOPacket &Packet) {
    switch (Opcode) {
    case EMMOOpcode::SrvCharCreateSuccess: {
        FMMOCharCreateSuccessPacket Success;
        if (FMMOCharCreateSuccessPacket::FromPacket(Packet, Success)) {
            UE_LOG(LogCharCreate, Log, TEXT("Char created: guid=%u name=%s class=%u spec=%u map=%u"),
                   Success.CharacterId, *Success.Name, Success.Class, SelectedSpec, Success.StartMap);
            // preview_customization байт-в-байт == отправленный blob -> список рендерит без БД.
            ApplyFactionVisual();
            if (Button_Create)
                Button_Create->SetIsEnabled(true);
            // TODO(M1): broadcast OnCharacterCreated delegate -> экран выбора персонажа.
        }
        break;
    }
    case EMMOOpcode::SrvCharCreateError: {
        FMMOCharCreateErrorPacket Err;
        if (FMMOCharCreateErrorPacket::FromPacket(Packet, Err)) {
            static const TMap<EMMOCharCreateError, FString> Msgs = {
                {EMMOCharCreateError::NameInvalid, TEXT("Имя содержит недопустимые символы.")},
                {EMMOCharCreateError::NameExists, TEXT("Имя занято.")},
                {EMMOCharCreateError::RaceClassMismatch, TEXT("Класс недоступен выбранной расе.")},
                {EMMOCharCreateError::FactionInvalid, TEXT("Фракция несовместима с классом/расой.")},
                {EMMOCharCreateError::CustomizationBlob, TEXT("Ошибка кастомизации — сбросьте слайдеры.")},
            };
            TextBlock_NameError->SetText(FText::FromString(Msgs.FindRef(Err.Reason)));
            if (Button_Create)
                Button_Create->SetIsEnabled(true);
        }
        break;
    }
    case EMMOOpcode::SrvClassInfo: {
        // Серверный дамп доступности — сверяем с DataTable, расхождение = warning.
        FMMOClassInfoPacket Info;
        if (FMMOClassInfoPacket::FromPacket(Packet, Info)) {
            if (const FDMClassRow *Row = DMClassData::FindRowById<FDMClassRow>(ClassInfoTable, Info.Class)) {
                bool bMatch = true;
                for (uint8 R : Info.AllowedRaces)
                    if (!Row->AllowedRaces.Contains(R))
                        bMatch = false;
                if (!bMatch)
                    UE_LOG(LogCharCreate, Warning,
                           TEXT("DT_ClassInfo(class %u) разошёлся с сервером (class_restrictions) — пересоберите dbc."),
                           Info.Class);
            }
        }
        break;
    }
    default:
        break;
    }
}

// ------------------------------------------------------------------ //
// Визуал фракции на превью-модели                                    //
// ------------------------------------------------------------------ //

void UUI_CharacterCreate::ApplyFactionVisual() {
    if (!PreviewMesh || PendingFaction == 0)
        return;
    if (UGameInstance *GI = GetGameInstance()) {
        if (UFactionFXManager *FX = GI->GetSubsystem<UFactionFXManager>()) {
            const float Corruption = FX->GetCorruption();
            FX->SetFactionOnMesh(PreviewMesh, PendingFaction, Corruption);
        }
    }
}
