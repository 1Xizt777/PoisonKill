#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/PKInteractionTypes.h"
#include "Interaction/PKInteractable.h"
#include "PKProcessingStation.generated.h"

class UStaticMeshComponent;
class UPKGameplayDataSubsystem;
class UPKInventoryComponent;
class UNiagaraComponent;
class UNiagaraSystem;
struct FPKRecipeDefinition;

UCLASS(Blueprintable)
class POISONKILL_API APKProcessingStation : public AActor, public IPKInteractable
{
	GENERATED_BODY()

public:
	APKProcessingStation();

	UFUNCTION(BlueprintPure, Category = "PK|Processing")
	FName GetRecipeId() const { return RecipeId; }

	
	/*Interactble interface Begin*/
	virtual bool CanInteract_Implementation(APawn* Interactor, FPKInteractionRequest& OutRequest) override;
	virtual void OnInteractionStarted_Implementation(APawn* Interactor) override;
	virtual void OnInteractionProgressUpdated_Implementation(APawn* Interactor, float NormalizedProgress) override;
	virtual void OnInteractionCompleted_Implementation(APawn* Interactor) override;
	virtual void OnInteractionCanceled_Implementation(APawn* Interactor) override;
	/*Interactble interface End*/
	
protected:
	
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PK|Processing")
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PK|Processing")
	FName RecipeId = TEXT("Recipe_Hemlock");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PK|Processing")
	FText ProcessingPrompt;

	UFUNCTION(BlueprintImplementableEvent, Category = "PK|Processing")
	void BP_OnProcessingStarted();

	UFUNCTION(BlueprintImplementableEvent, Category = "PK|Processing")
	void BP_OnProcessingProgress(float NormalizedProgress);

	UFUNCTION(BlueprintImplementableEvent, Category = "PK|Processing")
	void BP_OnProcessingCompleted(FName OutputPoisonId);

	UFUNCTION(BlueprintImplementableEvent, Category = "PK|Processing")
	void BP_OnProcessingCanceled();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PK|Processing|VFX")
	TObjectPtr<UNiagaraSystem> ProcessingNiagaraSystem;

private:
	UPROPERTY(Transient)
	TObjectPtr<UNiagaraComponent> ProcessingNiagaraComponent;

	UPKGameplayDataSubsystem* GetDataSubsystem() const;
	UPKInventoryComponent* GetInventoryComponent(APawn* Interactor) const;
	static bool CanApplyRecipe(const FPKRecipeDefinition& Recipe, const UPKInventoryComponent* Inventory);
	static bool ApplyRecipe(const FPKRecipeDefinition& Recipe, UPKInventoryComponent* Inventory);
	void StartProcessingEffect();
	void StopProcessingEffect();

	friend struct FPKProcessingStationTestAccessor;
};
