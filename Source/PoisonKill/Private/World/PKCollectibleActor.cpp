#include "World/PKCollectibleActor.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Pawn.h"
#include "Items/PKInventoryComponent.h"

APKCollectibleActor::APKCollectibleActor()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);
	
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//可本地化文本
	InteractionPrompt = NSLOCTEXT("PKCollectibleActor" , "DefaultInteractionPrompt" , "Collect Material");
}

void APKCollectibleActor::BeginPlay()
{
	Super::BeginPlay();

	RemainingStock = FMath::Max(0, InitialStock);
	OnStockChanged.Broadcast(RemainingStock);
}

int32 APKCollectibleActor::GetRemainingStock() const
{
	return RemainingStock;
}

bool APKCollectibleActor::CanInteract_Implementation(APawn* Interactor,FPKInteractionRequest& OutRequest)
{
	if (!IsValid(Interactor) || RemainingStock <= 0 || MaterialId.IsNone())
	{
		return false;
	}

	OutRequest.PromptText = InteractionPrompt;
	OutRequest.HoldDuration = HarvestDuration;
	OutRequest.bRetainProgressOnRelease = true;
	OutRequest.bLockPlayerInput = true;
	OutRequest.Priority = 0;
	return true;
}

void APKCollectibleActor::OnInteractionStarted_Implementation(APawn* Interactor)
{
}

void APKCollectibleActor::OnInteractionProgressUpdated_Implementation(APawn* Interactor,const float NormalizedProgress)
{
}

void APKCollectibleActor::OnInteractionCompleted_Implementation(APawn* Interactor)
{
	if (!IsValid(Interactor) || RemainingStock <= 0 || MaterialId.IsNone())
	{
		return;
	}

	UPKInventoryComponent* Inventory = Interactor->FindComponentByClass<UPKInventoryComponent>();
	if (!Inventory)
	{
		return;
	}

	if (Inventory->AddMaterial(MaterialId, 1) < 0)
	{
		return;
	}

	--RemainingStock;
	OnStockChanged.Broadcast(RemainingStock);

	if (RemainingStock <= 0)
	{
		OnStockDepleted.Broadcast(RemainingStock);
	}
}

void APKCollectibleActor::OnInteractionCanceled_Implementation(APawn* Interactor)
{
	
}
