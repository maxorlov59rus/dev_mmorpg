// =============================================================================
// «Чёрная Материя» — UI_SpecSelectionWidget.cpp (артефакт 3 промпта
// «Квестовая система 6–10: Инстансы и Специализации»).
//
// Модальное окно выбора специализации на 10 уровне.
//   * OpenForClass(ClassId, SpecIds) — фильтр DT_SpecPanel по классу + whitelist
//     SpecID из пакета SMSG_SPEC_SELECTION_OPEN (сервер — источник истины).
//   * SelectSpecPanel(Index) — подсветка карточки + превью экипировки
//     (щит+меч для танка, двуручник/парные клинки для ДД).
//   * ConfirmSelection() — CMSG_CHOOSE_SPEC (wire 0x2024), анти-спам блок 1с,
//     кнопка неактивна до выбора панели (требование ТЗ).
//   * OnServerConfirmed() — ответ SMSG_SPEC_CHOSEN: золотая подсветка новых
//     способностей action bar на 10 секунд, закрытие модалки.
//
// Сетевой ввод исходит из AMMOPlayerController::HandleServerPacket (wire
// парсеры — MMOProtocol.h); виджет не читает сокеты сам.
// =============================================================================
#include "UI_SpecSelectionWidget.h"

#include "Blueprint/GameViewportSubsystem.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/HorizontalBox.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Data/ClassInfoDataTable.h"
#include "Engine/DataTable.h"
#include "Engine/GameInstance.h"
#include "MMOGameInstance.h"
#include "MMONetworkClient.h"
#include "MMOPlayerController.h"

DEFINE_LOG_CATEGORY_STATIC(LogSpecSelect, Log, All);

namespace {
/** Время жизни золотой подсветки новых способностей (ТЗ: 10 секунд). */
constexpr float GOLD_GLOW_SECONDS = 10.f;
/** Анти-спам двойного клика «Подтвердить». */
constexpr float CONFIRM_BLOCK_SECONDS = 1.f;
} // namespace

// ------------------------------------------------------------------ //
// Жизненный цикл                                                     //
// ------------------------------------------------------------------ //

void UUI_SpecSelectionWidget::NativeConstruct() {
    Super::NativeConstruct();

    if (ConfirmButton) {
        ConfirmButton->OnClicked.AddDynamic(this, &UUI_SpecSelectionWidget::ConfirmSelection);
        ConfirmButton->SetIsEnabled(false); // активна только после выбора панели
    }
    SetConfirmEnabled(false);

    // Модалка: захват UI-ввода (gamepad-safe, без camera lock в мире).
    if (APlayerController *PC = GetOwningPlayer()) {
        FInputModeGameAndUI InputMode;
        InputMode.SetWidgetToFocus(GetCachedWidget());
        InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
        PC->SetInputMode(InputMode);
        PC->bShowMouseCursor = true;
    }
}

void UUI_SpecSelectionWidget::NativeTick(const FGeometry &MyGeometry, float InDeltaTime) {
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (ConfirmBlockUntil > 0.f)
        ConfirmBlockUntil -= InDeltaTime;

    if (GoldGlowRemaining > 0.f) {
        GoldGlowRemaining -= InDeltaTime;
        const float Alpha = FMath::Clamp(GoldGlowRemaining / GOLD_GLOW_SECONDS, 0.f, 1.f);
        // Плавное затухание свечения: BP-анимация GlowCurve или материал с
        // параметром GlowAmount (контракт для художников: MF_MMO_GoldGlow).
        if (GlowOverlay)
            GlowOverlay->SetOpacity(Alpha);
        if (GoldGlowRemaining <= 0.f && GlowOverlay)
            GlowOverlay->SetVisibility(ESlateVisibility::Collapsed);
    }
}

// ------------------------------------------------------------------ //
// Открытие / построение панелей                                      //
// ------------------------------------------------------------------ //

void UUI_SpecSelectionWidget::OpenForClass(uint8 ClassId, const TArray<uint8> &SpecIds) {
    ActiveClassId = ClassId;
    Panels.Reset();
    SelectedIndex = INDEX_NONE;
    bAwaitingServer = false;

    if (!SpecPanelTable) {
        UE_LOG(LogSpecSelect, Warning, TEXT("SpecPanelTable (DT_SpecPanel) не назначена на виджете"));
        return;
    }

    // Whitelist спеков приходит от сервера (class_specs.spec_id через
    // playercreateinfo_specs) — DataTable даёт только визуальную часть.
    for (const uint8 SpecId : SpecIds) {
        const FDFSpecPanelRow *Row = DMClassData::FindRowById<FDFSpecPanelRow>(SpecPanelTable, SpecId);
        if (!Row || Row->ClassID != ClassId)
            continue;

        FMMOSpecPanelView View;
        View.SpecId = SpecId;
        View.DisplayName = Row->DisplayName;
        View.BigDescription = FText::FromString(Row->Description);
        View.Icon = Row->Icon;
        View.AbilitySpellIds = Row->StarterAbilities; // 3 способности спеки
        View.WeaponMesh = Row->WeaponMesh;
        View.OffhandMesh = Row->OffhandMesh;
        // Превью роли: PrimaryStat 1 (Сила) трактуем как «танк-путь» (Воин),
        // остальные — ДД. Точная роль придёт в SMSG_SPEC_CHOSEN.lfg_role_mask.
        View.bTankPreview = (Row->PrimaryStat == 1);
        Panels.Add(View);
    }

    if (HeaderText)
        HeaderText->SetText(FText::Format(LOCTEXT("Header", "Путь избранного — {0}"),
                                          FText::AsNumber(Panels.Num())));

    RebuildPanels();
    SetConfirmEnabled(false);
}

void UUI_SpecSelectionWidget::RebuildPanels() {
    if (!PanelsRow)
        return;
    PanelsRow->ClearChildren();

    // Карточки строятся в Widget Blueprint: BP создаёт дочерние W_SpecCard
    // через OnBuildSpecCards (BP-реализация делегата) либо мы добавляем
    // заглушки-контейнеры, заполняемые дизайнерами. Чистый C++ слот здесь —
    // чтобы виджет не ломался до готовности BP.
    for (int32 i = 0; i < Panels.Num(); ++i) {
        UBorder *Slot = NewObject<UBorder>(this);
        Slot->SetVisibility(ESlateVisibility::Visible);
        PanelsRow->AddChild(Slot);

        if (UTextBlock *Label = NewObject<UTextBlock>(this)) {
            Label->SetText(Panels[i].DisplayName);
            Slot->SetContent(Label);
        }
    }
}

// ------------------------------------------------------------------ //
// Выбор панели + превью экипировки                                   //
// ------------------------------------------------------------------ //

void UUI_SpecSelectionWidget::SelectSpecPanel(int32 PanelIndex) {
    if (!Panels.IsValidIndex(PanelIndex) || bAwaitingServer)
        return;

    SelectedIndex = PanelIndex;
    UpdateEquipmentPreview(Panels[PanelIndex]);
    SetConfirmEnabled(true);

    UE_LOG(LogSpecSelect, Log, TEXT("Выбрана панель %d (SpecId=%u, tank=%d)"), PanelIndex,
           Panels[PanelIndex].SpecId, Panels[PanelIndex].bTankPreview ? 1 : 0);
}

void UUI_SpecSelectionWidget::UpdateEquipmentPreview(const FMMOSpecPanelView &View) {
    // Контракт BP: силуэт персонажа на фоне окна меняет оружие по выбранной
    // спеке. Реализация — через BlueprintImplementableEvent, т.к. mesh-компы
    // превью живут в BP-сцене (PreviewWeaponRoot / PreviewOffhandRoot).
    K2_OnPreviewEquipChanged(View.bTankPreview, View.WeaponMesh, View.OffhandMesh);

    if (PreviewSilhouette) {
        // Визуальная метка роли на силуэте (иконка щита/клинка поверх фона).
        PreviewSilhouette->SetBrushFromSoftObjectPath(
            View.bTankPreview ? FSoftObjectPath(TEXT("/Game/UI/Icons/T_RoleTank.T_RoleTank"))
                              : FSoftObjectPath(TEXT("/Game/UI/Icons/T_RoleDPS.T_RoleDPS")));
    }
}

// ------------------------------------------------------------------ //
// Подтверждение -> сеть                                              //
// ------------------------------------------------------------------ //

void UUI_SpecSelectionWidget::ConfirmSelection() {
    if (SelectedIndex == INDEX_NONE || bAwaitingServer)
        return;
    if (GetWorld()->GetTimeSeconds() < ConfirmBlockUntil)
        return; // анти-спам

    UMMOGameInstance *DMGI = Cast<UMMOGameInstance>(GetGameInstance());
    if (!DMGI)
        return;
    TSharedPtr<FMMONetworkClient> Client = DMGI->GetNetworkClient();
    if (!Client.IsValid()) {
        UE_LOG(LogSpecSelect, Warning, TEXT("Нет сетевого клиента — выбор спеки не отправлен"));
        return;
    }

    FMMOChooseSpecPacket Out;
    Out.SpecId = Panels[SelectedIndex].SpecId;
    Client->SendMessage(Out.ToPacket()); // wire 0x2024, UI-поток безопасен

    bAwaitingServer = true;
    ConfirmBlockUntil = GetWorld()->GetTimeSeconds() + CONFIRM_BLOCK_SECONDS;
    SetConfirmEnabled(false);
    if (ConfirmLabel)
        ConfirmLabel->SetText(LOCTEXT("Waiting", "Ожидание сервера…"));
}

void UUI_SpecSelectionWidget::SetConfirmEnabled(bool bEnabled) {
    if (ConfirmButton)
        ConfirmButton->SetIsEnabled(bEnabled && !bAwaitingServer);
}

// ------------------------------------------------------------------ //
// Ответ сервера                                                      //
// ------------------------------------------------------------------ //

void UUI_SpecSelectionWidget::OnServerConfirmed(uint8 SpecId, uint8 LfgRoleMask) {
    bAwaitingServer = false;

    int32 ConfirmedIdx = INDEX_NONE;
    for (int32 i = 0; i < Panels.Num(); ++i) {
        if (Panels[i].SpecId == SpecId) {
            ConfirmedIdx = i;
            break;
        }
    }
    if (ConfirmedIdx == INDEX_NONE) {
        UE_LOG(LogSpecSelect, Warning, TEXT("SMSG_SPEC_CHOSEN: неизвестный SpecId=%u"), SpecId);
        SetConfirmEnabled(true);
        return;
    }

    SelectedIndex = ConfirmedIdx;
    TriggerGoldGlow(Panels[ConfirmedIdx].AbilitySpellIds);
    OnSpecConfirmed.Broadcast(SpecId, true);

    if (AMMOPlayerController *PC = Cast<AMMOPlayerController>(GetOwningPlayer()))
        PC->ActiveSpec = SpecId; // кэш для FX/UI (ClassFXManager читает его)

    // Закрытие модалки: возврат контроля игроку.
    if (APlayerController *RawPC = GetOwningPlayer()) {
        RawPC->SetInputMode(FInputModeGameOnly());
        RawPC->bShowMouseCursor = false;
    }
    RemoveFromParent();
}

void UUI_SpecSelectionWidget::TriggerGoldGlow(const TArray<uint32> &NewSpellIds) {
    // Action bar подписан на AMMOPlayerController::OnLearnSpell — controller
    // роутит SMSG_LEARNED_SPELLS_BATCH (0x2026) и красит слоты. Здесь — только
    // собственное свечение оверлея окна + лог списка новых спеллов.
    GoldGlowRemaining = GOLD_GLOW_SECONDS;
    if (GlowOverlay) {
        GlowOverlay->SetVisibility(ESlateVisibility::HitTestInvisible);
        GlowOverlay->SetOpacity(1.f);
    }
    UE_LOG(LogSpecSelect, Log, TEXT("Золотая подсветка %d новых способностей (%.0fс)"), NewSpellIds.Num(),
           GOLD_GLOW_SECONDS);
}
