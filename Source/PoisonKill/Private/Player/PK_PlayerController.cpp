

#include "Player/PK_PlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/Character.h"

APK_PlayerController::APK_PlayerController()
{
	
}

void APK_PlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (IsValid(Subsystem))
	{
		Subsystem->AddMappingContext(PKIMC,0.f);
	}
	
}

void APK_PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	UEnhancedInputComponent* PKInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);
	if (IsValid(PKInputComponent))
	{
		PKInputComponent->BindAction(MoveAction,ETriggerEvent::Triggered,this,&ThisClass::Input_Move);
		PKInputComponent->BindAction(LookAction,ETriggerEvent::Triggered,this,&ThisClass::Input_Look);
		PKInputComponent->BindAction(JumpAction,ETriggerEvent::Started,this,&ThisClass::Input_Jump);
	}
}

void APK_PlayerController::Input_Move(const FInputActionValue& InputActionValue)
{
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
	
	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0.f,Rotation.Yaw,0.f);
	
	
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	
	if (APawn* ControlledPawn = GetPawn())
	{
		ControlledPawn->AddMovementInput(ForwardDirection,InputAxisVector.Y);
		ControlledPawn->AddMovementInput(RightDirection,InputAxisVector.X);
	}
}

void APK_PlayerController::Input_Look(const FInputActionValue& InputActionValue)
{
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
	
	AddYawInput(InputAxisVector.X);
	AddPitchInput(InputAxisVector.Y);
	
}

void APK_PlayerController::Input_Jump()
{
	if (!IsValid(GetCharacter())) return;
	
	GetCharacter()->Jump();
	
}
