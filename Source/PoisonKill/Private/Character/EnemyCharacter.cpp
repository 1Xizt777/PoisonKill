#include "Character/EnemyCharacter.h"



AEnemyCharacter::AEnemyCharacter()
{

	PrimaryActorTick.bCanEverTick = false;
	
	IsEquipedWeapon = false;
}


bool AEnemyCharacter::GetIsEquipedWeapon_Implementation()
{
	return IsEquipedWeapon;
}

void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}


