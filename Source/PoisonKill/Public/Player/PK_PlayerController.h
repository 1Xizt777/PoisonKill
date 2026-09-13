// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PK_PlayerController.generated.h"

class UInputAction;
class UInputMappingContext;
struct FInputActionValue;


UCLASS()
class POISONKILL_API APK_PlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	APK_PlayerController();
	
	
protected:

	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	
	
private:
	UPROPERTY(EditAnywhere,Category="PK|Input")
	TObjectPtr<UInputMappingContext> PKIMC;
	
	UPROPERTY(EditAnywhere,Category="PK|Input")
	TObjectPtr<UInputAction> MoveAction;
	
	UPROPERTY(EditAnywhere,Category="PK|Input")
	TObjectPtr<UInputAction> LookAction;
	
	UPROPERTY(EditAnywhere,Category="PK|Input")
	TObjectPtr<UInputAction> JumpAction;
	
	void Input_Move(const FInputActionValue& InputActionValue);
	void Input_Look(const FInputActionValue& InputActionValue);
	void Input_Jump();
	
	
};
