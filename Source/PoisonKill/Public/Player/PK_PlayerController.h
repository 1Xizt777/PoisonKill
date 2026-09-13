// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PK_PlayerController.generated.h"

class UPKInteractionComponent;
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
private:
	
	TWeakObjectPtr<UPKInteractionComponent> InteractionComponent;
	
	UPKInteractionComponent* GetInteractionComponent() const;
	
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
	
	void Input_Move(const FInputActionValue& InputActionValue);
	void Input_Look(const FInputActionValue& InputActionValue);
	void Input_Jump();
	
	void Input_InteractStarted();
	void Input_InteractCanceled();
};
