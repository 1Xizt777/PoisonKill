#pragma once

#include "CoreMinimal.h"
#include "Effects/PKPoisonAttributionTypes.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "PKPoisonAttributionSubsystem.generated.h"

UCLASS()
class POISONKILL_API UPKPoisonAttributionSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "PK|Attribution")
	void RecordPoisonDeath(AActor* Victim, FName PoisonId, AActor* Instigator);

	UFUNCTION(BlueprintCallable, Category = "PK|Attribution")
	void ClearDeathRecords();

	UFUNCTION(BlueprintPure, Category = "PK|Attribution")
	TArray<FPKPoisonDeathRecord> GetPoisonDeathRecords() const { return DeathRecords; }

	UFUNCTION(BlueprintPure, Category = "PK|Attribution")
	int32 GetPoisonKillCount() const { return DeathRecords.Num(); }

	UFUNCTION(BlueprintPure, Category = "PK|Attribution")
	bool WasLastDeathAttributedTo(AActor* Instigator) const;

private:
	UPROPERTY()
	TArray<FPKPoisonDeathRecord> DeathRecords;
};
