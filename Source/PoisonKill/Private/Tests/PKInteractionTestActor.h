#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/PKInteractable.h"
#include "PKInteractionTestActor.generated.h"

UCLASS()
class APKInteractionTestActor : public AActor, public IPKInteractable
{
	GENERATED_BODY()

public:
	FPKInteractionRequest Request;
	int32 StartCount = 0;
	int32 ProgressCount = 0;
	int32 CompletedCount = 0;
	int32 CanceledCount = 0;
	float LastProgress = 0.0f;

	virtual bool CanInteract_Implementation(
		APawn* Interactor,
		FPKInteractionRequest& OutRequest
	) override
	{
		OutRequest = Request;
		return true;
	}

	virtual void OnInteractionStarted_Implementation(APawn* Interactor) override
	{
		++StartCount;
	}

	virtual void OnInteractionProgressUpdated_Implementation(
		APawn* Interactor,
		const float NormalizedProgress
	) override
	{
		++ProgressCount;
		LastProgress = NormalizedProgress;
	}

	virtual void OnInteractionCompleted_Implementation(APawn* Interactor) override
	{
		++CompletedCount;
	}

	virtual void OnInteractionCanceled_Implementation(APawn* Interactor) override
	{
		++CanceledCount;
	}
};
