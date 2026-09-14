#include "World/PKProcessingStation.h"

#include "Components/StaticMeshComponent.h"
#include "Data/PKGameplayDataSubsystem.h"
#include "Data/PKGameplayTypes.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "Items/PKInventoryComponent.h"

APKProcessingStation::APKProcessingStation()
{
	PrimaryActorTick.bCanEverTick = false;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ProcessingPrompt = NSLOCTEXT("PKProcessingStation", "DefaultProcessingPrompt", "Process {0}");
}

void APKProcessingStation::BeginPlay()
{
	Super::BeginPlay();
}

bool APKProcessingStation::CanInteract_Implementation(APawn* Interactor, FPKInteractionRequest& OutRequest)
{
	if (!IsValid(Interactor) || RecipeId.IsNone())
	{
		return false;
	}

	UPKGameplayDataSubsystem* DataSubsystem = GetDataSubsystem();
	UPKInventoryComponent* Inventory = GetInventoryComponent(Interactor);
	if (!DataSubsystem || !Inventory)
	{
		return false;
	}

	FPKRecipeDefinition Recipe;
	FPKPoisonDefinition Poison;
	if (!DataSubsystem->GetRecipeDefinition(RecipeId, Recipe))
	{
		return false;
	}

	if (!DataSubsystem->GetPoisonDefinition(Recipe.OutputPoisonId, Poison) ||
		!CanApplyRecipe(Recipe, Inventory))
	{
		return false;
	}

	OutRequest.PromptText = FText::Format(ProcessingPrompt, Poison.DisplayName);
	OutRequest.HoldDuration = Recipe.ProcessingDuration;
	OutRequest.bRetainProgressOnRelease = true;
	OutRequest.bLockPlayerInput = true;
	OutRequest.Priority = 10;
	return true;
}

void APKProcessingStation::OnInteractionStarted_Implementation(APawn* Interactor)
{
	BP_OnProcessingStarted();
}

void APKProcessingStation::OnInteractionProgressUpdated_Implementation(APawn* Interactor, float NormalizedProgress)
{
	BP_OnProcessingProgress(FMath::Clamp(NormalizedProgress, 0.0f, 1.0f));
}

void APKProcessingStation::OnInteractionCompleted_Implementation(APawn* Interactor)
{
	UPKGameplayDataSubsystem* DataSubsystem = GetDataSubsystem();
	UPKInventoryComponent* Inventory = GetInventoryComponent(Interactor);
	if (!DataSubsystem || !Inventory)
	{
		return;
	}

	FPKRecipeDefinition Recipe;
	FPKPoisonDefinition Poison;
	if (!DataSubsystem->GetRecipeDefinition(RecipeId, Recipe) ||
		!DataSubsystem->GetPoisonDefinition(Recipe.OutputPoisonId, Poison))
	{
		return;
	}

	if (ApplyRecipe(Recipe, Inventory))
	{
		BP_OnProcessingCompleted(Recipe.OutputPoisonId);
	}
}

void APKProcessingStation::OnInteractionCanceled_Implementation(APawn* Interactor)
{
	BP_OnProcessingCanceled();
}

UPKGameplayDataSubsystem* APKProcessingStation::GetDataSubsystem() const
{
	UGameInstance* GameInstance = GetWorld() ? GetWorld()->GetGameInstance() : nullptr;
	return GameInstance ? GameInstance->GetSubsystem<UPKGameplayDataSubsystem>() : nullptr;
}

UPKInventoryComponent* APKProcessingStation::GetInventoryComponent(APawn* Interactor) const
{
	return IsValid(Interactor) ? Interactor->FindComponentByClass<UPKInventoryComponent>() : nullptr;
}

bool APKProcessingStation::CanApplyRecipe(const FPKRecipeDefinition& Recipe, const UPKInventoryComponent* Inventory)
{
	return Inventory &&
		!Recipe.InputItemId.IsNone() &&
		!Recipe.OutputPoisonId.IsNone() &&
		Recipe.RequiredInputCount > 0 &&
		Inventory->HasMaterial(Recipe.InputItemId, Recipe.RequiredInputCount);
}

bool APKProcessingStation::ApplyRecipe(const FPKRecipeDefinition& Recipe, UPKInventoryComponent* Inventory)
{
	if (!CanApplyRecipe(Recipe, Inventory))
	{
		return false;
	}

	if (!Inventory->RemoveMaterial(Recipe.InputItemId, Recipe.RequiredInputCount))
	{
		return false;
	}

	if (Inventory->AddPoison(Recipe.OutputPoisonId, 1) < 0)
	{
		Inventory->AddMaterial(Recipe.InputItemId, Recipe.RequiredInputCount);
		return false;
	}

	return true;
}
