#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "Interface/EnemyInterface.h"
#include "EnemyCharacter.generated.h"

class UPKPoisonVictimComponent;

UCLASS()
class POISONKILL_API AEnemyCharacter : public ABaseCharacter, public IEnemyInterface
{
	GENERATED_BODY()

public:
	AEnemyCharacter();

	UFUNCTION(BlueprintPure, Category = "PK|Poison")
	UPKPoisonVictimComponent* GetPoisonVictimComponent() const { return PoisonVictimComponent; }

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PK|Weapon")
	bool IsEquipedWeapon;

	virtual bool GetIsEquipedWeapon_Implementation() override;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PK|Poison", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UPKPoisonVictimComponent> PoisonVictimComponent;
};