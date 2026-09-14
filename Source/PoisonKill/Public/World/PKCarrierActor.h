#pragma once

#include "CoreMinimal.h"
#include "Carriers/PKCarrierTypes.h"
#include "GameFramework/Actor.h"
#include "Interaction/PKInteractable.h"
#include "PKCarrierActor.generated.h"

class UStaticMeshComponent;

class USphereComponent;
class UPrimitiveComponent;
struct FHitResult;
class UPKGameplayDataSubsystem;
class UPKInventoryComponent;
class UPKPoisonVictimComponent;
struct FPKCarrierDefinition;
struct FPKPoisonDefinition;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(
	FPKCarrierPayloadChangedSignature,
	FName,
	PoisonId,
	float,
	RemainingDose,
	int32,
	RemainingResidueHits
);

UCLASS(Blueprintable)
class POISONKILL_API APKCarrierActor : public AActor, public IPKInteractable
{
	GENERATED_BODY()

public:
	APKCarrierActor();

	UPROPERTY(BlueprintAssignable, Category = "PK|Carrier")
	FPKCarrierPayloadChangedSignature OnPayloadChanged;

	UPROPERTY(BlueprintAssignable, Category = "PK|Carrier")
	FPKCarrierPayloadChangedSignature OnPayloadCleared;

	UFUNCTION(BlueprintPure, Category = "PK|Carrier")
	FPKCarrierPayload GetPayload() const { return Payload; }

	UFUNCTION(BlueprintPure, Category = "PK|Carrier")
	bool HasPayload() const { return !Payload.IsEmpty(); }

	UFUNCTION(BlueprintCallable, Category = "PK|Carrier")
	void ClearPayload();

	UFUNCTION(BlueprintCallable, Category = "PK|Carrier")
	float ConsumeIngestion(APawn* Consumer);

	UFUNCTION(BlueprintCallable, Category = "PK|Carrier")
	float ApplyContactDose(APawn* Toucher);

	virtual bool CanInteract_Implementation(APawn* Interactor, FPKInteractionRequest& OutRequest) override;
	virtual void OnInteractionStarted_Implementation(APawn* Interactor) override;
	virtual void OnInteractionProgressUpdated_Implementation(APawn* Interactor, float NormalizedProgress) override;
	virtual void OnInteractionCompleted_Implementation(APawn* Interactor) override;
	virtual void OnInteractionCanceled_Implementation(APawn* Interactor) override;

	UFUNCTION(BlueprintCallable, Category = "PK|Carrier")
	bool UseCarrier(APawn* User);
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PK|Carrier")
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PK|Carrier")
	TObjectPtr<USphereComponent> TouchCollision;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PK|Carrier")
	FName CarrierId = TEXT("Carrier_SoupPot");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PK|Carrier")
	FText IngestionPrompt;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PK|Carrier")
	FText ContactPrompt;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "PK|Carrier", meta = (AllowPrivateAccess = "true"))
	FPKCarrierPayload Payload;

	UFUNCTION(BlueprintImplementableEvent, Category = "PK|Carrier")
	void BP_OnPoisonApplied(FName PoisonId, float RemainingDose, int32 RemainingResidueHits);

	UFUNCTION(BlueprintImplementableEvent, Category = "PK|Carrier")
	void BP_OnPoisonCleared();

private:
	UPKGameplayDataSubsystem* GetDataSubsystem() const;
	UPKInventoryComponent* GetInventoryComponent(APawn* Interactor) const;
	UPKPoisonVictimComponent* GetVictimComponent(APawn* Pawn) const;
	bool GetCarrierDefinition(FPKCarrierDefinition& OutDefinition) const;
	bool GetSelectedPoison(APawn* Interactor, FName& OutPoisonId, FPKPoisonDefinition& OutDefinition) const;
	static bool IsPoisonAllowed(const FPKPoisonDefinition& Poison, const FPKCarrierDefinition& Carrier);
	static float CalculateContactDose(const FPKPoisonDefinition& Poison, const FPKCarrierDefinition& Carrier);
	static bool ShouldDetectPoison(const FPKPoisonDefinition& Poison, const FPKCarrierDefinition& Carrier);
	void ApplyPayload(FName PoisonId, float SingleDose, int32 ResidueHits, AActor* AppliedBy);
	UFUNCTION()
	void HandleTouchBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void HandleTouchEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	TSet<TWeakObjectPtr<AActor>> TouchingActors;

	friend struct FPKCarrierActorTestAccessor;
};