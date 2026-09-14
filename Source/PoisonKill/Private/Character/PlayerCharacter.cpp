#include "Character/PlayerCharacter.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Interaction/PKInteractionComponent.h"
#include "Items/PKInventoryComponent.h"
#include "Effects/PKPoisonVictimComponent.h"


APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
	
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	
	GetCharacterMovement()->bOrientRotationToMovement = true;

	
	InventoryComponent = CreateDefaultSubobject<UPKInventoryComponent>(TEXT("InventoryComponent"));

	
	InteractionComponent = CreateDefaultSubobject<UPKInteractionComponent>(TEXT("InteractionComponent"));

	PoisonVictimComponent = CreateDefaultSubobject<UPKPoisonVictimComponent>(TEXT("PoisonVictimComponent"));

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetRootComponent());
	SpringArm->bUsePawnControlRotation = true;
	
	
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FollowCamera->SetupAttachment(SpringArm);
	FollowCamera->bUsePawnControlRotation = false;
}


void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

