#pragma once

#include "CoreMinimal.h"
#include "PKPoisonAttributionTypes.generated.h"

class AActor;
class APlayerState;

USTRUCT(BlueprintType)
struct POISONKILL_API FPKPoisonDeathRecord
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "PK|Attribution")
	TObjectPtr<AActor> Victim;

	UPROPERTY(BlueprintReadOnly, Category = "PK|Attribution")
	FName PoisonId;

	UPROPERTY(BlueprintReadOnly, Category = "PK|Attribution")
	TObjectPtr<AActor> Instigator;

	UPROPERTY(BlueprintReadOnly, Category = "PK|Attribution")
	TObjectPtr<APlayerState> InstigatorPlayerState;

	UPROPERTY(BlueprintReadOnly, Category = "PK|Attribution")
	float WorldTimeSeconds = 0.0f;
};
