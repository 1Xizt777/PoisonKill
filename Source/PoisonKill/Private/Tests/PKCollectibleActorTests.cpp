#if WITH_DEV_AUTOMATION_TESTS

#include "Items/PKInventoryComponent.h"
#include "Misc/AutomationTest.h"
#include "World/PKCollectibleActor.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FPKCollectibleActorTest,
	"PoisonKill.Collectible.Actor",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FPKCollectibleActorTest::RunTest(const FString& Parameters)
{
	APawn* Interactor = NewObject<APawn>(
		GetTransientPackage(),
		TEXT("CollectibleTestPawn")
	);
	UPKInventoryComponent* Inventory = NewObject<UPKInventoryComponent>(
		Interactor,
		TEXT("InventoryComponent")
	);
	APKCollectibleActor* Collectible = NewObject<APKCollectibleActor>(
		GetTransientPackage(),
		TEXT("CollectibleTestActor")
	);

	TestNotNull(TEXT("Interactor is created"), Interactor);
	TestNotNull(TEXT("Inventory is created"), Inventory);
	TestNotNull(TEXT("Collectible is created"), Collectible);
	if (!Interactor || !Inventory || !Collectible)
	{
		return false;
	}

	Interactor->AddInstanceComponent(Inventory);

	FPKInteractionRequest Request;
	TestTrue(
		TEXT("Collectible can be interacted with"),
		Collectible->CanInteract_Implementation(Interactor, Request)
	);
	TestEqual(TEXT("Harvest duration is two seconds"), Request.HoldDuration, 2.0f);
	TestTrue(TEXT("Harvest progress is retained"), Request.bRetainProgressOnRelease);
	TestTrue(TEXT("Harvest locks player input"), Request.bLockPlayerInput);
	TestEqual(TEXT("Initial stock is three"), Collectible->GetRemainingStock(), 3);

	for (int32 Index = 0; Index < 3; ++Index)
	{
		Collectible->OnInteractionCompleted_Implementation(Interactor);
	}

	TestEqual(
		TEXT("Inventory receives three materials"),
		Inventory->GetMaterialCount(TEXT("Item_ToxicPlant")),
		3
	);
	TestEqual(TEXT("Stock is depleted"), Collectible->GetRemainingStock(), 0);
	TestFalse(
		TEXT("Depleted collectible cannot be interacted with"),
		Collectible->CanInteract_Implementation(Interactor, Request)
	);

	return true;
}

#endif
