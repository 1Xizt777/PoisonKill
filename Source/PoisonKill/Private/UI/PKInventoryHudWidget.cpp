#include "UI/PKInventoryHudWidget.h"

#include "Components/TextBlock.h"
#include "Data/PKGameplayDataSubsystem.h"
#include "Data/PKGameplayTypes.h"
#include "Engine/GameInstance.h"

void UPKInventoryHudWidget::SetInventoryComponent(UPKInventoryComponent* NewComponent)
{
	if (InventoryComponent == NewComponent)
	{
		RefreshFromComponent();
		return;
	}

	UnbindInventoryEvents();
	InventoryComponent = NewComponent;
	BindInventoryEvents();
	RefreshFromComponent();
}

UPKInventoryComponent* UPKInventoryHudWidget::GetInventoryComponent() const
{
	return InventoryComponent;
}

void UPKInventoryHudWidget::NativeConstruct()
{
	Super::NativeConstruct();
	BindInventoryEvents();
	RefreshFromComponent();
}

void UPKInventoryHudWidget::NativeDestruct()
{
	UnbindInventoryEvents();
	Super::NativeDestruct();
}

void UPKInventoryHudWidget::HandleSelectedItemChanged(FName ItemId, EPKInventoryItemCategory Category)
{
	RefreshFromComponent();
}

void UPKInventoryHudWidget::HandleInventoryChanged(FName ItemId, int32 NewCount, EPKInventoryItemCategory Category)
{
	RefreshFromComponent();
}

void UPKInventoryHudWidget::HandleSelectedPoisonChanged(FName PoisonId)
{
	RefreshFromComponent();
}

void UPKInventoryHudWidget::HandleSelectedMaterialChanged(FName MaterialId)
{
	RefreshFromComponent();
}

void UPKInventoryHudWidget::BindInventoryEvents()
{
	if (bEventsBound || !InventoryComponent)
	{
		return;
	}

	InventoryComponent->OnSelectedItemChanged.AddDynamic(this,&ThisClass::HandleSelectedItemChanged);
	InventoryComponent->OnInventoryChanged.AddDynamic(this,&ThisClass::HandleInventoryChanged);
	InventoryComponent->OnSelectedPoisonChanged.AddDynamic(this,&ThisClass::HandleSelectedPoisonChanged);
	InventoryComponent->OnSelectedMaterialChanged.AddDynamic(this,&ThisClass::HandleSelectedMaterialChanged);
	bEventsBound = true;
}

void UPKInventoryHudWidget::UnbindInventoryEvents()
{
	if (bEventsBound && InventoryComponent)
	{
		InventoryComponent->OnSelectedItemChanged.RemoveDynamic(this,&ThisClass::HandleSelectedItemChanged);
		InventoryComponent->OnInventoryChanged.RemoveDynamic(this,&ThisClass::HandleInventoryChanged);
		InventoryComponent->OnSelectedPoisonChanged.RemoveDynamic(this,&ThisClass::HandleSelectedPoisonChanged);
		InventoryComponent->OnSelectedMaterialChanged.RemoveDynamic(this,&ThisClass::HandleSelectedMaterialChanged);
	}

	bEventsBound = false;
}

void UPKInventoryHudWidget::RefreshFromComponent()
{
	if (ItemNameTextBlock)
	{
		ItemNameTextBlock->SetText(FText::FromString(TEXT("None")));
	}

	if (ItemCountTextBlock)
	{
		ItemCountTextBlock->SetText(FText::FromString(TEXT("0")));
	}

	if (!InventoryComponent)
	{
		return;
	}

	const FName ItemId = InventoryComponent->GetSelectedItemId();
	const int32 Count = InventoryComponent->GetSelectedItemCount();
	const EPKInventoryItemCategory Category = InventoryComponent->GetSelectedItemCategory();

	FText ItemName = FText::FromName(ItemId);

	if (Category == EPKInventoryItemCategory::Poison && !ItemId.IsNone())
	{
		const UGameInstance* GameInstance = GetGameInstance();
		if (GameInstance)
		{
			const UPKGameplayDataSubsystem* DataSubsystem = GameInstance->GetSubsystem<UPKGameplayDataSubsystem>();
			if (DataSubsystem)
			{
				FPKPoisonDefinition PoisonDef;
				if (DataSubsystem->GetPoisonDefinition(ItemId, PoisonDef))
				{
					ItemName = PoisonDef.DisplayName;
				}
			}
		}
	}

	if (ItemNameTextBlock)
	{
		ItemNameTextBlock->SetText(ItemName);
	}

	if (ItemCountTextBlock)
	{
		ItemCountTextBlock->SetText(FText::AsNumber(Count));
	}
}