// =============================================================================
// «Чёрная Материя» — UI_QuestLogWidget.cpp
// Журнал квестов 6–10 ур.; особый случай q_1104 -> кнопка «Выбрать путь».
// =============================================================================
#include "UI/UI_QuestLogWidget.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "MMOPlayerController.h"
#include "UI_SpecSelectionWidget.h"

DEFINE_LOG_CATEGORY_STATIC(LogQuestLog, Log, All);

void UUI_QuestLogWidget::NativeConstruct() {
    Super::NativeConstruct();
    InitializeLog();
}

void UUI_QuestLogWidget::InitializeLog() {
    if (CloseButton)
        CloseButton->OnClicked.AddDynamic(this, &UUserWidget::RemoveFromParent);
    RefreshQuestList();
}

void UUI_QuestLogWidget::RefreshQuestList() {
    if (!QuestListContainer)
        return;
    QuestListContainer->ClearChildren();

    // Источник истины — кэш контроллера (SMSG_QUEST_LIST / SMSG_QUEST_PROGRESS).
    // Пока репликация квестов в M-стадии, controller отдаёт пустой список и
    // работают только вручную добавленные записи (AddQuestTracker/BP).
    if (AMMOPlayerController *PC = Cast<AMMOPlayerController>(GetOwningPlayer()))
        Trackers = PC->GetQuestTrackers();

    for (const FQuestTrackerView &Tracker : Trackers) {
        if (UUserWidget *Row = BuildTrackerRow(Tracker))
            QuestListContainer->AddChild(Row);
    }
}

void UUI_QuestLogWidget::AddQuestTracker(const FQuestTrackerView &Data) {
    // Дедуп по QuestId: повторный вызов обновляет запись, а не дублирует её.
    for (FQuestTrackerView &Existing : Trackers) {
        if (Existing.QuestId == Data.QuestId) {
            Existing = Data;
            RefreshQuestList();
            return;
        }
    }
    Trackers.Add(Data);
    if (QuestListContainer) {
        if (UUserWidget *Row = BuildTrackerRow(Data))
            QuestListContainer->AddChild(Row);
    }
}

UUserWidget *UUI_QuestLogWidget::BuildTrackerRow(const FQuestTrackerView &Data) {
    if (!QuestRowClass) {
        UE_LOG(LogQuestLog, Warning, TEXT("QuestRowClass не назначен — строка '%s' пропущена"),
               *Data.Title.ToString());
        return nullptr;
    }

    UUserWidget *Row = CreateWidget<UUserWidget>(this, QuestRowClass);
    if (!Row)
        return nullptr;

    // Контракт W_QuestTrackerRow: TextBlock_Title / TextBlock_Objective /
    // ActionButton (кнопка справа). Заполняем через generic-компоненты, BP
    // может переопрешить внешний вид.
    if (UTextBlock *Title = Cast<UTextBlock>(Row->GetWidgetFromName(TEXT("TextBlock_Title"))))
        Title->SetText(Data.Title);

    if (UTextBlock *Objective = Cast<UTextBlock>(Row->GetWidgetFromName(TEXT("TextBlock_Objective")))) {
        if (Data.bSpecChoiceQuest)
            Objective->SetText(FText::GetEmpty());
        else
            Objective->SetText(FText::FromString(
                FString::Printf(TEXT("%s (%d/%d)"), *Data.ObjectiveText, Data.ObjectivesCompleted,
                                FMath::Max(1, Data.ObjectivesTotal))));
    }

    // ОСОБЫЙ СЛУЧАЙ: квест выбора специализации (q_1104).
    if (Data.QuestId == QUEST_SPEC_SELECTION || Data.bSpecChoiceQuest) {
        if (UButton *ActionButton = Cast<UButton>(Row->GetWidgetFromName(TEXT("ActionButton")))) {
            if (UTextBlock *BtnLabel = Cast<UTextBlock>(ActionButton->GetChildAt(0)))
                BtnLabel->SetText(FText::FromString(TEXT("Выбрать путь")));
            ActionButton->OnClicked.Clear();
            ActionButton->OnClicked.AddDynamic(this, &UUI_QuestLogWidget::OnSpecQuestButtonClicked);
        }
    }
    return Row;
}

void UUI_QuestLogWidget::OnSpecQuestButtonClicked() {
    if (!SpecSelectionClass) {
        UE_LOG(LogQuestLog, Warning, TEXT("SpecSelectionClass не назначен на журнале квестов"));
        return;
    }

    AMMOPlayerController *PC = Cast<AMMOPlayerController>(GetOwningPlayer());
    if (!PC)
        return;

    UUI_SpecSelectionWidget *SpecModal = CreateWidget<UUI_SpecSelectionWidget>(this, SpecSelectionClass);
    if (!SpecModal)
        return;
    SpecModal->AddToViewport(10); // поверх журнала
    // Данные спеков придут по SMSG_SPEC_SELECTION_OPEN (controller роутит их
    // в тот же класс модалки); если окно открыто из журнала повторно — сервер
    // шлёт актуальный whitelist, перетирая панели.
    SpecModal->OpenForClass(PC->ActiveClass, PC->GetAvailableSpecIds());
    UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(PC, SpecModal, EMouseLockMode::DoNotLock);
}
