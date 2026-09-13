#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PKInteractionTypes.h"
#include "PKInteractable.generated.h"

UINTERFACE(BlueprintType)
class POISONKILL_API UPKInteractable : public UInterface
{
	GENERATED_BODY()
};

class POISONKILL_API IPKInteractable	//可交互接口
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "PK|Interaction")
	bool CanInteract(APawn* Interactor, FPKInteractionRequest& OutRequest);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "PK|Interaction")
	void OnInteractionStarted(APawn* Interactor);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "PK|Interaction")
	void OnInteractionProgressUpdated(APawn* Interactor, float NormalizedProgress);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "PK|Interaction")
	void OnInteractionCompleted(APawn* Interactor);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "PK|Interaction")
	void OnInteractionCanceled(APawn* Interactor);
};
