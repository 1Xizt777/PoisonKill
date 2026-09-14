#include "World/PKCarrierActor.h"

#include "Components/StaticMeshComponent.h"
#include "Data/PKGameplayDataSubsystem.h"
#include "Data/PKGameplayTypes.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "Items/PKInventoryComponent.h"

APKCarrierActor::APKCarrierActor()
{
	PrimaryActorTick.bCanEverTick = false;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	IngestionPrompt = NSLOCTEXT("PKCarrierActor", "DefaultIngestionPrompt", "Add {0}");
	ContactPrompt = NSLOCTEXT("PKCarrierActor", "DefaultContactPrompt", "Apply {0}");
}

void APKCarrierActor::ClearPayload()
{
	if (Payload.IsEmpty())
	{
		return;
	}

	const FName PreviousPoisonId = Payload.PoisonId;
	Payload = FPKCarrierPayload();
	OnPayloadCleared.Broadcast(PreviousPoisonId, 0.0f, 0);
	BP_OnPoisonCleared();
}

bool APKCarrierActor::CanInteract_Implementation(APawn* Interactor, FPKInteractionRequest& OutRequest)
{
	FPKCarrierDefinition Carrier;
	FPKPoisonDefinition Poison;
	FName PoisonId;

	if (!GetCarrierDefinition(Carrier) ||
		!GetSelectedPoison(Interactor, PoisonId, Poison) ||
		!IsPoisonAllowed(Poison, Carrier))
	{
		return false;
	}

	OutRequest.PromptText = FText::Format(
		Carrier.CarrierType == EPKCarrierType::Ingestion
			? IngestionPrompt
			: ContactPrompt,
		Poison.DisplayName
	);
	OutRequest.HoldDuration = 2.0f;
	OutRequest.bRetainProgressOnRelease = false;
	OutRequest.bLockPlayerInput = true;
	OutRequest.Priority = 20;
	return true;
}

void APKCarrierActor::OnInteractionStarted_Implementation(APawn* Interactor)
{
}

void APKCarrierActor::OnInteractionProgressUpdated_Implementation(APawn* Interactor, float NormalizedProgress)
{
}

void APKCarrierActor::OnInteractionCompleted_Implementation(APawn* Interactor)
{
	UPKInventoryComponent* Inventory = GetInventoryComponent(Interactor);
	FPKCarrierDefinition Carrier;
	FPKPoisonDefinition Poison;
	FName PoisonId;

	if (!Inventory ||
		!GetCarrierDefinition(Carrier) ||
		!GetSelectedPoison(Interactor, PoisonId, Poison) ||
		!IsPoisonAllowed(Poison, Carrier) ||
		!Inventory->RemovePoison(PoisonId, 1))
	{
		return;
	}

	ApplyPayload(PoisonId, Poison.SingleDose, Carrier.DefaultResidueHits);
}

void APKCarrierActor::OnInteractionCanceled_Implementation(APawn* Interactor)
{
}

UPKGameplayDataSubsystem* APKCarrierActor::GetDataSubsystem() const
{
	UGameInstance* GameInstance = GetWorld() ? GetWorld()->GetGameInstance() : nullptr;
	return GameInstance ? GameInstance->GetSubsystem<UPKGameplayDataSubsystem>() : nullptr;
}

UPKInventoryComponent* APKCarrierActor::GetInventoryComponent(APawn* Interactor) const
{
	return IsValid(Interactor) ? Interactor->FindComponentByClass<UPKInventoryComponent>() : nullptr;
}

bool APKCarrierActor::GetCarrierDefinition(FPKCarrierDefinition& OutDefinition) const
{
	UPKGameplayDataSubsystem* DataSubsystem = GetDataSubsystem();
	return DataSubsystem && DataSubsystem->GetCarrierDefinition(CarrierId, OutDefinition);
}

bool APKCarrierActor::GetSelectedPoison(APawn* Interactor, FName& OutPoisonId, FPKPoisonDefinition& OutDefinition) const
{
	UPKInventoryComponent* Inventory = GetInventoryComponent(Interactor);
	UPKGameplayDataSubsystem* DataSubsystem = GetDataSubsystem();
	if (!Inventory || !DataSubsystem)
	{
		return false;
	}

	OutPoisonId = Inventory->GetSelectedPoisonId();
	return !OutPoisonId.IsNone() &&
		Inventory->HasPoison(OutPoisonId, 1) &&
		DataSubsystem->GetPoisonDefinition(OutPoisonId, OutDefinition);
}

bool APKCarrierActor::IsPoisonAllowed(const FPKPoisonDefinition& Poison, const FPKCarrierDefinition& Carrier)
{
	return Poison.AllowedCarrierTypes.Contains(Carrier.CarrierType);
}

void APKCarrierActor::ApplyPayload(FName PoisonId, float SingleDose, int32 ResidueHits)
{
	const bool bSamePoison = Payload.PoisonId == PoisonId;
	Payload.PoisonId = PoisonId;
	Payload.RemainingDose = bSamePoison
		? Payload.RemainingDose + SingleDose
		: SingleDose;
	Payload.RemainingResidueHits = FMath::Max(0, ResidueHits);
	OnPayloadChanged.Broadcast(PoisonId, Payload.RemainingDose, Payload.RemainingResidueHits);
	BP_OnPoisonApplied(PoisonId, Payload.RemainingDose, Payload.RemainingResidueHits);
}
