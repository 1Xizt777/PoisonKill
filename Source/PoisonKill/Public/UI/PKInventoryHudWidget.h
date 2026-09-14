#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Items/PKInventoryComponent.h"
#include "PKInventoryHudWidget.generated.h"

class UTextBlock;

UCLASS(Abstract, Blueprintable)
class POISONKILL_API UPKInventoryHudWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "PK|Inventory|UI")
	void SetInventoryComponent(UPKInventoryComponent* NewComponent);

	UFUNCTION(BlueprintPure, Category = "PK|Inventory|UI")
	UPKInventoryComponent* GetInventoryComponent() const;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION()
	void HandleSelectedItemChanged(FName ItemId, EPKInventoryItemCategory Category);

	UFUNCTION()
	void HandleInventoryChanged(FName ItemId, int32 NewCount, EPKInventoryItemCategory Category);

	UFUNCTION()
	void HandleSelectedPoisonChanged(FName PoisonId);

	UFUNCTION()
	void HandleSelectedMaterialChanged(FName MaterialId);

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ItemNameTextBlock;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ItemCountTextBlock;

private:
	UPROPERTY(Transient)
	TObjectPtr<UPKInventoryComponent> InventoryComponent;

	bool bEventsBound = false;

	void BindInventoryEvents();
	void UnbindInventoryEvents();
	void RefreshFromComponent();
};