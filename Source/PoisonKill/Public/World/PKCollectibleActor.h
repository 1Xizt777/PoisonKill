#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/PKInteractable.h"
#include "PKCollectibleActor.generated.h"

class UStaticMeshComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPKCollectibleStockChangedSignature , int32,RemainingStock);

UCLASS(Blueprintable)
class POISONKILL_API APKCollectibleActor : public AActor, public IPKInteractable
{
	GENERATED_BODY()

public:
	APKCollectibleActor();

	UPROPERTY(BlueprintAssignable, Category = "PK|Collectible")
	FPKCollectibleStockChangedSignature OnStockChanged;

	UPROPERTY(BlueprintAssignable, Category = "PK|Collectible")
	FPKCollectibleStockChangedSignature OnStockDepleted;	//耗尽

	UFUNCTION(BlueprintPure, Category = "PK|Collectible")
	int32 GetRemainingStock() const;

	
	/*Interactble interface Begin*/
	virtual bool CanInteract_Implementation(APawn* Interactor,FPKInteractionRequest& OutRequest) override;
	virtual void OnInteractionStarted_Implementation(APawn* Interactor) override;
	virtual void OnInteractionProgressUpdated_Implementation(APawn* Interactor,float NormalizedProgress) override;
	virtual void OnInteractionCompleted_Implementation(APawn* Interactor) override;
	virtual void OnInteractionCanceled_Implementation(APawn* Interactor) override;
	/*Interactble interface End*/
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PK|Collectible")
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PK|Collectible")
	FText InteractionPrompt;	//交互提示文本

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PK|Collectible")
	FName MaterialId = TEXT("Item_ToxicPlant");		//原料ID：毒芹

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PK|Collectible", meta = (ClampMin = "0"))
	int32 InitialStock = 3;		//采集一次数量

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PK|Collectible", meta = (ClampMin = "0.0", Units = "s"))
	float HarvestDuration = 2.0f;		//采集所需时间

private:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "PK|Collectible", meta = (AllowPrivateAccess = "true"))
	int32 RemainingStock = 3;		//所剩数量
};
