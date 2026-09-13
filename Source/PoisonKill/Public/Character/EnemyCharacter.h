#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "Interface/EnemyInterface.h"
#include "EnemyCharacter.generated.h"

UCLASS()
class POISONKILL_API AEnemyCharacter : public ABaseCharacter, public IEnemyInterface
{
	GENERATED_BODY()

public:

	AEnemyCharacter();

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite, Category="PK|Weapon")
	bool IsEquipedWeapon;
	
	/*Begin EnemyInterface*/
	virtual bool GetIsEquipedWeapon_Implementation() override;
	/*End   EnemyInterface*/
	
protected:

	virtual void BeginPlay() override;

private:

};
