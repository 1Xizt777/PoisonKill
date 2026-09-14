#include "World/PKCarrierActor.h"

#include "Components/StaticMeshComponent.h"
#include "Data/PKGameplayDataSubsystem.h"
#include "Data/PKGameplayTypes.h"
#include "Effects/PKPoisonVictimComponent.h"
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

float APKCarrierActor::ConsumeIngestion(APawn* Consumer)
{
	UPKPoisonVictimComponent* Victim = GetVictimComponent(Consumer);
	FPKCarrierDefinition Carrier;
	FPKPoisonDefinition Poison;
	if (!Victim ||
		Payload.IsEmpty() ||
		!GetCarrierDefinition(Carrier) ||
		!GetDataSubsystem()->GetPoisonDefinition(Payload.PoisonId, Poison) ||
		Carrier.CarrierType != EPKCarrierType::Ingestion)
	{
		return 0.0f;
	}

	const float Dose = Payload.RemainingDose;
	Victim->ReceivePoisonDose(Payload.PoisonId, Dose, Payload.Instigator);
	ClearPayload();
	return Dose;
}

float APKCarrierActor::ApplyContactDose(APawn* Toucher)
{
	UPKPoisonVictimComponent* Victim = GetVictimComponent(Toucher);
	FPKCarrierDefinition Carrier;
	FPKPoisonDefinition Poison;
	if (!Victim ||
		Payload.IsEmpty() ||
		Payload.RemainingResidueHits <= 0 ||
		!GetCarrierDefinition(Carrier) ||
		!GetDataSubsystem()->GetPoisonDefinition(Payload.PoisonId, Poison) ||
		Carrier.CarrierType != EPKCarrierType::Contact)
	{
		return 0.0f;
	}

	const float Dose = CalculateContactDose(Poison, Carrier);
	if (Dose <= 0.0f)
	{
		return 0.0f;
	}

	Payload.RemainingDose = FMath::Max(0.0f, Payload.RemainingDose - Dose);
	--Payload.RemainingResidueHits;
	Victim->ReceivePoisonDose(Payload.PoisonId, Dose, Payload.Instigator);

	if (Payload.RemainingResidueHits <= 0 || Payload.RemainingDose <= 0.0f)
	{
		ClearPayload();
	}
	else
	{
		OnPayloadChanged.Broadcast(Payload.PoisonId, Payload.RemainingDose, Payload.RemainingResidueHits);
	}

	return Dose;
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
		Carrier.CarrierType == EPKCarrierType::Ingestion ? IngestionPrompt : ContactPrompt,
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

	ApplyPayload(PoisonId, Poison.SingleDose, Carrier.DefaultResidueHits, Interactor);
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

UPKPoisonVictimComponent* APKCarrierActor::GetVictimComponent(APawn* Pawn) const
{
	return IsValid(Pawn) ? Pawn->FindComponentByClass<UPKPoisonVictimComponent>() : nullptr;
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

float APKCarrierActor::CalculateContactDose(const FPKPoisonDefinition& Poison, const FPKCarrierDefinition& Carrier)
{
	return Poison.SingleDose * Carrier.DoseCoefficient;
}

void APKCarrierActor::ApplyPayload(FName PoisonId, float SingleDose, int32 ResidueHits, AActor* AppliedBy)
{
	const bool bSamePoison = Payload.PoisonId == PoisonId;
	Payload.PoisonId = PoisonId;
	Payload.RemainingDose = bSamePoison ? Payload.RemainingDose + SingleDose : SingleDose;
	Payload.RemainingResidueHits = FMath::Max(0, ResidueHits);
	Payload.Instigator = AppliedBy;
	OnPayloadChanged.Broadcast(PoisonId, Payload.RemainingDose, Payload.RemainingResidueHits);
	BP_OnPoisonApplied(PoisonId, Payload.RemainingDose, Payload.RemainingResidueHits);
}