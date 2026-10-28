// =============================================================================
// «Чёрная Материя» — UI_SpecializationWidget.cpp
// Дерево талантов по SpecID + подсветка связей (bezier) + предпросмотр спелла.
// =============================================================================
#include "UI_SpecializationWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/GridPanel.h"
#include "Components/Image.h"
#include "Components/SplineComponent.h"
#include "Components/TextBlock.h"
#include "Engine/DataTable.h"

void UUI_SpecializationWidget::NativeConstruct() {
    Super::NativeConstruct();
}

void UUI_SpecializationWidget::OpenForSpec(uint8 InClassId, uint8 InSpecId) {
    CurrentClass = InClassId;
    CurrentSpec = InSpecId;
    TalentTree = nullptr;

    const FDFSpecRow *Spec = DMClassData::FindRowById<FDFSpecRow>(SpecCatalogTable, InSpecId);
    if (!Spec) {
        TextBlock_SpecName->SetText(FText::FromString(TEXT("Специализация не найдена")));
        return;
    }
    TextBlock_SpecName->SetText(Spec->DisplayName);

    // TalentTreeTable — TSoftObjectPtr<UDataTable>: резолвим синхронно на открытии
    // окна (дерево одно и маленькое; async-загрузка добавится с контент-паком M1).
    TalentTree = Spec->TalentTreeTable.LoadSynchronous();

    // Построение узлов: Grid_Talents заполняется BP-графиком WBP_TalentNode по
    // GetRowMap(); здесь — только логика связей/подсветки поверх сетки.
    HighlightedChain.Reset();
    RedrawConnections();
}

const FDMTalentRow *UUI_SpecializationWidget::FindTalent(uint16 TalentId) const {
    if (!TalentTree)
        return nullptr;
    const FName RowName = FName(*FString::Printf(TEXT("T_%u"), TalentId));
    return TalentTree->FindRow<FDMTalentRow>(RowName, TEXT("SpecializationWidget"), false);
}

void UUI_SpecializationWidget::BuildAncestorChain(uint16 TalentId, TArray<uint16> &InOut, int32 DepthGuard) const {
    if (DepthGuard <= 0 || InOut.Contains(TalentId))
        return; // защита от циклов в данных (design-таблица может ошибиться)
    InOut.Add(TalentId);
    if (const FDMTalentRow *Row = FindTalent(TalentId))
        for (uint16 Parent : Row->ParentIds)
            BuildAncestorChain(Parent, InOut, DepthGuard - 1);
}

void UUI_SpecializationWidget::HighlightTalentPath(uint16 TalentId) {
    HighlightedChain.Reset();
    BuildAncestorChain(TalentId, HighlightedChain, /*DepthGuard=*/32);
    RedrawConnections();

    // Предпросмотр новой способности: иконка + описание узла.
    if (const FDMTalentRow *Row = FindTalent(TalentId)) {
        TextBlock_PreviewDesc->SetText(FText::FromString(
            FString::Printf(TEXT("%s\n%s\n[спелл %u] ур. %u+"), *Row->DisplayName.ToString(), *Row->Description,
                            Row->PreviewSpellId, Row->RequiredLevel)));
        if (Image_PreviewIcon && Row->Icon.IsValid())
            Image_PreviewIcon->SetBrushFromTexture(Row->Icon.LoadSynchronous());
    }
}

bool UUI_SpecializationWidget::TrySelectTalent(uint16 TalentId, FString &OutError) {
    const FDMTalentRow *Row = FindTalent(TalentId);
    if (!Row) {
        OutError = TEXT("Талант отсутствует в дереве спеки.");
        return false;
    }
    // Все родители должны быть уже выбраны (локальный пре-чек; authoritative — сервер).
    for (uint16 Parent : Row->ParentIds)
        if (!HighlightedChain.Contains(Parent) && Parent != TalentId) {
            // Проверка «выбранности» живёт в BP-сторте выбранных TalentId:
            OutError = FString::Printf(TEXT("Сначала откройте родительский талант T_%u."), Parent);
            return false;
        }
    // TODO(M1): CMSG_SPEC_APPLY{spec_id, talent_id} -> SMSG_SPEC_APPLY_RESULT.
    HighlightTalentPath(TalentId);
    return true;
}

void UUI_SpecializationWidget::RedrawConnections() {
    if (!Canvas_Connections || !TalentTree)
        return;
    Canvas_Connections->ClearChildren();

    // Для каждой пары (parent -> child) рисуем USplineComponent с кривизной
    // Bezier (InterpCurve_Ceps) — визуальный стиль как в WoW-classic окне талантов.
    auto NodeCenter = [](const FDMTalentRow &R) {
        // Сетка 8x6, ячейка 96px; координаты совпадают с layout BP-виджета.
        return FVector2D(R.GridCol * 96.f + 48.f, R.GridRow * 96.f + 48.f);
    };

    for (const auto &Pair : TalentTree->GetRowMap()) {
        const FDMTalentRow *Child = Pair.Value.Get<FDMTalentRow>();
        if (!Child)
            continue;
        for (uint16 ParentId : Child->ParentIds) {
            const FDMTalentRow *Parent = FindTalent(ParentId);
            if (!Parent)
                continue;
            USplineComponent *Line = WidgetTree->ConstructWidget<USplineComponent>(
                USplineComponent::StaticClass(), FName(*FString::Printf(TEXT("Link_%u_%u"), ParentId, Child->TalentId)));
            const FVector2D A = NodeCenter(*Parent), B = NodeCenter(*Child);
            Line->AddSplinePoint(A, ESplineCoordinateSpace::Local);
            Line->AddSplinePoint(B, ESplineCoordinateSpace::Local);
            Line->SetSplinePointType(0, ESplinePointType::Curve);
            Line->SetSplinePointType(1, ESplinePointType::Curve);
            const bool bOnPath = HighlightedChain.Contains(ParentId) && HighlightedChain.Contains(Child->TalentId);
            Line->SetThickness(0, bOnPath ? 5.f : 2.f);
            Canvas_Connections->AddChild(Line);
        }
    }
}
