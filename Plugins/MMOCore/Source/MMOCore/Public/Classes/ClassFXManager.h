// =============================================================================
// «Чёрная Материя» — ClassFXManager.h (артефакт 4 промпта «Система классов»)
//
// Реестр визуальных эффектов по классу/спеке. Загружает Niagara-системы из
// DT_ClassInfo / DT_SpecInfo (TSoftObjectPtr -> асинхронный Streamable-кэш,
// критично для Android: не держим все VFX в памяти).
//
// Триггеры:
//   * MMOPlayerController::HandleSpellCast (SMSG_CAST_RESULT/SMSG_SPELL_GO)
//     -> PlayClassCastFx(SpellId, CasterMesh, TargetLoc)
//   * Спринт разбойника (SpecID==SPEC_ROG_SHADOW) -> afterimage через TickComponent.
// =============================================================================
#pragma once

#include "CoreMinimal.h"
#include "Components/GameInstanceSubsystem.h"
#include "NiagaraSystem.h"
#include "UObject/SoftObjectPath.h"
#include "ClassFXManager.generated.h"

class UDataTable;
class USkeletalMeshComponent;
class UNiagaraComponent;
class AActor;

/** Ключ реестра: класс + спека + тип момента. */
USTRUCT(BlueprintType)
struct FDmFxKey {
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite) uint8 ClassId = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) uint8 SpecId = 0; // 0 = любой
    /** 0 cast-hit 1 cast-self(бафф) 2 sprint-trail 3 proc-passive 4 death */
    UPROPERTY(EditAnywhere, BlueprintReadWrite) uint8 Moment = 0;

    bool operator==(const FDmFxKey &O) const {
        return ClassId == O.ClassId && SpecId == O.SpecId && Moment == O.Moment;
    }
};

FORCEINLINE uint32 GetTypeHash(const FDmFxKey &K) {
    return HashCombine(HashCombine(GetTypeHash(K.ClassId), GetTypeHash(K.SpecId)), GetTypeHash(K.Moment));
}

USTRUCT()
struct FFDmFxEntry {
    GENERATED_BODY()

    UPROPERTY() TSoftObjectPtr<UNiagaraSystem> System;
    /** Attach-точка скелета ("hand_r_socket", "root", "" = world-space). */
    UPROPERTY() FName Socket = NAME_None;
    /** Duration для world-spawn систем (0 = one-shot auto-remove). */
    UPROPERTY() float AutoDestroySec = 0.f;
    /** Mobile LOD: на Android система заменяется на буст с -50% частиц. */
    UPROPERTY() TSoftObjectPtr<UNiagaraSystem> SystemMobile;
};

UCLASS(Config = MMOCore)
class MMOCORE_API UClassFXManager : public UGameInstanceSubsystem {
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase &Collection) override;

    UFUNCTION(BlueprintCallable, Category = "MMO|ClassFX", DisplayName = "Get Class FX Manager")
    static UClassFXManager *Get(UObject *WorldContextObject);

    /** Основной вход из сетевого кода: каст SpellId юнитом ClassId/SpecId.
        Возвращает spawned-компонент (или nullptr если эффекта нет). */
    UFUNCTION(BlueprintCallable, Category = "MMO|ClassFX")
    UNiagaraComponent *PlayClassCastFx(uint32 SpellId, uint8 ClassId, uint8 SpecId,
                                       USkeletalMeshComponent *CasterMesh, FVector TargetLocation);

    /** Afterimage-шлейф Порожника/Тени: вызывается из CharacterTick на спринте. */
    UFUNCTION(BlueprintCallable, Category = "MMO|ClassFX")
    void EmitSprintTrail(USkeletalMeshComponent *Mesh, uint8 ClassId, uint8 SpecId, float DeltaSeconds);

    /** Красная вспышка экрана (post-process) для «Кровавой ярости» и т.п. */
    UFUNCTION(BlueprintCallable, Category = "MMO|ClassFX")
    void TriggerScreenFlash(FLinearColor Color, float Intensity, float DurationSec);

    /** Гибд-регистрация из DataTable (вызывается Initialize; BP может докинуть свои ключи). */
    UFUNCTION(BlueprintCallable, Category = "MMO|ClassFX")
    void RegisterFx(FDmFxKey Key, FFDmFxEntry Entry);

private:
    const FFDmFxEntry *Lookup(const FDmFxKey &Key) const;
    UNiagaraSystem *ResolveSystem(const FFDmFxEntry &Entry) const;

    UPROPERTY(Config) TObjectPtr<UDataTable> ClassFxTable; // DT_ClassFX (FDMClassFxRow)

    TMap<FDmFxKey, FFDmFxEntry> Registry;
    /** Кэш загруженных систем (Streamable handles живут до Deinitialize). */
    mutable TMap<FSoftObjectPath, FStreamableHandle> LoadHandles;
    float TrailAccumulator = 0.f;
};
