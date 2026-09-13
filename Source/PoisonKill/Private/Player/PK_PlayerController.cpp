

#include "Player/PK_PlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/Character.h"
#include "Interaction/PKInteractionComponent.h"

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
		
		
		PKInputComponent->BindAction(InteractAction,ETriggerEvent::Started,this,&ThisClass::Input_InteractStarted);
		PKInputComponent->BindAction(InteractAction,ETriggerEvent::Completed,this,&ThisClass::Input_InteractCanceled);
		PKInputComponent->BindAction(InteractAction,ETriggerEvent::Canceled,this,&ThisClass::Input_InteractCanceled);
	}
}

void APK_PlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	InteractionComponent = IsValid(InPawn)? InPawn->FindComponentByClass<UPKInteractionComponent>() : nullptr;  //获取InteractComponent
}

void APK_PlayerController::OnUnPossess()
{
	if (UPKInteractionComponent* Component = InteractionComponent.Get())
	{
		Component->CancelInteraction();
	}

	InteractionComponent.Reset();	//back to the null，回到空指针
	
	
	//在流程之前取消交互，reset指针
	
	Super::OnUnPossess();	//执行 PlayerController 基类的“解除 Pawn 关系”流程，
}

UPKInteractionComponent* APK_PlayerController::GetInteractionComponent() const
{
	return InteractionComponent.Get();
}

bool APK_PlayerController::IsInteractionLocked() const
{
	const UPKInteractionComponent* Component = GetInteractionComponent();
	return Component && Component->IsInteractionLocked();
}

void APK_PlayerController::Input_Move(const FInputActionValue& InputActionValue)
{
	if (IsInteractionLocked()) return;	//交互锁定则直接返回
	
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
	if (IsInteractionLocked()) return;	//交互锁定则直接返回
	
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
	
	AddYawInput(InputAxisVector.X);
	AddPitchInput(InputAxisVector.Y);
	
}

void APK_PlayerController::Input_Jump()
{
	if (IsInteractionLocked()) return;	//交互锁定则直接返回
	
	if (!IsValid(GetCharacter())) return;
	
	GetCharacter()->Jump();
	
}

void APK_PlayerController::Input_InteractStarted()
{
	GEngine->AddOnScreenDebugMessage(-1,3.f,FColor::Red,TEXT("Input_InteractStarted"));
	if (UPKInteractionComponent* Component = GetInteractionComponent())
	{
		Component->BeginInteraction();
	}
}

void APK_PlayerController::Input_InteractCanceled()
{
	GEngine->AddOnScreenDebugMessage(-1,3.f,FColor::Red,TEXT("Input_InteractCanceled"));
	if (UPKInteractionComponent* Component = GetInteractionComponent())
	{
		Component->EndInteraction();
	}
}
