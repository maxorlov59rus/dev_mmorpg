// =============================================================================
// «Чёрная Материя» — ClassFXManager.cpp (артефакт 4 промпта «Система классов»)
// Визуализация классовых способностей: Воин (motion-blur/камень), Маг (ледяные
// кристаллы Niagara), Разбойник (afterimage-шлейф). РеестрSpellId -> Key.
// =============================================================================
#include "Classes/ClassFXManager.h"

#include "Data/ClassInfoDataTable.h"
#include "Engine/GameInstance.h"
#include "Engine/StreamableManager.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/SkeletalMeshComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogClassFX, Log, All);

namespace {
// Классовые спеллы из class_spells (sql/upgrades/Create_Class_Database.sql).
// Единый источник нумерации — TRC-совместимые ID; новые добавлять в конец.
constexpr uint32 SPELL_HEROIC_STRIKE = 100;
constexpr uint32 SPELL_CHARGE = 116;
constexpr uint32 SPELL_FIREBALL = 133;
constexpr uint32 SPELL_FROST_NOVA = 122;
constexpr uint32 SPELL_BACKSTAB = 1752;
constexpr uint32 SPELL_EVISCARATE = 53;
} // namespace

void UClassFXManager::Initialize(FSubsystemCollectionBase &Collection) {
    Super::Initialize(Collection);

    // Дефолтный реестр (контент-таблица DT_ClassFX может переопределить любой
    // ключ через RegisterFx из BP — DataTable грузится позже на login).
    auto Reg = [this](uint8 Cls, uint8 Spec, uint8 Moment, const TCHAR *Asset, const TCHAR *Socket, float AutoSec) {
        FDmFxKey K{Cls, Spec, Moment};
        FFDmFxEntry E;
        E.System = FSoftObjectPath(FString::Printf(TEXT("/Game/VFX/Classes/%s.%s"), Asset, Asset));
        E.Socket = FName(Socket);
        E.AutoDestroySec = AutoSec;
        Registry.Add(K, E);
    };

    // ---- Воин: удар с размытием движения + каменные частицы ----
    Reg(1 /*Warrior*/, 0, 0, TEXT("NS_Warrior_MotionSmash"), TEXT("hand_r_socket"), 0.f);
    Reg(1, 0, 1, TEXT("NS_Warrior_ChargeDust"), NAME_None, 0.6f); // world-spawn на точке цели
    // ---- Маг: ледяные кристаллы вокруг рук (Frost nova) / огненная стрела ----
    Reg(3 /*Mage*/, 4 /*Frost*/, 0, TEXT("NS_Mage_FrostCrystals"), TEXT("hand_l_socket"), 0.f);
    Reg(3, 0, 0, TEXT("NS_Mage_FireboltTrail"), TEXT("hand_r_socket"), 0.f);
    // ---- Разбойник: afterimage при спринте + вспышка каста из стелса ----
    Reg(4 /*Rogue*/, 7 /*Shadow*/, 2, TEXT("NS_Rogue_Afterimage"), TEXT("root"), 0.35f);
    Reg(4, 0, 0, TEXT("NS_Rogue_BackstabSlash"), TEXT("weapon_socket"), 0.f);
}

UClassFXManager *UClassFXManager::Get(UObject *WorldContextObject) {
    if (UGameInstance *GI = WorldContextObject ? WorldContextObject->GetGameInstance() : nullptr)
        return GI->GetSubsystem<UClassFXManager>();
    return nullptr;
}

void UClassFXManager::RegisterFx(FDmFxKey Key, FFDmFxEntry Entry) {
    Registry.Add(Key, MoveTemp(Entry));
}

const FFDmFxEntry *UClassFXManager::Lookup(const FDmFxKey &Key) const {
    // Точное совпадение (class+spec+moment), иначе fallback spec=0 того же класса.
    if (const FFDmFxEntry *E = Registry.Find(Key))
        return E;
    FDmFxKey Fallback = Key;
    Fallback.SpecId = 0;
    return Registry.Find(Fallback);
}

UNiagaraSystem *UClassFXManager::ResolveSystem(const FFDmFxEntry &Entry) const {
    // Mobile LOD: на Android отдаём SystemMobile, если задан (docs/04 §Android).
    const bool bMobile = (GPlatformName == TEXT("Android"));
    const TSoftObjectPtr<UNiagaraSystem> &Want = (bMobile && Entry.SystemMobile.IsValid()) ? Entry.SystemMobile : Entry.System;
    if (!Want.IsValid())
        return nullptr;
    if (UNiagaraSystem *Loaded = Want.Get())
        return Loaded;

    // Асинхронная загрузка с кэшем handle; первый кадр эффект может не появиться —
    // это осознанный trade-off вместо hard-load stutter на средних Android.
    FStreamableHandle *&H = const_cast<FStreamableHandle *&>(LoadHandles.FindOrAdd(Want.ToSoftObjectPath()));
    if (!H || !H->IsActive()) {
        H = new FStreamableHandle();
        FStreamableManager::Get().RequestAsyncLoad(Want.ToSoftObjectPath(), *H);
    }
    return Want.Get(); // nullptr, пока не догружено
}

UNiagaraComponent *UClassFXManager::PlayClassCastFx(uint32 SpellId, uint8 ClassId, uint8 SpecId,
                                                    USkeletalMeshComponent *CasterMesh, FVector TargetLocation) {
    // Момент вывода по таблице спеллов:
    uint8 Moment = 0; // cast-hit
    switch (SpellId) {
    case SPELL_HEROIC_STRIKE:
    case SPELL_BACKSTAB:
    case SPELL_EVISCARATE:
    case SPELL_FIREBALL:
    case SPELL_FROST_NOVA:
        Moment = 0;
        break;
    case SPELL_CHARGE:
        Moment = 1; // self/world-бафф рывок
        break;
    default:
        break;
    }

    FDmFxKey Key{ClassId, SpecId, Moment};
    const FFDmFxEntry *Entry = Lookup(Key);
    if (!Entry) {
        UE_LOG(LogClassFX, Verbose, TEXT("No FX registered for spell %u (class %u spec %u moment %u)"), SpellId, ClassId,
               SpecId, Moment);
        return nullptr;
    }
    UNiagaraSystem *Sys = ResolveSystem(*Entry);
    if (!Sys)
        return nullptr;

    UWorld *World = CasterMesh ? CasterMesh->GetWorld() : GetGameInstance()->GetWorld();
    if (!World)
        return nullptr;

    // Attach к сокету меша или world-space на позиции цели.
    if (CasterMesh && Entry->Socket != NAME_None) {
        UNiagaraComponent *Comp = UNiagaraFunctionLibrary::SpawnSystemAttached(
            Sys, CasterMesh, Entry->Socket, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset,
            /*bAutoDestroy=*/true);
        return Comp;
    }
    UNiagaraComponent *Comp =
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(World, Sys, TargetLocation, FRotator::ZeroRotator, FVector(1.f),
                                                       /*bAutoDestroy=*/true);
    return Comp;
}

void UClassFXManager::EmitSprintTrail(USkeletalMeshComponent *Mesh, uint8 ClassId, uint8 SpecId, float DeltaSeconds) {
    if (!Mesh)
        return;
    // Only rogues shadow/gate specs have trails; throttle to ~12 emitters/sec on mobile.
    static constexpr float TrailInterval = 0.083f;
    TrailAccumulator += DeltaSeconds;
    if (TrailAccumulator < TrailInterval)
        return;
    TrailAccumulator = 0.f;

    FDmFxKey Key{ClassId, SpecId, /*Moment=*/2};
    const FFDmFxEntry *Entry = Lookup(Key);
    if (!Entry)
        return;
    UNiagaraSystem *Sys = ResolveSystem(*Entry);
    if (!Sys)
        return;
    UNiagaraFunctionLibrary::SpawnSystemAttached(Sys, Mesh, Entry->Socket, FVector::ZeroVector,
                                                 FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, true);
}

void UClassFXManager::TriggerScreenFlash(FLinearColor Color, float Intensity, float DurationSec) {
    // Post-process boost через UMG overlay (BP-виджет WBP_ScreenFX слушает делегат);
    // здесь — только лог-маркер + GameplayStatics-impulse для camera-shake тяжёлыхSpi.
    if (UWorld *World = GetGameInstance()->GetWorld()) {
        UE_LOG(LogClassFX, Log, TEXT("Screen flash color=%s intensity=%.2f dur=%.2f"), *Color.ToString(), Intensity,
               DurationSec);
        // TODO(M1): APostProcessVolume blend-in/out; сейчас визуал делает BP overlay.
        (void)World;
    }
}
