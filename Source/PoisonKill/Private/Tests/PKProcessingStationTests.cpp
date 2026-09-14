#if WITH_DEV_AUTOMATION_TESTS

#include "Data/PKGameplayTypes.h"
#include "Items/PKInventoryComponent.h"
#include "Misc/AutomationTest.h"
#include "World/PKProcessingStation.h"

struct FPKProcessingStationTestAccessor
{
	static bool CanApply(const FPKRecipeDefinition& Recipe, const UPKInventoryComponent* Inventory)
	{
		return APKProcessingStation::CanApplyRecipe(Recipe, Inventory);
	}

	static bool Apply(const FPKRecipeDefinition& Recipe, UPKInventoryComponent* Inventory)
	{
		return APKProcessingStation::ApplyRecipe(Recipe, Inventory);
	}
};

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FPKProcessingStationTest,
	"PoisonKill.Processing.Station",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FPKProcessingStationTest::RunTest(const FString& Parameters)
{
	UPKInventoryComponent* Inventory = NewObject<UPKInventoryComponent>();
	TestNotNull(TEXT("Inventory is created"), Inventory);
	if (!Inventory)
	{
		return false;
	}

	FPKRecipeDefinition Recipe;
	Recipe.InputItemId = TEXT("Item_ToxicPlant");
	Recipe.RequiredInputCount = 1;
	Recipe.OutputPoisonId = TEXT("Poison_Hemlock");
	Recipe.ProcessingDuration = 4.0f;

	TestFalse(TEXT("Recipe cannot run without materials"), FPKProcessingStationTestAccessor::CanApply(Recipe, Inventory));
	Inventory->AddMaterial(Recipe.InputItemId, 1);
	TestTrue(TEXT("Recipe can run with required materials"), FPKProcessingStationTestAccessor::CanApply(Recipe, Inventory));
	TestTrue(TEXT("Recipe applies successfully"), FPKProcessingStationTestAccessor::Apply(Recipe, Inventory));
	TestEqual(TEXT("Input material is consumed"), Inventory->GetMaterialCount(Recipe.InputItemId), 0);
	TestEqual(TEXT("Output poison is created"), Inventory->GetPoisonCount(Recipe.OutputPoisonId), 1);
	return true;
}

#endif
