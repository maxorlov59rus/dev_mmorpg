// =============================================================================
// «Чёрная Материя» — UI_QuestLogWidget.h (артефакт 3 промпта
// «Квестовая система 6–10: Инстансы и Специализации»).
//
// Журнал квестов: список активных заданий (6–9 ур. — стандартные трекеры),
// особый случай q_1104 «Путь избранного» (10 ур.): вместо «Найти NPC» в
// строке появляется кнопка «Выбрать путь», открывающая модалку
// UUI_SpecSelectionWidget.
//
// Данные берутся из кэша AMMOPlayerController, который наполняется пакетами
// SMSG_QUEST_LIST / SMSG_QUEST_PROGRESS; до готовности репликации виджет
// умеет работать на вручную добавленных записях (AddQuestTracker) — удобно
// для BP-прототипирования.
//
// Привязки Widget Blueprint (BindWidget):
//   QuestListContainer (UVerticalBox), CloseButton (UButton).
// =============================================================================
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI_QuestLogWidget.generated.h"

class UVerticalBox;
class UButton;
class UUserWidget;
class UUI_SpecSelectionWidget;

/** Одна запись журнала (трекер) в UI-представлении. */
USTRUCT(BlueprintType)
struct FQuestTrackerView {
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite) uint32 QuestId = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FText Title;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FString ObjectiveText;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 ObjectivesCompleted = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 ObjectivesTotal = 1;
    /** true => special quest (q_1104): кнопка «Выбрать путь» вместо трекера. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bSpecChoiceQuest = false;
};

UCLASS(Abstract, Blueprintable)
class MMOCORE_API UUI_QuestLogWidget : public UUserWidget {
    GENERATED_BODY()

public:
    /** Инициализация: подписка Close + первичное RefreshQuestList(). */
    UFUNCTION(BlueprintCallable, Category = "MMO|Quest")
    void InitializeLog();

    /** Перечитать активные квесты из кэша контроллера и перестроить список. */
    UFUNCTION(BlueprintCallable, Category = "MMO|Quest")
    void RefreshQuestList();

    /** Ручное добавление записи (сетевой кэш/BP-прототип). */
    UFUNCTION(BlueprintCallable, Category = "MMO|Quest")
    void AddQuestTracker(const FQuestTrackerView &Data);

    /** Кнопка «Выбрать путь» в строке q_1104 -> открыть модалку спеков. */
    UFUNCTION()
    void OnSpecQuestButtonClicked();

protected:
    virtual void NativeConstruct() override;

    // ---- BindWidget (см. шапку файла) ----
    UPROPERTY(meta = (BindWidget)) TObjectPtr<UVerticalBox> QuestListContainer;
    UPROPERTY(meta = (BindWidget)) TObjectPtr<UButton> CloseButton;

    /** Класс строки журнала (W_QuestTrackerRow): обязателен для C++-построения. */
    UPROPERTY(EditAnywhere, Category = "MMO|Quest")
    TSubclassOf<UUserWidget> QuestRowClass;

    /** Класс модалки выбора специализации (BP-дочерний UUI_SpecSelectionWidget). */
    UPROPERTY(EditAnywhere, Category = "MMO|Quest")
    TSubclassOf<UUI_SpecSelectionWidget> SpecSelectionClass;

private:
    /** Построить одну строку: текст цели или кнопка «Выбрать путь». */
    UUserWidget *BuildTrackerRow(const FQuestTrackerView &Data);

    TArray<FQuestTrackerView> Trackers;

    /** Квест выбора специализации (quest_template.ID = 1104, «Путь избранного»). */
    static constexpr uint32 QUEST_SPEC_SELECTION = 1104;
};
