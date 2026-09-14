#include "Effects/PKPoisonVictimComponent.h"

#include "Data/PKGameplayDataSubsystem.h"
#include "Data/PKGameplayTypes.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"

UPKPoisonVictimComponent::UPKPoisonVictimComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPKPoisonVictimComponent::ReceivePoisonDose(FName PoisonId, float Dose, AActor* Instigator)
{
	if (bDead || PoisonId.IsNone() || Dose <= 0.0f)
	{
		return;
	}

	FPKPoisonDefinition Poison;
	FPKNpcDefinition Npc;
	if (!GetPoisonAndNpc(PoisonId, Poison, Npc))
	{
		return;
	}

	ApplyPoisonDose(PoisonId, Dose, Instigator, Poison, Npc);
}

float UPKPoisonVictimComponent::GetAccumulatedDose(FName PoisonId) const
{
	const float* Dose = AccumulatedDoseByPoison.Find(PoisonId);
	return Dose ? *Dose : 0.0f;
}

float UPKPoisonVictimComponent::GetActualLethalThreshold(FName PoisonId) const
{
	FPKPoisonDefinition Poison;
	FPKNpcDefinition Npc;
	if (!GetPoisonAndNpc(PoisonId, Poison, Npc))
	{
		return 0.0f;
	}

	return CalculateActualLethalThreshold(Poison, Npc);
}

bool UPKPoisonVictimComponent::GetPoisonAndNpc(FName PoisonId, FPKPoisonDefinition& OutPoison, FPKNpcDefinition& OutNpc) const
{
	UGameInstance* GameInstance = GetWorld() ? GetWorld()->GetGameInstance() : nullptr;
	UPKGameplayDataSubsystem* DataSubsystem = GameInstance ? GameInstance->GetSubsystem<UPKGameplayDataSubsystem>() : nullptr;
	return DataSubsystem &&
		DataSubsystem->GetPoisonDefinition(PoisonId, OutPoison) &&
		DataSubsystem->GetNpcDefinition(NpcId, OutNpc);
}

void UPKPoisonVictimComponent::ApplyPoisonDose(FName PoisonId, float Dose, AActor* Instigator, const FPKPoisonDefinition& Poison, const FPKNpcDefinition& Npc)
{
	float& TotalDose = AccumulatedDoseByPoison.FindOrAdd(PoisonId);
	TotalDose += Dose;
	PoisonInstigators.Add(PoisonId, Instigator);

	const float ActualLethalThreshold = CalculateActualLethalThreshold(Poison, Npc);
	OnPoisonDoseChanged.Broadcast(PoisonId, TotalDose, ActualLethalThreshold);
	BP_OnPoisonDoseReceived(PoisonId, TotalDose, ActualLethalThreshold);

	if (!WarningTriggeredPoisons.Contains(PoisonId) &&
		HasReachedWarningThreshold(TotalDose, Poison, Npc))
	{
		WarningTriggeredPoisons.Add(PoisonId);
		OnPoisonWarningReached.Broadcast(PoisonId, TotalDose);
		BP_OnPoisonWarningReached(PoisonId, TotalDose);
	}

	if (!HasReachedLethalThreshold(TotalDose, Poison, Npc))
	{
		return;
	}

	if (Poison.IncubationTime <= KINDA_SMALL_NUMBER)
	{
		KillFromPoison(PoisonId, Instigator);
		return;
	}

	if (FTimerHandle* ExistingTimer = IncubationTimers.Find(PoisonId))
	{
		if (GetWorld()->GetTimerManager().IsTimerActive(*ExistingTimer))
		{
			return;
		}
	}

	FTimerHandle NewTimer;
	GetWorld()->GetTimerManager().SetTimer(
		NewTimer,
		FTimerDelegate::CreateUObject(
			this,
			&ThisClass::HandleIncubationExpired,
			PoisonId
		),
		Poison.IncubationTime,
		false
	);
	IncubationTimers.Add(PoisonId, NewTimer);
	OnPoisonIncubationStarted.Broadcast(PoisonId, Poison.IncubationTime);
	BP_OnPoisonIncubationStarted(PoisonId, Poison.IncubationTime);
}

void UPKPoisonVictimComponent::HandleIncubationExpired(FName PoisonId)
{
	if (bDead)
	{
		return;
	}

	AActor* Instigator = nullptr;
	if (TWeakObjectPtr<AActor>* InstigatorRef = PoisonInstigators.Find(PoisonId))
	{
		Instigator = InstigatorRef->Get();
	}

	KillFromPoison(PoisonId, Instigator);
}

void UPKPoisonVictimComponent::KillFromPoison(FName PoisonId, AActor* Instigator)
{
	if (bDead)
	{
		return;
	}

	bDead = true;
	for (TPair<FName, FTimerHandle>& TimerPair : IncubationTimers)
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerPair.Value);
	}

	OnPoisonDeath.Broadcast(PoisonId, Instigator);
	BP_OnPoisonDeath(PoisonId, Instigator);
}

float UPKPoisonVictimComponent::CalculateActualLethalThreshold(const FPKPoisonDefinition& Poison, const FPKNpcDefinition& Npc)
{
	return Poison.LethalThreshold * Npc.ThresholdCoefficient;
}

float UPKPoisonVictimComponent::CalculateWarningThreshold(const FPKPoisonDefinition& Poison, const FPKNpcDefinition& Npc)
{
	return CalculateActualLethalThreshold(Poison, Npc) * Npc.WarningThresholdRatio;
}

bool UPKPoisonVictimComponent::HasReachedWarningThreshold(float TotalDose, const FPKPoisonDefinition& Poison, const FPKNpcDefinition& Npc)
{
	return TotalDose >= CalculateWarningThreshold(Poison, Npc);
}

bool UPKPoisonVictimComponent::HasReachedLethalThreshold(float TotalDose, const FPKPoisonDefinition& Poison, const FPKNpcDefinition& Npc)
{
	return TotalDose >= CalculateActualLethalThreshold(Poison, Npc);
}
