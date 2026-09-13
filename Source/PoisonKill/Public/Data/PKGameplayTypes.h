#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "PKGameplayTypes.generated.h"

UENUM(BlueprintType)
enum class EPKCarrierType : uint8  
{
	None UMETA(DisplayName = "None"),
	Ingestion UMETA(DisplayName = "Ingestion"), //摄入型
	Contact UMETA(DisplayName = "Contact")		//接触型
};

UENUM(BlueprintType)
enum class EPKVisualSignificance : uint8	//视觉显著度
{
	Low UMETA(DisplayName = "Low"),		//低
	High UMETA(DisplayName = "High"),	//高
};

USTRUCT(BlueprintType)
struct POISONKILL_API FPKPoisonDefinition : public FTableRowBase  //毒物定义
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Poison")
	FText DisplayName;  

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Poison", meta = (ClampMin = "0.0"))
	float SingleDose = 0.0f;		//单剂量

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Poison", meta = (ClampMin = "0.0"))
	float LethalThreshold = 0.0f;		//致死阈值

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Poison", meta = (ClampMin = "0.0", Units = "s"))
	float IncubationTime = 0.0f;		//潜伏期

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Poison")
	EPKVisualSignificance VisualSignificance = EPKVisualSignificance::Low;		//视觉显著度

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Poison")
	TArray<EPKCarrierType> AllowedCarrierTypes;			//可携带方式
};

USTRUCT(BlueprintType)
struct POISONKILL_API FPKCarrierDefinition : public FTableRowBase	//载体定义
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Carrier")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Carrier")
	EPKCarrierType CarrierType = EPKCarrierType::Ingestion;		//摄入类型

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Carrier")
	bool bConcealsVisuals = false;		//是否遮蔽视觉	

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Carrier", meta = (ClampMin = "0.0"))
	float DoseCoefficient = 1.0f;		//剂量系数

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Carrier", meta = (ClampMin = "1"))
	int32 DefaultResidueHits = 3;		//残留次数，被触碰该次数后才清除。默认 3 次（输入型1次）
};

USTRUCT(BlueprintType)
struct POISONKILL_API FPKNpcDefinition : public FTableRowBase	//Npc定义
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NPC")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NPC", meta = (ClampMin = "0.01"))
	float ThresholdCoefficient = 1.0f;		//阈值系数(主要NPC：1   |   次要NPC：0.5)

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NPC", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float WarningThresholdRatio = 0.5f;		//预警阈值 = 实际致死阈值 × WarningThresholdRatio

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NPC", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float WarningMoveSpeedScale = 0.5f;		//预警后最大速度 = 原始最大速度 × WarningMoveSpeedScale(0.5)
};

USTRUCT(BlueprintType)
struct POISONKILL_API FPKRecipeDefinition : public FTableRowBase		//原料定义
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Recipe")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Recipe")
	FName InputItemId;		//原料ID

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Recipe", meta = (ClampMin = "1"))
	int32 RequiredInputCount = 1;		//原料不足时隐藏加工交互提示

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Recipe")
	FName OutputPoisonId;		//制成的毒药ID

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Recipe", meta = (ClampMin = "0.0", Units = "s"))
	float ProcessingDuration = 4.0f;		//加工台长按时间
};
