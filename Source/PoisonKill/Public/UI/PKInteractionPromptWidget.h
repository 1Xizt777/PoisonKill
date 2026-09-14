#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PKInteractionPromptWidget.generated.h"

class UImage;
class UPKInteractionComponent;
class UTextBlock;

UCLASS(Abstract, Blueprintable)
class POISONKILL_API UPKInteractionPromptWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "PK|Interaction|UI")
	void SetInteractionComponent(UPKInteractionComponent* NewComponent);

	UFUNCTION(BlueprintPure, Category = "PK|Interaction|UI")
	UPKInteractionComponent* GetInteractionComponent() const;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION()
	void HandleInteractionTargetChanged(AActor* Target, FText PromptText);

	UFUNCTION()
	void HandleInteractionProgressChanged(AActor* Target, float NormalizedProgress);

	UFUNCTION()
	void HandleInteractionCompleted(AActor* Target);

	UFUNCTION()
	void HandleInteractionCanceled(AActor* Target);

	
	//三个回调由蓝图实现
	UFUNCTION(BlueprintImplementableEvent, Category = "PK|Interaction|UI")
	void BP_HandleInteractionProgressChanged(AActor* Target, float NormalizedProgress);

	UFUNCTION(BlueprintImplementableEvent, Category = "PK|Interaction|UI")
	void BP_HandleInteractionCompleted(AActor* Target);

	UFUNCTION(BlueprintImplementableEvent, Category = "PK|Interaction|UI")
	void BP_HandleInteractionCanceled(AActor* Target);

	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> PromptTextBlock;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> KeyIconImage;

private:
	UPROPERTY(Transient)
	TObjectPtr<UPKInteractionComponent> InteractionComponent;

	bool bEventsBound = false;

	void BindInteractionEvents();
	void UnbindInteractionEvents();
	void RefreshFromComponent();
	void SetPromptVisible(bool bVisible);
};