// Copyright BlackMatter Team. All Rights Reserved.
//
// FactionFXManager.h — подсистема GameInstance: визуальные эффекты фракций.
// См. подробное описание в Private/FactionFXManager.cpp (WPO+Emissive вены,
// DataTable-конфиг DT_FactionVisuals, батч MID по corruption).

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/DataTable.h"
#include "CharacterCustomization.h"
#include "FactionFXManager.generated.h"

class USkeletalMeshComponent;
class UMaterialInterface;
class UMaterialInstanceDynamic;
class UDataTable;

/** Строка DT_FactionVisuals: материалы/палитры на фракцию. */
USTRUCT(BlueprintType)
struct FFactionVisualRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Faction")
	uint8 FactionId = 0; // 1=DawnGuard 2=BlackMatter(Круг Поглощения) 3=ThresholdGuild

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Faction|Materials")
	FSoftObjectPath SkinMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Faction|Materials")
	FSoftObjectPath EyeMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Faction|Materials")
	FSoftObjectPath ClothingMaterial;

	/** Индексы слотов материалов скелетной сетки (расозависимые, см. CharacterCustomizationDef.json). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Faction|Slots")
	int32 BodyMaterialSlot = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Faction|Slots")
	int32 EyeMaterialSlot = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Faction|Slots")
	int32 ClothingMaterialSlot = 2;

	/** Скорость пульса вен (материальный параметр PulseSpeed). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Faction|FX", meta = (ClampMin = "0.2", ClampMax = "3.0"))
	float PulseSpeed = 0.8f;
};

UCLASS(Config = MMOCore)
class MMOCORE_API UFactionFXManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	// --- UGameInstanceSubsystem ---
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	/** Доступ из BP/C++ через любой объект с WorldContext. */
	UFUNCTION(BlueprintCallable, Category = "MMO|FactionFX", DisplayName = "Get Faction FX Manager")
	static UFactionFXManager* Get(UObject* WorldContextObject);

	/** Наложение фракционного облика на превью-меш (экран создания / список персов). */
	UFUNCTION(BlueprintCallable, Category = "MMO|FactionFX")
	void SetFactionOnMesh(USkeletalMeshComponent* Mesh, uint8 FactionId, float Corruption01);

	/** Глобальный уровень corruption (0..1) из SMSG_CORRUPTION_STATE (value_x100 / 10000). */
	UFUNCTION(BlueprintCallable, Category = "MMO|FactionFX")
	void UpdateCorruption(float NewCorruption01);

	/** Хелпер UI_CharacterCreate: blob для CMSG_CHAR_CREATE. */
	UFUNCTION(BlueprintCallable, Category = "MMO|FactionFX")
	static TArray<uint8> SerializeCustomization(const FCharacterCustomization& Customization);

	/** Применение blob'а (morph targets + палитры) к мешу. Используется и для preview_customization. */
	UFUNCTION(BlueprintCallable, Category = "MMO|FactionFX")
	bool ApplyCustomizationToMesh(USkeletalMeshComponent* Mesh, const TArray<uint8>& Blob);

	float GetCorruption() const { return GlobalCorruption01; }

protected:
	/** DT_FactionVisuals (задаётся в config или BP-подклассе). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MMO|FactionFX", Config)
	TObjectPtr<UDataTable> FactionTable;

	/** Базовый материал «Чёрной материи» (WPO+Emissive вены). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MMO|FactionFX", Config)
	FSoftObjectPath BlackMatterBaseMaterial;

	/** Порог corruption, после которого вены лезут даже у не-Круга (0..1). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MMO|FactionFX", Config, meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float CorruptionVeinThreshold = 0.55f;

	/** Палитры индексов кастомизации (Skin/Hair/Eye), 16 записей каждая. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MMO|FactionFX", Config)
	TArray<FLinearColor> SkinPalette;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MMO|FactionFX", Config)
	TArray<FLinearColor> HairPalette;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MMO|FactionFX", Config)
	TArray<FLinearColor> EyePalette;

private:
	const FFactionVisualRow* FindFactionRow(uint8 FactionId) const;
	UMaterialInstanceDynamic* GetOrCreateVeinsMID(USkeletalMeshComponent* Mesh);
	UMaterialInstanceDynamic* FindVeinsMID(USkeletalMeshComponent* Mesh) const;
	UMaterialInstanceDynamic* FindOrLoadSkinMID(USkeletalMeshComponent* Mesh);
	float ComputeVeinIntensity(uint8 FactionId, float Corruption01) const;
	void ClearOverrides(USkeletalMeshComponent* Mesh);
	void TrackMesh(USkeletalMeshComponent* Mesh, uint8 FactionId);
	static FLinearColor PaletteColor(const TArray<FLinearColor>& Palette, uint8 Index);

	float GlobalCorruption01 = 0.f;

	/** Все активные превью-меши -> faction id (для пакетного обновления corruption). */
	TMap<TWeakObjectPtr<USkeletalMeshComponent>, uint8> Tracked;

	/** Тег наших veins-MID (TSet по UObject*), чтобы отличать от чужих динамических матов. */
	UPROPERTY(Transient)
	TArray<TObjectPtr<UMaterialInstanceDynamic>> VeinsMIDTagArr;
	TSet<UObject*> VeinsMIDTag;

	/** Кэш skin-MID на компонент (один MID на mesh). */
	TMap<TWeakObjectPtr<USkeletalMeshComponent>, TObjectPtr<UMaterialInstanceDynamic>> SkinMIDCache;
};
