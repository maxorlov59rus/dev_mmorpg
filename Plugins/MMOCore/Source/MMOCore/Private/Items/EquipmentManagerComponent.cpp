// EquipmentManagerComponent.cpp — логика применения VFX-«подложек» при экипировке.
#include "Items/EquipmentManagerComponent.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/AssetManager.h"
#include "Engine/SkeletalMesh.h"

DEFINE_LOG_CATEGORY_STATIC(LogMMOEquip, Log, All);

UEquipmentManagerComponent::UEquipmentManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UEquipmentManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	// Fallback-бинд, если Blueprint не выставил ссылки явно.
	if (!WeaponMeshComp)
	{
		WeaponMeshComp = GetOwner() ? GetOwner()->FindComponentByClass<USkeletalMeshComponent>() : nullptr;
	}
}

void UEquipmentManagerComponent::EndPlay(const EEndPlayReason::Type Reason)
{
	ClearEnchantEffects();
	Super::EndPlay(Reason);
}

UNiagaraSystem* UEquipmentManagerComponent::ResolveEnchantSystem(const EEnchantElement Element) const
{
	const TSoftObjectPtr<UNiagaraSystem>* Soft = nullptr;
	switch (Element)
	{
	case EEnchantElement::Fire:  Soft = &NS_Enchant_Fire;  break;
	case EEnchantElement::Ice:   Soft = &NS_Enchant_Ice;   break;
	case EEnchantElement::Holy:  Soft = &NS_Enchant_Holy;  break;
	case EEnchantElement::Shade: Soft = &NS_Enchant_Shade; break;
	case EEnchantElement::Storm: Soft = &NS_Enchant_Storm; break;
	default: return nullptr;
	}

	if (Soft->IsValid())
	{
		return Soft->LoadSynchronous();
	}
	UE_LOG(LogMMOEquip, Warning, TEXT("Enchant NS not assigned for element %d"), static_cast<int32>(Element));
	return nullptr;
}

void UEquipmentManagerComponent::AttachEnchant(USkeletalMeshComponent* MeshComp, UNiagaraSystem* System)
{
	if (!MeshComp || !System || !MeshComp->DoesSocketExist(WeaponSocketName))
	{
		return;
	}

	UNiagaraComponent* NC = UNiagaraFunctionLibrary::SpawnSystemAttached(
		System,
		MeshComp,
		WeaponSocketName,
		FVector::ZeroVector,
		FRotator::ZeroRotator,
		EAttachLocation::SnapToTarget,
		true); // bAutoDestroy = true: эффект живёт вместе с сокетом

	if (NC)
	{
		NC->SetOnlyOwnerSee(false);
		NC->Activate(true);
	}
}

void UEquipmentManagerComponent::ApplyEquipUpdate(
	uint32 MainhandDisplayId, uint32 OffhandDisplayId, uint8 MainhandQuality, uint8 EnchantElementHint)
{
	Mainhand.DisplayId = MainhandDisplayId;
	Offhand.DisplayId = OffhandDisplayId;
	Mainhand.Rarity = static_cast<EItemRarity>(FMath::Max(0, MainhandQuality - 1));

	ClearEnchantEffects();

	// Правила из ТЗ: Niagara только для Quality >= Epic и наличием зачарования (EnchantID > 0).
	const bool bWantsEnchant = MainhandQuality >= MinQualityForEnchant && EnchantElementHint > 0;
	if (bWantsEnchant)
	{
		if (UNiagaraSystem* Sys = ResolveEnchantSystem(static_cast<EEnchantElement>(EnchantElementHint)))
		{
			AttachEnchant(WeaponMeshComp, Sys);
			Mainhand.ActiveEnchant = nullptr; // авто-уничтожение по байте; ссылка не нужна для lifetime
		}
	}

	// MI_Item_FactionColor: перекраска меша в цвет фракции владельца (параметр FactionColor).
	if (FactionTintMaterial && WeaponMeshComp)
	{
		UMaterialInstanceDynamic* MID = WeaponMeshComp->CreateAndSetMaterialInstanceDynamic(FactionTintMaterial);
		// Игрок-специфичный цвет задаётся externally через SetVectorParameterValue в BP-хелпере.
		(void)MID;
	}
}

void UEquipmentManagerComponent::ClearEnchantEffects()
{
	if (AActor* Owner = GetOwner())
	{
		TArray<UNiagaraComponent*> Comps;
		Owner->GetComponents<UNiagaraComponent>(Comps);
		for (UNiagaraComponent* NC : Comps)
		{
			if (NC && NC->IsAttachedTo(WeaponMeshComp))
			{
				NC->DeactivateSystem();
				NC->DestroyComponent();
			}
		}
	}
	Mainhand.ActiveEnchant = nullptr;
	Offhand.ActiveEnchant = nullptr;
}

void UEquipmentManagerComponent::PreviewWeaponMeshes(FName WeaponMeshPath, FName OffhandMeshPath)
{
	auto ApplySoft = [](USkeletalMeshComponent* Comp, FName Path)
	{
		if (!Comp || Path.IsNone())
		{
			return;
		}
		const FSoftObjectPtr Soft(Path.ToString());
		if (USkeletalMesh* Mesh = Cast<USkeletalMesh>(Soft.ToObject()))
		{
			Comp->SetSkeletalMesh(Mesh, /*bRecreateState*/ true);
		}
	};
	ApplySoft(WeaponMeshComp, WeaponMeshPath);
	ApplySoft(OffhandMeshComp, OffhandMeshPath);
}
