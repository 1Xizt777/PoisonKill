#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PKInventoryComponent.generated.h"

UENUM(BlueprintType)
enum class EPKInventoryItemCategory : uint8
{
	Material UMETA(DisplayName = "Material"),		//原料
	Poison UMETA(DisplayName = "Poison"),		//毒物
};

//当背包改变时
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FPKInventoryChangedSignature , FName , ItemId , int32 , NewCount , EPKInventoryItemCategory , Category);


//当所拿毒物改变时
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPKSelectedPoisonChangedSignature , FName , PoisonId);


UCLASS(ClassGroup = (PK), meta = (BlueprintSpawnableComponent))
class POISONKILL_API UPKInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPKInventoryComponent();

	//实例化两个委托
	UPROPERTY(BlueprintAssignable, Category = "PK|Inventory")
	FPKInventoryChangedSignature OnInventoryChanged;

	UPROPERTY(BlueprintAssignable, Category = "PK|Inventory")
	FPKSelectedPoisonChangedSignature OnSelectedPoisonChanged;

	
	//----原料API----//
	UFUNCTION(BlueprintCallable, Category = "PK|Inventory|Material")
	int32 AddMaterial(FName MaterialId, int32 Amount = 1);

	UFUNCTION(BlueprintCallable, Category = "PK|Inventory|Material")
	bool RemoveMaterial(FName MaterialId, int32 Amount = 1);

	UFUNCTION(BlueprintPure, Category = "PK|Inventory|Material")
	bool HasMaterial(FName MaterialId, int32 Amount = 1) const;

	UFUNCTION(BlueprintPure, Category = "PK|Inventory|Material")
	int32 GetMaterialCount(FName MaterialId) const;
	
	const TMap<FName, int32>& GetMaterials() const;		//返回实际材料背包
	//----原料API End----//
	
	
	
	//----毒物API----//
	UFUNCTION(BlueprintCallable, Category = "PK|Inventory|Poison")
	int32 AddPoison(FName PoisonId, int32 Amount = 1);

	UFUNCTION(BlueprintCallable, Category = "PK|Inventory|Poison")
	bool RemovePoison(FName PoisonId, int32 Amount = 1);

	UFUNCTION(BlueprintPure, Category = "PK|Inventory|Poison")
	bool HasPoison(FName PoisonId, int32 Amount = 1) const;

	UFUNCTION(BlueprintPure, Category = "PK|Inventory|Poison")
	int32 GetPoisonCount(FName PoisonId) const;
	
	const TMap<FName, int32>& GetPoisons() const;		//返回实际毒物背包
	//----毒物API End----//
	
	
	
	//----所持毒物API----//
	UFUNCTION(BlueprintCallable, Category = "PK|Inventory|Poison")
	bool SetSelectedPoison(FName PoisonId);

	UFUNCTION(BlueprintCallable, Category = "PK|Inventory|Poison")
	void ClearSelectedPoison();

	UFUNCTION(BlueprintPure, Category = "PK|Inventory|Poison")
	FName GetSelectedPoisonId() const;

	UFUNCTION(BlueprintPure, Category = "PK|Inventory|Poison")
	int32 GetSelectedPoisonCount() const;
	//----所持毒物API End----//
	
protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PK|Inventory")	//出生自带的材料
	TMap<FName, int32> InitialMaterials;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PK|Inventory")	//出生自带的毒物
	TMap<FName, int32> InitialPoisons;

	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "PK|Inventory", meta = (AllowPrivateAccess = "true"))
	TMap<FName, int32> Materials;		//实际上的原料背包

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "PK|Inventory", meta = (AllowPrivateAccess = "true"))
	TMap<FName, int32> Poisons;			//实际上的毒物背包

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "PK|Inventory", meta = (AllowPrivateAccess = "true"))
	FName SelectedPoisonId;		//当前所持毒物ID

	
	//工具函数，加减数量
	int32 AddToMap(TMap<FName, int32>& Map, FName ItemId, int32 Amount);	
	bool RemoveFromMap(TMap<FName, int32>& Map, FName ItemId, int32 Amount);
};
