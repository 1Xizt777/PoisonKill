#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/TimerHandle.h"
#include "PKPoisonVictimComponent.generated.h"

class UPKGameplayDataSubsystem;
struct FPKNpcDefinition;
struct FPKPoisonDefinition;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(
	FPKPoisonDoseChangedSignature,
	FName,
	PoisonId,
	float,
	TotalDose,
	float,
	ActualLethalThreshold
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FPKPoisonWarningReachedSignature,
	FName,
	PoisonId,
	float,
	TotalDose
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FPKPoisonIncubationStartedSignature,
	FName,
	PoisonId,
	float,
	IncubationTime
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FPKPoisonDeathSignature,
	FName,
	PoisonId,
	AActor*,
	Instigator
);

UCLASS(Blueprintable, ClassGroup = (PK), meta = (BlueprintSpawnableComponent))
class POISONKILL_API UPKPoisonVictimComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPKPoisonVictimComponent();

	UPROPERTY(BlueprintAssignable, Category = "PK|Poison")
	FPKPoisonDoseChangedSignature OnPoisonDoseChanged;

	UPROPERTY(BlueprintAssignable, Category = "PK|Poison")
	FPKPoisonWarningReachedSignature OnPoisonWarningReached;

	UPROPERTY(BlueprintAssignable, Category = "PK|Poison")
	FPKPoisonIncubationStartedSignature OnPoisonIncubationStarted;

	UPROPERTY(BlueprintAssignable, Category = "PK|Poison")
	FPKPoisonDeathSignature OnPoisonDeath;

	UFUNCTION(BlueprintCallable, Category = "PK|Poison")
	void ReceivePoisonDose(FName PoisonId, float Dose, AActor* Instigator);

	UFUNCTION(BlueprintPure, Category = "PK|Poison")
	float GetAccumulatedDose(FName PoisonId) const;

	UFUNCTION(BlueprintPure, Category = "PK|Poison")
	float GetActualLethalThreshold(FName PoisonId) const;

	UFUNCTION(BlueprintPure, Category = "PK|Poison")
	bool IsDead() const { return bDead; }

	UFUNCTION(BlueprintCallable, Category = "PK|Poison")
	void SetNpcId(FName NewNpcId) { NpcId = NewNpcId; }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PK|Poison")
	FName NpcId = TEXT("NPC_Target");

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "PK|Poison", meta = (AllowPrivateAccess = "true"))
	TMap<FName, float> AccumulatedDoseByPoison;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "PK|Poison", meta = (AllowPrivateAccess = "true"))
	bool bDead = false;

	UFUNCTION(BlueprintImplementableEvent, Category = "PK|Poison")
	void BP_OnPoisonDoseReceived(FName PoisonId, float TotalDose, float ActualLethalThreshold);

	UFUNCTION(BlueprintImplementableEvent, Category = "PK|Poison")
	void BP_OnPoisonWarningReached(FName PoisonId, float TotalDose);

	UFUNCTION(BlueprintImplementableEvent, Category = "PK|Poison")
	void BP_OnPoisonIncubationStarted(FName PoisonId, float IncubationTime);

	UFUNCTION(BlueprintImplementableEvent, Category = "PK|Poison")
	void BP_OnPoisonDeath(FName PoisonId, AActor* Instigator);

private:
	TMap<FName, FTimerHandle> IncubationTimers;
	TMap<FName, TWeakObjectPtr<AActor>> PoisonInstigators;
	TSet<FName> WarningTriggeredPoisons;

	bool GetPoisonAndNpc(FName PoisonId, FPKPoisonDefinition& OutPoison, FPKNpcDefinition& OutNpc) const;
	void ApplyPoisonDose(FName PoisonId, float Dose, AActor* Instigator, const FPKPoisonDefinition& Poison, const FPKNpcDefinition& Npc);
	void HandleIncubationExpired(FName PoisonId);
	void KillFromPoison(FName PoisonId, AActor* Instigator);

	static float CalculateActualLethalThreshold(const FPKPoisonDefinition& Poison, const FPKNpcDefinition& Npc);
	static float CalculateWarningThreshold(const FPKPoisonDefinition& Poison, const FPKNpcDefinition& Npc);
	static bool HasReachedWarningThreshold(float TotalDose, const FPKPoisonDefinition& Poison, const FPKNpcDefinition& Npc);
	static bool HasReachedLethalThreshold(float TotalDose, const FPKPoisonDefinition& Poison, const FPKNpcDefinition& Npc);

	friend struct FPKPoisonVictimTestAccessor;
};
