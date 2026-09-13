#if WITH_DEV_AUTOMATION_TESTS

#include "Items/PKInventoryComponent.h"
#include "Misc/AutomationTest.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FPKInventoryComponentTest,
	"PoisonKill.Inventory.Component",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)


bool FPKInventoryComponentTest::RunTest(const FString& Parameters)
{
	UPKInventoryComponent* Inventory = NewObject<UPKInventoryComponent>();
	TestNotNull(TEXT("Inventory component is created"), Inventory);
	if (!Inventory)
	{
		return false;
	}

	TestEqual(
		TEXT("Adding materials returns the new count"),
		Inventory->AddMaterial(TEXT("Item_ToxicPlant"), 2),
		2
	);
	TestTrue(
		TEXT("Material amount can be queried"),
		Inventory->HasMaterial(TEXT("Item_ToxicPlant"), 2)
	);
	TestTrue(
		TEXT("Removing an available material succeeds"),
		Inventory->RemoveMaterial(TEXT("Item_ToxicPlant"), 1)
	);
	TestEqual(
		TEXT("Material count is updated"),
		Inventory->GetMaterialCount(TEXT("Item_ToxicPlant")),
		1
	);
	TestFalse(
		TEXT("Removing more material than available fails"),
		Inventory->RemoveMaterial(TEXT("Item_ToxicPlant"), 2)
	);

	Inventory->AddPoison(TEXT("Poison_Hemlock"), 1);
	TestTrue(
		TEXT("First poison becomes selected"),
		Inventory->GetSelectedPoisonId() == TEXT("Poison_Hemlock")
	);
	TestEqual(
		TEXT("Selected poison count is available"),
		Inventory->GetSelectedPoisonCount(),
		1
	);

	TestTrue(
		TEXT("Poison can be consumed"),
		Inventory->RemovePoison(TEXT("Poison_Hemlock"), 1)
	);
	TestTrue(
		TEXT("Selected poison clears when depleted"),
		Inventory->GetSelectedPoisonId().IsNone()
	);

	return true;
}

#endif
