#if WITH_DEV_AUTOMATION_TESTS

#include "Effects/PKPoisonAttributionSubsystem.h"
#include "GameFramework/Pawn.h"
#include "Misc/AutomationTest.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FPKPoisonAttributionTest,
	"PoisonKill.Attribution.Record",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FPKPoisonAttributionTest::RunTest(const FString& Parameters)
{
	UPKPoisonAttributionSubsystem* Attribution =
		NewObject<UPKPoisonAttributionSubsystem>();
	AActor* Victim = NewObject<AActor>(
		GetTransientPackage(),
		TEXT("AttributionVictim")
	);
	APawn* Instigator = NewObject<APawn>(
		GetTransientPackage(),
		TEXT("AttributionInstigator")
	);

	TestNotNull(TEXT("Attribution subsystem is created"), Attribution);
	TestNotNull(TEXT("Victim is created"), Victim);
	TestNotNull(TEXT("Instigator is created"), Instigator);
	if (!Attribution || !Victim || !Instigator)
	{
		return false;
	}

	Attribution->RecordPoisonDeath(
		Victim,
		TEXT("Poison_Hemlock"),
		Instigator
	);

	TestEqual(TEXT("One death is recorded"), Attribution->GetPoisonKillCount(), 1);
	TestEqual(
		TEXT("Record stores poison id"),
		Attribution->GetPoisonDeathRecords()[0].PoisonId,
		FName(TEXT("Poison_Hemlock"))
	);
	TestTrue(
		TEXT("Record stores victim"),
		Attribution->GetPoisonDeathRecords()[0].Victim == Victim
	);
	TestTrue(
		TEXT("Record stores instigator"),
		Attribution->GetPoisonDeathRecords()[0].Instigator == Instigator
	);
	TestTrue(
		TEXT("Last death attribution can be queried"),
		Attribution->WasLastDeathAttributedTo(Instigator)
	);

	Attribution->ClearDeathRecords();
	TestEqual(
		TEXT("Death records can be cleared"),
		Attribution->GetPoisonKillCount(),
		0
	);
	return true;
}

#endif
