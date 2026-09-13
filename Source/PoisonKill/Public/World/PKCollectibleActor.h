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
	FPKCollectibleStockChangedSignature OnStockDepleted;

	UFUNCTION(BlueprintPure, Category = "PK|Collectible")
	int32 GetRemainingStock() const;

	virtual bool CanInteract_Implementation(APawn* Interactor,FPKInteractionRequest& OutRequest) override;

	virtual void OnInteractionStarted_Implementation(APawn* Interactor) override;

	virtual void OnInteractionProgressUpdated_Implementation(APawn* Interactor,float NormalizedProgress) override;

	virtual void OnInteractionCompleted_Implementation(APawn* Interactor) override;

	virtual void OnInteractionCanceled_Implementation(APawn* Interactor) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PK|Collectible")
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PK|Collectible")
	FText InteractionPrompt;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PK|Collectible")
	FName MaterialId = TEXT("Item_ToxicPlant");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PK|Collectible", meta = (ClampMin = "0"))
	int32 InitialStock = 3;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PK|Collectible", meta = (ClampMin = "0.0", Units = "s"))
	float HarvestDuration = 2.0f;

private:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "PK|Collectible", meta = (AllowPrivateAccess = "true"))
	int32 RemainingStock = 3;
};
