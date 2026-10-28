// EquipmentManagerComponent.h — клиентский компонент экипировки (UE 5.8).
// Получает SMSG_ITEM_EQUIP_UPDATE (0x2037) от MMOPlayerController, подменяет
// SkeletalMesh по displayid (mapping_displayid.csv) и навешивает Niagara-«подложки»
// (NS_Enchant_*) при Quality >= Epic && EnchantId > 0.
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EquipmentManagerComponent.generated.h"

class UNiagaraComponent;
class UNiagaraSystem;
class USkeletalMeshComponent;
class UMaterialInstanceDynamic;

/** Редкость предмета — зеркало item_template.quality (0..5). */
UENUM(BlueprintType)
enum class EItemRarity : uint8
{
	Common UMETA(DisplayName = "Common"),
	Uncommon UMETA(DisplayName = "Uncommon"),
	Rare UMETA(DisplayName = "Rare"),
	Epic UMETA(DisplayName = "Epic"),
	Legendary UMETA(DisplayName = "Legendary")
};

/** Элемент зачарования -> NS_Enchant_* (контракт с dm_items.enchant_effects). */
UENUM(BlueprintType)
enum class EEnchantElement : uint8
{
	None = 0 UMETA(DisplayName = "None"),
	Fire = 1 UMETA(DisplayName = "Fire"),
	Ice = 2 UMETA(DisplayName = "Ice"),
	Holy = 3 UMETA(DisplayName = "Holy"),
	Shade = 4 UMETA(DisplayName = "Shade"),
	Storm = 5 UMETA(DisplayName = "Storm")
};

USTRUCT(BlueprintType)
struct FEquipSlotVisual
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	uint32 ItemEntry = 0;

	UPROPERTY(BlueprintReadOnly)
	uint32 DisplayId = 0;

	UPROPERTY(BlueprintReadOnly)
	EItemRarity Rarity = EItemRarity::Common;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UNiagaraComponent> ActiveEnchant = nullptr;
};

UCLASS(ClassGroup = (MMO), meta = (BlueprintSpawnableComponent))
class MMOCORE_API UEquipmentManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UEquipmentManagerComponent();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type Reason) override;

	/** Вызывается из AMMOPlayerController::HandleServerPacket(SrvItemEquipUpdate). */
	UFUNCTION(BlueprintCallable, Category = "MMO|Equipment")
	void ApplyEquipUpdate(uint32 MainhandDisplayId, uint32 OffhandDisplayId, uint8 MainhandQuality, uint8 EnchantElementHint);

	/** Снять все визуальные эффекты (unequip / смена оружия). */
	UFUNCTION(BlueprintCallable, Category = "MMO|Equipment")
	void ClearEnchantEffects();

	/** Превью в модалке выбора специализации (UI_SpecSelectionWidget::K2_OnPreviewEquipChanged). */
	UFUNCTION(BlueprintCallable, Category = "MMO|Equipment")
	void PreviewWeaponMeshes(FName WeaponMeshPath, FName OffhandMeshPath);

protected:
	/** Компоненты персонажа. Биндятся в Blueprint (Owner->GetComponentByClass как fallback). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MMO|Equipment")
	TObjectPtr<USkeletalMeshComponent> WeaponMeshComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MMO|Equipment")
	TObjectPtr<USkeletalMeshComponent> OffhandMeshComp;

	/** Сокет для прикрепления Niagra-подложек. */
	UPROPERTY(EditAnywhere, Category = "MMO|Equipment")
	FName WeaponSocketName = TEXT("weapon_socket");

	/** Реестр «подложек»: вместо 1200 уникальных VFX — 5 систем. */
	UPROPERTY(EditAnywhere, Category = "MMO|Equipment|VFX")
	TSoftObjectPtr<UNiagaraSystem> NS_Enchant_Fire;

	UPROPERTY(EditAnywhere, Category = "MMO|Equipment|VFX")
	TSoftObjectPtr<UNiagaraSystem> NS_Enchant_Ice;

	UPROPERTY(EditAnywhere, Category = "MMO|Equipment|VFX")
	TSoftObjectPtr<UNiagaraSystem> NS_Enchant_Holy;

	UPROPERTY(EditAnywhere, Category = "MMO|Equipment|VFX")
	TSoftObjectPtr<UNiagaraSystem> NS_Enchant_Shade;

	UPROPERTY(EditAnywhere, Category = "MMO|Equipment|VFX")
	TSoftObjectPtr<UNiagaraSystem> NS_Enchant_Storm;

	/** Материал с параметром цвета фракции (MI_Item_FactionColor). */
	UPROPERTY(EditAnywhere, Category = "MMO|Equipment|VFX")
	TObjectPtr<UMaterialInterface> FactionTintMaterial;

private:
	UNiagaraSystem* ResolveEnchantSystem(EEnchantElement Element) const;
	void AttachEnchant(USkeletalMeshComponent* MeshComp, UNiagaraSystem* System);

	FEquipSlotVisual Mainhand;
	FEquipSlotVisual Offhand;

	/** Порог включения эффектов: Quality >= Epic (5). */
	static constexpr uint8 MinQualityForEnchant = 5;
};
