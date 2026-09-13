#pragma once

#include "CoreMinimal.h"
#include "PKInteractionTypes.generated.h"

USTRUCT(BlueprintType)
struct POISONKILL_API FPKInteractionRequest
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	FText PromptText;		//显示的提示

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction", meta = (ClampMin = "0.0", Units = "s"))
	float HoldDuration = 2.0f;		//需要按住的时间 , 采集是 2 秒，加工是 4 秒，施用是 2 秒

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	bool bRetainProgressOnRelease = false;		//中断是否保留进度

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	bool bLockPlayerInput = true;		//交互中是否锁定Input

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	int32 Priority = 0;		//优先级
};
