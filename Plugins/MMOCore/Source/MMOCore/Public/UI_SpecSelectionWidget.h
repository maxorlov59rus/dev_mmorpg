// =============================================================================
// «Чёрная Материя» — UI_SpecSelectionWidget.h/cpp (артефакт 3 промпта
// «Квестовая система 6–10: Инстансы и Специализации»).
//
// КЛЮЧЕВОЙ элемент: модальное окно выбора специализации на 10 уровне.
//  * Открывается по SMSG_SPEC_SELECTION_OPEN (wire 0x2023) — сервер говорит
//    «открой окно», клиент строит панели из DataTable DT_SpecPanel (FDFSpecRow).
//  * Две панели (Spec 1 / Spec 2): иконка, название, большой текст описания,
//    список из 3 новых способностей (иконки + тултипы), превью экипировки на
//    заднем плане (щит+меч для танка, двуручник/парные клинки для ДД).
//  * Кнопка «Подтвердить» активна ТОЛЬКО после выбора панели; шлёт
//    CMSG_CHOOSE_SPEC (0x2024) и блокируется до SMSG_SPEC_CHOSEN (0x2025).
//  * После подтверждения сервер отвечает SMSG_LEARNED_SPELLS_BATCH (0x2026) —
//    AMMOPlayerController бродкастит OnLearnSpell, Action bar подсвечивает
//    новые слоты золотом на 10 секунд (TriggerGoldGlow).
//
// Наследник UI_SpecializationWidget (дерево талантов, M2) — здесь только
// первичный выбор пути; окно закрыто до подтверждения (модалка, gamepad-safe).
// =============================================================================
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/ClassInfoDataTable.h"
#include "UI_SpecSelectionWidget.generated.h"

class UCanvasPanel;
class UImage;
class UTextBlock;
class UButton;
class UHorizontalBox;
class UVerticalBox;
class UBorder;
class ACharacter;

/** Одна панель спекы внутри модалки. */
USTRUCT(BlueprintType)
struct FMMOSpecPanelView {
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite) uint8 SpecId = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FText DisplayName;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FText BigDescription;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) TSoftObjectPtr<UTexture2D> Icon;
    /** Три ключевые способности (SpellID -> иконки/тултипы через DT_Spells). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<uint32> AbilitySpellIds;
    /** Роль LFG для превью: true = щит+меч (танк), false = двуручник/парные (ДД). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bTankPreview = false;
    /** Превью-экипировка: mesh'и оружия (SpawnActor в фоновой сцене виджета). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FSoftObjectPath WeaponMesh;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FSoftObjectPath OffhandMesh; // пуст = нет
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FMMOOnSpecConfirmed, uint8, SpecId, bool, bSuccess);

UCLASS(Abstract, Blueprintable)
class MMOCORE_API UUI_SpecSelectionWidget : public UUserWidget {
    GENERATED_BODY()

public:
    /** Открыть модалку по пакету SMSG_SPEC_SELECTION_OPEN (класс + доступные спеки). */
    UFUNCTION(BlueprintCallable, Category = "MMO|Spec")
    void OpenForClass(uint8 ClassId, const TArray<uint8> &SpecIds);

    /** Выбор панели (клик по карточке): подсветка + смена превью-экипировки. */
    UFUNCTION(BlueprintCallable, Category = "MMO|Spec")
    void SelectSpecPanel(int32 PanelIndex);

    /** «Подтвердить»: активна только после SelectSpecPanel. Шлёт CMSG_CHOOSE_SPEC. */
    UFUNCTION(BlueprintCallable, Category = "MMO|Spec")
    void ConfirmSelection();

    /** Колбэк PlayerController на SMSG_SPEC_CHOSEN — разблокировать/закрыть. */
    UFUNCTION(BlueprintCallable, Category = "MMO|Spec")
    void OnServerConfirmed(uint8 SpecId, uint8 LfgRoleMask);

    UPROPERTY(BlueprintAssignable, Category = "MMO|Events")
    FMMOOnSpecConfirmed OnSpecConfirmed;

    /** BP-контракт: смена превью-оружия на фоне окна (щит+меч <-> двуручник/парные). */
    UFUNCTION(BlueprintImplementableEvent, Category = "MMO|Spec")
    void K2_OnPreviewEquipChanged(bool bTankPreview, const FSoftObjectPath &WeaponMesh,
                                  const FSoftObjectPath &OffhandMesh);

protected:
    virtual void NativeConstruct() override;
    virtual void NativeTick(const FGeometry &MyGeometry, float InDeltaTime) override;

    // ---- Панели (BuildPanel создаёт контент из DT_SpecPanel) ----
    UPROPERTY(meta = (BindWidget)) TObjectPtr<UHorizontalBox> PanelsRow;      // две колоны
    UPROPERTY(meta = (BindWidget)) TObjectPtr<UTextBlock> HeaderText;         // «Путь избранного»
    UPROPERTY(meta = (BindWidget)) TObjectPtr<UButton> ConfirmButton;         // disabled до выбора
    UPROPERTY(meta = (BindWidget)) TObjectPtr<UTextBlock> ConfirmLabel;
    UPROPERTY(meta = (BindWidget)) TObjectPtr<UImage> PreviewSilhouette;      // фон: модель перса

    /** Таблица панелей (строки FDFSpecRow маппятся в FMMOSpecPanelView). */
    UPROPERTY(EditAnywhere, Category = "MMO|Data")
    TObjectPtr<UDataTable> SpecPanelTable; // DT_SpecPanel (ClassInfoDataTable.h)

    /** Тултип-таблица способностей (иконка+описание по SpellID). */
    UPROPERTY(EditAnywhere, Category = "MMO|Data")
    TObjectPtr<UDataTable> SpellTooltipTable;

private:
    /** Построить карточки панелей (BP-заглушки до готовности W_SpecCard). */
    void RebuildPanels();

    /** Смена превью-экипировки на фоне окна (щит+меч <-> двуручник/парные). */
    void UpdateEquipmentPreview(const FMMOSpecPanelView &View);

    /** Золотая подсветка новых способностей в action bar (10 секунд). */
    void TriggerGoldGlow(const TArray<uint32> &NewSpellIds);

    void SetConfirmEnabled(bool bEnabled);

    uint8 ActiveClassId = 0;
    TArray<FMMOSpecPanelView> Panels;
    int32 SelectedIndex = INDEX_NONE;   // -1 => Confirm неактивна
    bool bAwaitingServer = false;       // CMSG отправлен, ждём SMSG_SPEC_CHOSEN
    float ConfirmBlockUntil = 0.f;      // анти-спам (double click)
    float GoldGlowRemaining = 0.f;      // таймер золотого свечения (10с)

    /** Оверлей свечения (BindWidgetOpt: виджет работает и без него). */
    UPROPERTY(meta = (BindWidgetOptional)) TObjectPtr<UImage> GlowOverlay;
};
