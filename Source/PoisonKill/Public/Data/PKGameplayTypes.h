#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "PKGameplayTypes.generated.h"

UENUM(BlueprintType)
enum class EPKCarrierType : uint8
{
	Ingestion UMETA(DisplayName = "Ingestion"),
	Contact UMETA(DisplayName = "Contact"),
};

UENUM(BlueprintType)
enum class EPKVisualSignificance : uint8
{
	Low UMETA(DisplayName = "Low"),
	High UMETA(DisplayName = "High"),
};

USTRUCT(BlueprintType)
struct POISONKILL_API FPKPoisonDefinition : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Poison")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Poison", meta = (ClampMin = "0.0"))
	float SingleDose = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Poison", meta = (ClampMin = "0.0"))
	float LethalThreshold = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Poison", meta = (ClampMin = "0.0", Units = "s"))
	float IncubationTime = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Poison")
	EPKVisualSignificance VisualSignificance = EPKVisualSignificance::Low;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Poison")
	TArray<EPKCarrierType> AllowedCarrierTypes;
};

USTRUCT(BlueprintType)
struct POISONKILL_API FPKCarrierDefinition : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Carrier")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Carrier")
	EPKCarrierType CarrierType = EPKCarrierType::Ingestion;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Carrier")
	bool bConcealsVisuals = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Carrier", meta = (ClampMin = "0.0"))
	float DoseCoefficient = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Carrier", meta = (ClampMin = "1"))
	int32 DefaultResidueHits = 3;
};

USTRUCT(BlueprintType)
struct POISONKILL_API FPKNpcDefinition : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NPC")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NPC", meta = (ClampMin = "0.01"))
	float ThresholdCoefficient = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NPC", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float WarningThresholdRatio = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NPC", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float WarningMoveSpeedScale = 0.5f;
};

USTRUCT(BlueprintType)
struct POISONKILL_API FPKRecipeDefinition : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Recipe")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Recipe")
	FName InputItemId;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Recipe", meta = (ClampMin = "1"))
	int32 RequiredInputCount = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Recipe")
	FName OutputPoisonId;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Recipe", meta = (ClampMin = "0.0", Units = "s"))
	float ProcessingDuration = 4.0f;
};
