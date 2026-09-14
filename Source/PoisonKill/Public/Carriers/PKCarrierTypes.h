#pragma once

#include "CoreMinimal.h"
class AActor;
#include "PKCarrierTypes.generated.h"

USTRUCT(BlueprintType)
struct POISONKILL_API FPKCarrierPayload
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PK|Carrier")
	FName PoisonId;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PK|Carrier", meta = (ClampMin = "0.0"))
	float RemainingDose = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PK|Carrier", meta = (ClampMin = "0"))
	int32 RemainingResidueHits = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PK|Carrier")
	TObjectPtr<AActor> Instigator;

	bool IsEmpty() const { return PoisonId.IsNone() || RemainingDose <= 0.0f || RemainingResidueHits <= 0; }
};