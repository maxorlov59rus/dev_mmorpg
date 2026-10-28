// =============================================================================
// «Чёрная Материя» — ClassInfoDataTable.h (артефакт 3 промпта «Система классов»)
//
// Единый клиентский источник отображаемых данных о классах/спеках.
// Строки генерируются пайплайном docs/04 §5: DataTable -> .dbc -> pak,
// поэтому серверные SQL-таблицы (class_specs) и этот DataTable не могут
// разойтись — оба собираются из одного design-листа (xlsx/CSV в Content/).
// Жёсткой привязки ID в коде НЕТ: все lookup идут через GetRowClass.
// =============================================================================
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ClassInfoDataTable.generated.h"

UENUM(BlueprintType)
enum class EMMOBaseStat : uint8 {
    None UMETA(DisplayName = "-"),
    Strength UMETA(DisplayName = "Сила"),
    Agility UMETA(DisplayName = "Ловкость"),
    Stamina UMETA(DisplayName = "Выносливость"),
    Intellect UMETA(DisplayName = "Интеллект"),
    Spirit UMETA(DisplayName = "Дух"),
};

/** Строка класса: имя, иконка, множители статов расы для превью на экране создания. */
USTRUCT(BlueprintType)
struct FDMClassRow : public FTableRowBase {
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Class")
    uint8 ClassID = 0; // CLASS_* (контракт PlayerClassMgr.h / Classes.dbc)

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Class")
    FText DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Class")
    FString Description;

    /** FSlateBrush-путь к иконке (TSoftObjectPtr для лёгкой выгрузки на Android). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Class")
    TSoftObjectPtr<UTexture2D> Icon;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Class")
    TSoftObjectPtr<UTexture2D> PortraitSilhouette;

    /** На сколько % базовый стат РАСЫ усиливается этим классом (превью до сети).
        Ключ = индекс stata; сервер authoritative — это только UI-эвристика. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Class")
    TMap<EMMOBaseStat, float> StatMultipliers;

    /** Разрешённые расы (зеркало class_restrictions inverse-маски; сверяется
        с SMSG_CLASS_INFO при открытии экрана — расхождение = warning в лог). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Class")
    TArray<uint8> AllowedRaces;

    /** Доступные специализации (порядок = порядок кнопок в UI_CharacterCreate). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Class")
    TArray<uint8> SpecIDs;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Class")
    uint8 PrimaryStat = 0; // 1..5 как в class_specs.primary_stat
};

/** Строка специализации (дерево талантов грузится отдельно по TalentTreeID). */
USTRUCT(BlueprintType)
struct FDFSpecRow : public FTableRowBase {
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spec")
    uint8 SpecID = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spec")
    uint8 ClassID = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spec")
    FText DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spec")
    FString Description;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spec")
    TSoftObjectPtr<UTexture2D> Icon;

    /** Таблица талантов спеки (DT_Talent_<Spec>): UI_SpecializationWidget
        резолвит и строит дерево + связи. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spec")
    TSoftObjectPtr<UDataTable> TalentTreeTable;

    /** Niagara/VFX-система «фирменного» визуала спеки (ClassFXManager keyframe). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spec")
    TSoftObjectPtr<UNiagaraSystem> SignatureVFX;
};

/** Утилиты доступа (thin wrapper над UDataTable::FindRow, без кэша состояний). */
namespace DMClassData {
template <typename TRow> const TRow *FindRowById(const UDataTable *Table, uint8 Id) {
    if (!Table)
        return nullptr;
    static FName Prefix(TEXT("ID_"));
    const FName RowName = FName(*FString::Printf(TEXT("%s%d"), *Prefix.ToString(), Id));
    return Table->FindRow<TRow>(RowName, TEXT("DMClassData"), /*bWarnIfRowMissing=*/false);
}
} // namespace DMClassData
