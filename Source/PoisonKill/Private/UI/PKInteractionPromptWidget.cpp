#include "UI/PKInteractionPromptWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Interaction/PKInteractionComponent.h"

void UPKInteractionPromptWidget::SetInteractionComponent(UPKInteractionComponent* NewComponent)
{
	if (InteractionComponent == NewComponent)
	{
		RefreshFromComponent();
		return;
	}

	UnbindInteractionEvents();
	InteractionComponent = NewComponent;
	BindInteractionEvents();
	RefreshFromComponent();
}

UPKInteractionComponent* UPKInteractionPromptWidget::GetInteractionComponent() const
{
	return InteractionComponent;
}

void UPKInteractionPromptWidget::NativeConstruct()
{
	Super::NativeConstruct();

	BindInteractionEvents();
	RefreshFromComponent();
}

void UPKInteractionPromptWidget::NativeDestruct()
{
	UnbindInteractionEvents();
	Super::NativeDestruct();
}

void UPKInteractionPromptWidget::HandleInteractionTargetChanged(AActor* Target,FText PromptText)
{
	const bool bHasTarget = IsValid(Target);
	SetPromptVisible(bHasTarget);

	if (PromptTextBlock)
	{
		PromptTextBlock->SetText(PromptText);
	}

	BP_HandleInteractionProgressChanged(Target, 0.0f);
}

void UPKInteractionPromptWidget::HandleInteractionProgressChanged(AActor* Target,const float NormalizedProgress)
{
	BP_HandleInteractionProgressChanged(Target,FMath::Clamp(NormalizedProgress, 0.0f, 1.0f));
}

void UPKInteractionPromptWidget::HandleInteractionCompleted(AActor* Target)
{
	BP_HandleInteractionCompleted(Target);
	GEngine->AddOnScreenDebugMessage(-1,3.f,FColor::Yellow,TEXT("HandleInteractionCompleted"));
}

void UPKInteractionPromptWidget::HandleInteractionCanceled(AActor* Target)
{
	BP_HandleInteractionCanceled(Target);
}

void UPKInteractionPromptWidget::BindInteractionEvents()
{
	if (bEventsBound || !InteractionComponent)
	{
		return;
	}

	InteractionComponent->OnInteractionTargetChanged.AddDynamic(
		this,
		&ThisClass::HandleInteractionTargetChanged
	);
	InteractionComponent->OnInteractionProgressChanged.AddDynamic(
		this,
		&ThisClass::HandleInteractionProgressChanged
	);
	InteractionComponent->OnInteractionCompleted.AddDynamic(
		this,
		&ThisClass::HandleInteractionCompleted
	);
	InteractionComponent->OnInteractionCanceled.AddDynamic(
		this,
		&ThisClass::HandleInteractionCanceled
	);
	bEventsBound = true;
}

void UPKInteractionPromptWidget::UnbindInteractionEvents()
{
	if (!bEventsBound || !InteractionComponent)
	{
		bEventsBound = false;
		return;
	}

	InteractionComponent->OnInteractionTargetChanged.RemoveDynamic(
		this,
		&ThisClass::HandleInteractionTargetChanged
	);
	InteractionComponent->OnInteractionProgressChanged.RemoveDynamic(
		this,
		&ThisClass::HandleInteractionProgressChanged
	);
	InteractionComponent->OnInteractionCompleted.RemoveDynamic(
		this,
		&ThisClass::HandleInteractionCompleted
	);
	InteractionComponent->OnInteractionCanceled.RemoveDynamic(
		this,
		&ThisClass::HandleInteractionCanceled
	);
	bEventsBound = false;
}

void UPKInteractionPromptWidget::RefreshFromComponent()
{
	if (!InteractionComponent)
	{
		HandleInteractionTargetChanged(nullptr, FText::GetEmpty());
		return;
	}

	AActor* Target = InteractionComponent->GetCurrentTarget();
	const bool bHasTarget = IsValid(Target);

	HandleInteractionTargetChanged(Target,bHasTarget? InteractionComponent->GetCurrentRequest().PromptText: FText::GetEmpty());

	if (bHasTarget)
	{
		HandleInteractionProgressChanged(Target,InteractionComponent->GetNormalizedProgress());
	}
}

void UPKInteractionPromptWidget::SetPromptVisible(const bool bVisible)
{
	SetVisibility(bVisible? ESlateVisibility::HitTestInvisible: ESlateVisibility::Collapsed);

	if (KeyIconImage)
	{
		KeyIconImage->SetVisibility(bVisible? ESlateVisibility::HitTestInvisible: ESlateVisibility::Collapsed);
	}
}