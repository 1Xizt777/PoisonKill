#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "PlayerCharacter.generated.h"

class UCameraComponent;

class UPKPoisonVictimComponent;
class UPKInteractionComponent;
class UPKInventoryComponent;
class USpringArmComponent;

UCLASS()
class POISONKILL_API APlayerCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:

	APlayerCharacter();

	UFUNCTION(BlueprintPure, Category = "PK|Poison")
	UPKPoisonVictimComponent* GetPoisonVictimComponent() const { return PoisonVictimComponent; }

	UFUNCTION(BlueprintPure, Category = "PK|Inventory")
	UPKInventoryComponent* GetInventoryComponent() const { return InventoryComponent; }

protected:

	virtual void BeginPlay() override;

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PK|Inventory", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UPKInventoryComponent> InventoryComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PK|Interaction", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UPKInteractionComponent> InteractionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PK|Poison", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UPKPoisonVictimComponent> PoisonVictimComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> FollowCamera;
};