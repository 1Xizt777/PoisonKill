#include "Character/EnemyCharacter.h"

#include "Effects/PKPoisonVictimComponent.h"

AEnemyCharacter::AEnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	PoisonVictimComponent = CreateDefaultSubobject<UPKPoisonVictimComponent>(TEXT("PoisonVictimComponent"));
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