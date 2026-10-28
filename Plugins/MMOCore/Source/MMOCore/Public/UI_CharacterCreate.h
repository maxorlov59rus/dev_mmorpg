// =============================================================================
// «Чёрная Материя» — UI_CharacterCreate.h (артефакт 3, промпт «Система классов»)
//
// Экран создания персонажа: выбор Class -> фильтрация Race, выбор Spec
// (3 кнопки), live-пересчёт блока «Базовые характеристики», сборка
// FMMOCharCreatePacket (с customization blob) и отправка через FMMONetworkClient.
//
// Привязки Widget Blueprint (BindWidget обязательны — виджет не соберётся без них):
//   ScrollBox_ClassButtons / ScrollBox_RaceButtons / HorizontalBox_SpecButtons
//   TextBlock_StatsSummary / TextBlock_NameError / Button_Create
//   Slider_* (morph-слайдеры, см. CharacterCustomization.h)
// =============================================================================
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/HorizontalBox.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "MMOProtocol.h"
#include "UI_CharacterCreate.generated.h"

class UDataTable;
class USkeletalMeshComponent;
class UFactionFXManager;

/** Кнопка-запись класса/расы (динамически клонируется в скролл-бокс). */
USTRUCT(BlueprintType)
struct FDMClassButtonEntry {
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadWrite) uint8 Id = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FText Label;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) TSoftObjectPtr<UTexture2D> Icon;
};

UCLASS(Abstract, Blueprintable)
class MMOCORE_API UUI_CharacterCreate : public UUserWidget {
    GENERATED_BODY()

public:
    // ---------- Источник данных (назначается в BP или GameInstance) ----------
    /** DataTable DT_ClassInfo (FDMClassRow) — каталог классов + спеков. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
    TObjectPtr<UDataTable> ClassInfoTable;

    /** DataTable DT_RaceInfo (FTableRowBase c AllowedClasses) — расы. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
    TObjectPtr<UDataTable> RaceInfoTable;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scene")
    TObjectPtr<USkeletalMeshComponent> PreviewMesh;

    // ---------- BindWidgets ----------
    UPROPERTY(meta = (BindWidget)) TObjectPtr<UScrollBox> ScrollBox_ClassButtons;
    UPROPERTY(meta = (BindWidget)) TObjectPtr<UScrollBox> ScrollBox_RaceButtons;
    UPROPERTY(meta = (BindWidget)) TObjectPtr<UHorizontalBox> HorizontalBox_SpecButtons;
    UPROPERTY(meta = (BindWidget)) TObjectPtr<UTextBlock> TextBlock_StatsPreview;
    UPROPERTY(meta = (BindWidget)) TObjectPtr<UTextBlock> TextBlock_NameError;
    UPROPERTY(meta = (BindWidget)) TObjectPtr<UButton> Button_Create;

    // ---------- Публичный API для BP-графиков ----------
    UFUNCTION(BlueprintCallable, Category = "CharCreate")
    void SelectClass(uint8 InClassId);

    UFUNCTION(BlueprintCallable, Category = "CharCreate")
    void SelectRace(uint8 InRaceId);

    UFUNCTION(BlueprintCallable, Category = "CharCreate")
    void SelectSpec(uint8 InSpecId);

    /** Собирает всё состояние UI в пакет и отправляет CMSG_CHAR_CREATE. */
    UFUNCTION(BlueprintCallable, Category = "CharCreate")
    bool SubmitCreation(const FString &InName, uint8 InGender, uint8 InFaction);

    /** Колбэк сети (подписан на OnPacketReceived в NativeConstruct). */
    UFUNCTION()
    void HandleServerPacket(EMMOOpcode Opcode, const FMMOPacket &Packet);

protected:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

    /** Пересчёт превью статов: базовые строки DataTable * StatMultipliers. */
    UFUNCTION(BlueprintCallable, Category = "CharCreate")
    void RecalculateStatsPreview();

    /** Блокировка недоступных кнопок расы при смене класса (и наоборот). */
    void RebuildRaceAvailability();
    void RebuildSpecButtons();

    static FString FormatStatsBlock(const TMap<EMMOBaseStat, float> &Effective);

    UPROPERTY(BlueprintReadOnly, Category = "State") uint8 SelectedClass = 0;
    UPROPERTY(BlueprintReadOnly, Category = "State") uint8 SelectedRace = 0;
    UPROPERTY(BlueprintReadOnly, Category = "State") uint8 SelectedSpec = 0;

    /** Аккумулированные значения morph-слайдеров (ключ = EFaceSlider as uint8, пишет BP).
        Значение 0..1; при сборке blob квантуется в ValueX1000. */
    UPROPERTY(BlueprintReadOnly, Category = "State") TMap<uint8, float> MorphValues;

    UPROPERTY(BlueprintReadOnly, Category = "State") TArray<uint8> CachedCustomizationBlob;

    FDelegateHandle PacketHandle;

private:
    UFUNCTION() void OnCreateClicked();
    void ApplyFactionVisual(); // FactionFXManager::ApplyToMesh(SelectedFaction)
    uint8 PendingFaction = 0;
};
