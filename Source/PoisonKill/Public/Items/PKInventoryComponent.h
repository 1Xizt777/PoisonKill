#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PKInventoryComponent.generated.h"

UENUM(BlueprintType)
enum class EPKInventoryItemCategory : uint8
{
	Material UMETA(DisplayName = "Material"),
	Poison UMETA(DisplayName = "Poison"),
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(
	FPKInventoryChangedSignature,
	FName,
	ItemId,
	int32,
	NewCount,
	EPKInventoryItemCategory,
	Category
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FPKSelectedPoisonChangedSignature,
	FName,
	PoisonId
);

UCLASS(ClassGroup = (PK), meta = (BlueprintSpawnableComponent))
class POISONKILL_API UPKInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPKInventoryComponent();

	UPROPERTY(BlueprintAssignable, Category = "PK|Inventory")
	FPKInventoryChangedSignature OnInventoryChanged;

	UPROPERTY(BlueprintAssignable, Category = "PK|Inventory")
	FPKSelectedPoisonChangedSignature OnSelectedPoisonChanged;

	UFUNCTION(BlueprintCallable, Category = "PK|Inventory|Material")
	int32 AddMaterial(FName MaterialId, int32 Amount = 1);

	UFUNCTION(BlueprintCallable, Category = "PK|Inventory|Material")
	bool RemoveMaterial(FName MaterialId, int32 Amount = 1);

	UFUNCTION(BlueprintPure, Category = "PK|Inventory|Material")
	bool HasMaterial(FName MaterialId, int32 Amount = 1) const;

	UFUNCTION(BlueprintPure, Category = "PK|Inventory|Material")
	int32 GetMaterialCount(FName MaterialId) const;
	const TMap<FName, int32>& GetMaterials() const;

	UFUNCTION(BlueprintCallable, Category = "PK|Inventory|Poison")
	int32 AddPoison(FName PoisonId, int32 Amount = 1);

	UFUNCTION(BlueprintCallable, Category = "PK|Inventory|Poison")
	bool RemovePoison(FName PoisonId, int32 Amount = 1);

	UFUNCTION(BlueprintPure, Category = "PK|Inventory|Poison")
	bool HasPoison(FName PoisonId, int32 Amount = 1) const;

	UFUNCTION(BlueprintPure, Category = "PK|Inventory|Poison")
	int32 GetPoisonCount(FName PoisonId) const;
	const TMap<FName, int32>& GetPoisons() const;

	UFUNCTION(BlueprintCallable, Category = "PK|Inventory|Poison")
	bool SetSelectedPoison(FName PoisonId);

	UFUNCTION(BlueprintCallable, Category = "PK|Inventory|Poison")
	void ClearSelectedPoison();

	UFUNCTION(BlueprintPure, Category = "PK|Inventory|Poison")
	FName GetSelectedPoisonId() const;

	UFUNCTION(BlueprintPure, Category = "PK|Inventory|Poison")
	int32 GetSelectedPoisonCount() const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PK|Inventory")
	TMap<FName, int32> InitialMaterials;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PK|Inventory")
	TMap<FName, int32> InitialPoisons;

	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "PK|Inventory", meta = (AllowPrivateAccess = "true"))
	TMap<FName, int32> Materials;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "PK|Inventory", meta = (AllowPrivateAccess = "true"))
	TMap<FName, int32> Poisons;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "PK|Inventory", meta = (AllowPrivateAccess = "true"))
	FName SelectedPoisonId;

	int32 AddToMap(TMap<FName, int32>& Map, FName ItemId, int32 Amount);
	bool RemoveFromMap(TMap<FName, int32>& Map, FName ItemId, int32 Amount);
};
