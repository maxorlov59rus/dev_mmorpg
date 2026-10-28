// Copyright BlackMatter Team. All Rights Reserved.

#include "FactionFXManager.h"

#include "Components/SkeletalMeshComponent.h"
#include "Engine/DataTable.h"
#include "Engine/SkinnedAssetCommon.h"
#include "Materials/MaterialInterface.h"
#include "Materials/MaterialInstanceDynamic.h"

DEFINE_LOG_CATEGORY_STATIC(LogFactionFX, Log, All);

namespace
{
	/** Имена morph-таргетов в SKM (соглашение из CharacterCustomizationDef.json). */
	const TCHAR* MorphNameForSlider(EFaceSlider Slider)
	{
		switch (Slider)
		{
		case EFaceSlider::JawWidth:        return TEXT("morph_jaw_width");
		case EFaceSlider::JawProjection:   return TEXT("morph_jaw_proj");
		case EFaceSlider::CheekboneHeight: return TEXT("morph_cheek_h");
		case EFaceSlider::CheekFullness:   return TEXT("morph_cheek_full");
		case EFaceSlider::NoseWidth:       return TEXT("morph_nose_w");
		case EFaceSlider::NoseLength:      return TEXT("morph_nose_len");
		case EFaceSlider::NoseBridgeHump:  return TEXT("morph_nose_hump");
		case EFaceSlider::EyeSpacing:      return TEXT("morph_eye_space");
		case EFaceSlider::EyeSize:         return TEXT("morph_eye_size");
		case EFaceSlider::EyeTilt:         return TEXT("morph_eye_tilt");
		case EFaceSlider::BrowAngle:       return TEXT("morph_brow_angle");
		case EFaceSlider::BrowThickness:   return TEXT("morph_brow_thick");
		case EFaceSlider::LipFullness:     return TEXT("morph_lip_full");
		case EFaceSlider::LipWidth:        return TEXT("morph_lip_w");
		case EFaceSlider::EarSize:         return TEXT("morph_ear_size");
		case EFaceSlider::ChinProjection:  return TEXT("morph_chin_proj");
		case EFaceSlider::ChinWidth:       return TEXT("morph_chin_w");
		case EFaceSlider::ForeheadTilt:    return TEXT("morph_forehead");
		default:                           return nullptr;
		}
	}
}

// ---------------------------------------------------------------------------
// Subsystem lifecycle
// ---------------------------------------------------------------------------

UFactionFXManager* UFactionFXManager::Get(UObject* WorldContextObject)
{
	if (!WorldContextObject)
	{
		return nullptr;
	}
	UWorld* World = WorldContextObject->GetWorld();
	if (!World)
	{
		return nullptr;
	}
	UGameInstance* GI = World->GetGameInstance();
	if (!GI)
	{
		return nullptr;
	}
	return GI->GetSubsystem<UFactionFXManager>();
}

void UFactionFXManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// Палитры по умолчанию, если дизайнер не задал их в config ([MMOCore] FactionFXManager).
	if (SkinPalette.Num() == 0)
	{
		static const uint8 SkinTones[16] = { 24, 40, 56, 72, 90, 108, 126, 144, 160, 176, 190, 204, 216, 226, 236, 246 };
		for (uint8 t : SkinTones)
		{
			SkinPalette.Add(FLinearColor(t / 255.f, t / 255.f * 0.92f, t / 255.f * 0.84f));
		}
	}
	if (HairPalette.Num() == 0)
	{
		HairPalette = {
			FLinearColor(0.05f, 0.04f, 0.03f), FLinearColor(0.25f, 0.15f, 0.08f),
			FLinearColor(0.55f, 0.35f, 0.15f), FLinearColor(0.85f, 0.75f, 0.55f),
			FLinearColor(0.70f, 0.20f, 0.15f), FLinearColor(0.30f, 0.30f, 0.32f),
			FLinearColor(0.10f, 0.10f, 0.12f), FLinearColor(0.45f, 0.30f, 0.55f) };
	}
	if (EyePalette.Num() == 0)
	{
		EyePalette = {
			FLinearColor(0.20f, 0.35f, 0.25f), FLinearColor(0.25f, 0.40f, 0.65f),
			FLinearColor(0.45f, 0.30f, 0.15f), FLinearColor(0.15f, 0.15f, 0.15f),
			FLinearColor(0.55f, 0.55f, 0.50f), FLinearColor(0.30f, 0.60f, 0.70f),
			FLinearColor(0.60f, 0.20f, 0.20f), FLinearColor(0.35f, 0.10f, 0.45f) };
	}

	UE_LOG(LogFactionFX, Display, TEXT("FactionFXManager initialized (faction table=%s)"),
		FactionTable ? *FactionTable->GetName() : TEXT("<none - configure DT_FactionVisuals>"));
}

void UFactionFXManager::Deinitialize()
{
	Tracked.Empty();
	SkinMIDCache.Empty();
	VeinsMIDTag.Empty();
	VeinsMIDTagArr.Empty();
	Super::Deinitialize();
}

// ---------------------------------------------------------------------------
// Public API
// ---------------------------------------------------------------------------

void UFactionFXManager::SetFactionOnMesh(USkeletalMeshComponent* Mesh, uint8 FactionId, float Corruption01)
{
	if (!IsValid(Mesh))
	{
		UE_LOG(LogFactionFX, Warning, TEXT("SetFactionOnMesh: invalid mesh component"));
		return;
	}

	const FFactionVisualRow* Row = FindFactionRow(FactionId);
	if (!Row)
	{
		// Нет строки во фракционной таблице — чистый базовый скин без FX.
		ClearOverrides(Mesh);
		TrackMesh(Mesh, FactionId);
		return;
	}

	// 1) Кожа/глаза — статичные материалы из DataTable (без MID, дёшево на Android).
	if (UMaterialInterface* Skin = Cast<UMaterialInterface>(Row->SkinMaterial.TryLoad()))
	{
		Mesh->SetMaterial(Row->BodyMaterialSlot, Skin);
	}
	if (UMaterialInterface* Eyes = Cast<UMaterialInterface>(Row->EyeMaterial.TryLoad()))
	{
		Mesh->SetMaterial(Row->EyeMaterialSlot, Eyes);
	}

	// 2) Вены «Чёрной материи»: Круг Поглощения (faction==2) всегда; остальные — при
	//    corruption >= порога. WPO+Emissive живут в самом материале, здесь только MID-параметры.
	const bool bNeedVeins = (FactionId == 2) || (Corruption01 >= CorruptionVeinThreshold);
	if (bNeedVeins)
	{
		if (UMaterialInstanceDynamic* Mid = GetOrCreateVeinsMID(Mesh))
		{
			Mid->SetScalarParameterValue(TEXT("VeinIntensity"), ComputeVeinIntensity(FactionId, Corruption01));
			Mid->SetScalarParameterValue(TEXT("PulseSpeed"), Row->PulseSpeed);
			Mesh->SetMaterial(Row->ClothingMaterialSlot, Mid);
		}
	}
	else if (UMaterialInterface* Cloth = Cast<UMaterialInterface>(Row->ClothingMaterial.TryLoad()))
	{
		Mesh->SetMaterial(Row->ClothingMaterialSlot, Cloth);
	}

	TrackMesh(Mesh, FactionId);
}

void UFactionFXManager::UpdateCorruption(float NewCorruption01)
{
	GlobalCorruption01 = FMath::Clamp(NewCorruption01, 0.f, 1.f);

	// Батч всех активных MID за один проход (дешёвая операция: SetScalarParameterValue не тредшит шейдер).
	for (auto It = Tracked.CreateIterator(); It; ++It)
	{
		if (!It.Key().IsValid())
		{
			It.RemoveCurrent();
			continue;
		}
		if (UMaterialInstanceDynamic* Mid = FindVeinsMID(It.Key().Get()))
		{
			Mid->SetScalarParameterValue(TEXT("VeinIntensity"),
				ComputeVeinIntensity(It.Value(), GlobalCorruption01));
		}
	}
}

TArray<uint8> UFactionFXManager::SerializeCustomization(const FCharacterCustomization& Customization)
{
	TArray<uint8> Blob;
	if (!Customization.Serialize(Blob))
	{
		UE_LOG(LogFactionFX, Error, TEXT("SerializeCustomization failed (morphs=%d)"), Customization.Morphs.Num());
		Blob.Reset();
	}
	return Blob;
}

bool UFactionFXManager::ApplyCustomizationToMesh(USkeletalMeshComponent* Mesh, const TArray<uint8>& Blob)
{
	if (!IsValid(Mesh))
	{
		return false;
	}

	FCharacterCustomization C;
	if (!FCharacterCustomization::Deserialize(Blob, C))
	{
		UE_LOG(LogFactionFX, Warning, TEXT("ApplyCustomizationToMesh: bad blob (%d bytes)"), Blob.Num());
		return false;
	}

	// Морфы -> morph targets скелетной сетки (X1000 -> 0..1 float).
	if (USkeletalMesh* SkelMesh = Mesh->GetSkeletalMeshAsset())
	{
		for (const FMMMorphValue& M : C.Morphs)
		{
			const TCHAR* MorphName = MorphNameForSlider(M.Slider);
			if (!MorphName)
			{
				continue;
			}
			const int32 Idx = SkelMesh->GetMorphTargetIndexByName(MorphName);
			if (Idx != INDEX_NONE)
			{
				Mesh->SetMorphTarget(Idx, static_cast<float>(M.ValueX1000) / 1000.f);
			}
		}
	}

	// Индексы палитр + цвета одежды -> векторные параметры skin-MID.
	if (UMaterialInstanceDynamic* SkinMid = FindOrLoadSkinMID(Mesh))
	{
		SkinMid->SetVectorParameterValue(TEXT("SkinTint"), PaletteColor(SkinPalette, C.SkinToneIdx));
		SkinMid->SetVectorParameterValue(TEXT("HairTint"), PaletteColor(HairPalette, C.HairColorIdx));
		SkinMid->SetVectorParameterValue(TEXT("EyeTint"), PaletteColor(EyePalette, C.EyeColorIdx));
		SkinMid->SetVectorParameterValue(TEXT("ClothingPrimary"), C.ClothingPrimary.ToLinear());
		SkinMid->SetVectorParameterValue(TEXT("ClothingSecondary"), C.ClothingSecondary.ToLinear());
	}
	return true;
}

// ---------------------------------------------------------------------------
// Internals
// ---------------------------------------------------------------------------

const FFactionVisualRow* UFactionFXManager::FindFactionRow(uint8 FactionId) const
{
	if (!FactionTable)
	{
		return nullptr;
	}
	const TArray<FName> RowNames = FactionTable->GetRowNames();
	for (const FName& RowName : RowNames)
	{
		const FFactionVisualRow* Row = FactionTable->FindRow<FFactionVisualRow>(
			RowName, TEXT("UFactionFXManager::FindFactionRow"), /*bWarnIfMissing=*/false);
		if (Row && Row->FactionId == FactionId)
		{
			return Row;
		}
	}
	return nullptr;
}

UMaterialInstanceDynamic* UFactionFXManager::GetOrCreateVeinsMID(USkeletalMeshComponent* Mesh)
{
	if (UMaterialInstanceDynamic* Existing = FindVeinsMID(Mesh))
	{
		return Existing;
	}
	UMaterialInterface* Base = Cast<UMaterialInterface>(BlackMatterBaseMaterial.TryLoad());
	if (!Base)
	{
		UE_LOG(LogFactionFX, Warning, TEXT("BlackMatter base material not found at %s"),
			*BlackMatterBaseMaterial.ToString());
		return nullptr;
	}
	UMaterialInstanceDynamic* Mid = UMaterialInstanceDynamic::Create(Base, this);
	VeinsMIDTag.Add(Mid.Get()); // поиск по указателю
	VeinsMIDTagArr.Add(Mid);    // держим для GC
	return Mid;
}

UMaterialInstanceDynamic* UFactionFXManager::FindVeinsMID(USkeletalMeshComponent* Mesh) const
{
	// Наши veins-MID опознаются по тегу (не по имени слота — слоты у рас разные).
	for (int32 i = 0; i < Mesh->GetNumMaterials(); ++i)
	{
		UMaterialInterface* Mat = Mesh->GetMaterial(i);
		if (Mat && VeinsMIDTag.Contains(Mat))
		{
			return Cast<UMaterialInstanceDynamic>(Mat);
		}
	}
	return nullptr;
}

float UFactionFXManager::ComputeVeinIntensity(uint8 FactionId, float Corruption01) const
{
	// Круг Поглощения всегда держит минимум 0.35 — «врождённая порча».
	const float Base = (FactionId == 2) ? 0.35f : 0.f;
	return FMath::Clamp(FMath::Max(Base, Corruption01), 0.f, 1.f);
}

void UFactionFXManager::ClearOverrides(USkeletalMeshComponent* Mesh)
{
	for (int32 i = 0; i < Mesh->GetNumMaterials(); ++i)
	{
		UMaterialInterface* Mat = Mesh->GetMaterial(i);
		if (Mat && VeinsMIDTag.Contains(Mat))
		{
			Mesh->RemoveMaterialInstanceOverride(i); // сброс к дефолтному материалу скелетника
			VeinsMIDTag.Remove(Mat);
			VeinsMIDTagArr.Remove(Mat);
		}
	}
}

void UFactionFXManager::TrackMesh(USkeletalMeshComponent* Mesh, uint8 FactionId)
{
	Tracked.Add(TWeakObjectPtr<USkeletalMeshComponent>(Mesh), FactionId);
}

UMaterialInstanceDynamic* UFactionFXManager::FindOrLoadSkinMID(USkeletalMeshComponent* Mesh)
{
	// Один skin-MID на компонент: переиспользуем override нулевого слота.
	const TWeakObjectPtr<USkeletalMeshComponent> Key(Mesh);
	if (TObjectPtr<UMaterialInstanceDynamic>* Cached = SkinMIDCache.Find(Key))
	{
		if (Cached->Get())
		{
			return Cached->Get();
		}
		SkinMIDCache.Remove(Key);
	}
	UMaterialInterface* Base = Mesh->GetMaterial(0);
	if (!Base)
	{
		return nullptr;
	}
	UMaterialInstanceDynamic* Mid = UMaterialInstanceDynamic::Create(Base, this);
	Mesh->SetMaterial(0, Mid);
	SkinMIDCache.Add(Key, Mid);
	return Mid;
}

FLinearColor UFactionFXManager::PaletteColor(const TArray<FLinearColor>& Palette, uint8 Index)
{
	if (Palette.Num() == 0)
	{
		return FLinearColor::White;
	}
	return Palette[Index % Palette.Num()];
}
