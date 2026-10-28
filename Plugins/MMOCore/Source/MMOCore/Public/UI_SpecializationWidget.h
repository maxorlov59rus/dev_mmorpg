// =============================================================================
// «Чёрная Материя» — UI_SpecializationWidget.h/cpp (артефакт 4 промпта
// «Система классов»).
//
// Окно выбора специализации/талантов: дерево грузится из DataTable по SpecID
// (FDFSpecRow::TalentTreeTable). При наведении на узел таланта подсвечиваются
// все связи предков (Slate Bezier-кривые) и показывается предпросмотр новой
// способности (иконка + описание + SignatureVFX превью).
// =============================================================================
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/GridPanel.h"
#include "Data/ClassInfoDataTable.h"
#include "UI_SpecializationWidget.generated.h"

class UCanvasPanel;
class UImage;
class UTextBlock;
class USplineComponent;

/** Узел дерева талантов (строка DT_Talent_<Spec>). */
USTRUCT(BlueprintType)
struct FDMTalentRow : public FTableRowBase {
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite) uint16 TalentId = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) uint8 SpecId = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FText DisplayName;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FString Description;
    /** Ссылки на родительские TalentId (несколько = перекрёсток веток). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<uint16> ParentIds;
    /** Требование уровня персонажа для открытия узла. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite) uint8 RequiredLevel = 1;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) TSoftObjectPtr<UTexture2D> Icon;
    /** Изучаемая способность — предпросмотр в тултипе. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite) uint32 PreviewSpellId = 0;
    /** Позиция в сетке дерева (колонка 0..7, ряд 0..5). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite) uint8 GridCol = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) uint8 GridRow = 0;
};

UCLASS(Abstract, Blueprintable)
class MMOCORE_API UUI_SpecializationWidget : public UUserWidget {
    GENERATED_BODY()

public:
    /** Открыть окно для спеки (ClassID нужен для fallback-ветки «без спеки»). */
    UFUNCTION(BlueprintCallable, Category = "MMO|Spec")
    void OpenForSpec(uint8 InClassId, uint8 InSpecId);

    /** Наведение на узел -> подсветка связей + предпросмотр спелла. */
    UFUNCTION(BlueprintCallable, Category = "MMO|Spec")
    void HighlightTalentPath(uint16 TalentId);

    /** Клик по узлу: локальный выбор (отправка CMSG_SPEC_APPLY — M1). */
    UFUNCTION(BlueprintCallable, Category = "MMO|Spec")
    bool TrySelectTalent(uint16 TalentId, FString &OutError);

    UPROPERTY(meta = (BindWidget)) TObjectPtr<UTextBlock> TextBlock_SpecName;
    UPROPERTY(meta = (BindWidget)) TObjectPtr<UTextBlock> TextBlock_PreviewDesc;
    UPROPERTY(meta = (BindWidget)) TObjectPtr<UImage> Image_PreviewIcon;
    UPROPERTY(meta = (BindWidget)) TObjectPtr<UGridPanel> Grid_Talents;
    UPROPERTY(meta = (BindWidget)) TObjectPtr<UCanvasPanel> Canvas_Connections; // поверх Grid

protected:
    virtual void NativeConstruct() override;

    /** Каталог спеков (DT_SpecInfo, FDFSpecRow). Присваивается GameInstance. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
    TObjectPtr<UDataTable> SpecCatalogTable;

    UPROPERTY(BlueprintReadOnly, Category = "State") uint8 CurrentClass = 0;
    UPROPERTY(BlueprintReadOnly, Category = "State") uint8 CurrentSpec = 0;
    UPROPERTY(BlueprintReadOnly, Category = "State") TArray<uint16> HighlightedChain;

private:
    /** Рекурсивный обход ParentIds — цепочка до корня (для bezier-подсветки). */
    void BuildAncestorChain(uint16 TalentId, TArray<uint16> &InOut, int32 DepthGuard) const;
    const FDMTalentRow *FindTalent(uint16 TalentId) const;
    /** Перерисовать линии связей в Canvas_Connections (BP-хелпер рисует Spline). */
    void RedrawConnections();

    UDataTable *TalentTree = nullptr; // резолв FDFSpecRow::TalentTreeTable
};
