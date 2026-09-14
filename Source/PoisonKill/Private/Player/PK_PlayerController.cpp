#include "Player/PK_PlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/Character.h"
#include "Interaction/PKInteractionComponent.h"
#include "Items/PKInventoryComponent.h"
#include "UI/PKInteractionPromptWidget.h"
#include "UI/PKInventoryHudWidget.h"

void APK_PlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (IsValid(Subsystem))
	{
		Subsystem->AddMappingContext(PKIMC,0.f);
	}

	RefreshInteractionPrompt(GetInteractionComponent());
	RefreshInventoryHud(GetInventoryComponent());
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

		PKInputComponent->BindAction(SwitchItemAction,ETriggerEvent::Started,this,&ThisClass::Input_NextItem);
	}
}

void APK_PlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	InteractionComponent = IsValid(InPawn)? InPawn->FindComponentByClass<UPKInteractionComponent>(): nullptr;
	InventoryComponent = IsValid(InPawn)? InPawn->FindComponentByClass<UPKInventoryComponent>(): nullptr;

	if (HasActorBegunPlay())
	{
		RefreshInteractionPrompt(InteractionComponent.Get());
		RefreshInventoryHud(InventoryComponent.Get());
	}
}

void APK_PlayerController::OnUnPossess()
{
	if (UPKInteractionComponent* Component = InteractionComponent.Get())
	{
		Component->CancelInteraction();
	}

	RemoveInteractionPrompt();
	InteractionComponent.Reset();

	RemoveInventoryHud();
	InventoryComponent.Reset();
	
	Super::OnUnPossess();
}

UPKInteractionComponent* APK_PlayerController::GetInteractionComponent() const
{
	return InteractionComponent.Get();
}

UPKInventoryComponent* APK_PlayerController::GetInventoryComponent() const
{
	return InventoryComponent.Get();
}

bool APK_PlayerController::IsInteractionLocked() const
{
	const UPKInteractionComponent* Component = GetInteractionComponent();
	return Component && Component->IsInteractionLocked();
}

void APK_PlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	RemoveInteractionPrompt();
	RemoveInventoryHud();
	Super::EndPlay(EndPlayReason);
}

void APK_PlayerController::RefreshInteractionPrompt(UPKInteractionComponent* NewComponent)
{
	if (!InteractionPromptWidgetClass)
	{
		return;
	}

	if (!InteractionPromptWidget)
	{
		InteractionPromptWidget = CreateWidget<UPKInteractionPromptWidget>(this,InteractionPromptWidgetClass);
	}

	if (!InteractionPromptWidget)
	{
		return;
	}

	InteractionPromptWidget->SetInteractionComponent(NewComponent);

	if (NewComponent)
	{
		if (!InteractionPromptWidget->IsInViewport())
		{
			InteractionPromptWidget->AddToViewport();
		}
	}
	else
	{
		InteractionPromptWidget->RemoveFromParent();
	}
}

void APK_PlayerController::RemoveInteractionPrompt()
{
	if (!InteractionPromptWidget)
	{
		return;
	}

	InteractionPromptWidget->SetInteractionComponent(nullptr);
	InteractionPromptWidget->RemoveFromParent();
	InteractionPromptWidget = nullptr;
}

void APK_PlayerController::RefreshInventoryHud(UPKInventoryComponent* NewComponent)
{
	if (!InventoryHudWidgetClass)
	{
		return;
	}

	if (!InventoryHudWidget)
	{
		InventoryHudWidget = CreateWidget<UPKInventoryHudWidget>(this,InventoryHudWidgetClass);
	}

	if (!InventoryHudWidget)
	{
		return;
	}

	InventoryHudWidget->SetInventoryComponent(NewComponent);

	if (NewComponent)
	{
		if (!InventoryHudWidget->IsInViewport())
		{
			InventoryHudWidget->AddToViewport();
		}
	}
	else
	{
		InventoryHudWidget->RemoveFromParent();
	}
}

void APK_PlayerController::RemoveInventoryHud()
{
	if (!InventoryHudWidget)
	{
		return;
	}

	InventoryHudWidget->SetInventoryComponent(nullptr);
	InventoryHudWidget->RemoveFromParent();
	InventoryHudWidget = nullptr;
}

void APK_PlayerController::Input_Move(const FInputActionValue& InputActionValue)
{
	if (IsInteractionLocked()) return;
	
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
	// if (IsInteractionLocked()) return;
	
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
	
	AddYawInput(InputAxisVector.X);
	AddPitchInput(InputAxisVector.Y);
}

void APK_PlayerController::Input_Jump()
{
	if (IsInteractionLocked()) return;
	
	if (!IsValid(GetCharacter())) return;
	
	GetCharacter()->Jump();
}

void APK_PlayerController::Input_NextItem()
{
	if (IsInteractionLocked()) return;
	
	if (UPKInventoryComponent* Component = GetInventoryComponent())
	{
		Component->CycleSelectedItem(1);
	}
}

void APK_PlayerController::Input_InteractStarted()
{
	if (UPKInteractionComponent* Component = GetInteractionComponent())
	{
		Component->BeginInteraction();
	}
}

void APK_PlayerController::Input_InteractCanceled()
{
	if (UPKInteractionComponent* Component = GetInteractionComponent())
	{
		Component->EndInteraction();
	}
}