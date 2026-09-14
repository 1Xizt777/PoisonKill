// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PK_PlayerController.generated.h"

class UPKInteractionComponent;
class UPKInteractionPromptWidget;
class UPKInventoryComponent;
class UPKInventoryHudWidget;
class UInputAction;
class UInputMappingContext;
struct FInputActionValue;


UCLASS()
class POISONKILL_API APK_PlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:

	
protected:

	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
private:
	
	UPROPERTY(EditDefaultsOnly, Category = "PK|UI")
	TSubclassOf<UPKInteractionPromptWidget> InteractionPromptWidgetClass;	//WidgetClass

	UPROPERTY(Transient)
	TObjectPtr<UPKInteractionPromptWidget> InteractionPromptWidget;

	UPROPERTY(EditDefaultsOnly, Category = "PK|UI")
	TSubclassOf<UPKInventoryHudWidget> InventoryHudWidgetClass;

	UPROPERTY(Transient)
	TObjectPtr<UPKInventoryHudWidget> InventoryHudWidget;
	
	TWeakObjectPtr<UPKInteractionComponent> InteractionComponent;
	TWeakObjectPtr<UPKInventoryComponent> InventoryComponent;
	
	UFUNCTION(BlueprintPure, Category = "PK|Interaction")
	UPKInteractionComponent* GetInteractionComponent() const;

	UFUNCTION(BlueprintPure, Category = "PK|Inventory")
	UPKInventoryComponent* GetInventoryComponent() const;

	void RefreshInteractionPrompt(UPKInteractionComponent* NewComponent);
	void RemoveInteractionPrompt();
	void RefreshInventoryHud(UPKInventoryComponent* NewComponent);
	void RemoveInventoryHud();
	
	bool IsInteractionLocked() const;
	
	UPROPERTY(EditAnywhere,Category="PK|Input")
	TObjectPtr<UInputMappingContext> PKIMC;
	
	UPROPERTY(EditAnywhere,Category="PK|Input")
	TObjectPtr<UInputAction> MoveAction;
	
	UPROPERTY(EditAnywhere,Category="PK|Input")
	TObjectPtr<UInputAction> LookAction;
	
	UPROPERTY(EditAnywhere,Category="PK|Input")
	TObjectPtr<UInputAction> JumpAction;
	
	UPROPERTY(EditAnywhere, Category = "PK|Input")
	TObjectPtr<UInputAction> InteractAction;

	UPROPERTY(EditAnywhere, Category = "PK|Input")
	TObjectPtr<UInputAction> SwitchItemAction;
	
	void Input_Move(const FInputActionValue& InputActionValue);
	void Input_Look(const FInputActionValue& InputActionValue);
	void Input_Jump();
	void Input_NextItem();
	
	void Input_InteractStarted();
	void Input_InteractCanceled();
};