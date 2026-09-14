#if WITH_DEV_AUTOMATION_TESTS

#include "Data/PKGameplayTypes.h"
#include "Misc/AutomationTest.h"
#include "World/PKCarrierActor.h"

struct FPKCarrierActorTestAccessor
{
	static bool IsAllowed(const FPKPoisonDefinition& Poison, const FPKCarrierDefinition& Carrier)
	{
		return APKCarrierActor::IsPoisonAllowed(Poison, Carrier);
	}

	static void Apply(APKCarrierActor& CarrierState, FName PoisonId, float SingleDose, int32 ResidueHits)
	{
		CarrierState.ApplyPayload(PoisonId, SingleDose, ResidueHits);
	}
};

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FPKCarrierActorTest,
	"PoisonKill.Carrier.Actor",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FPKCarrierActorTest::RunTest(const FString& Parameters)
{
	FPKCarrierDefinition IngestionCarrier;
	IngestionCarrier.CarrierType = EPKCarrierType::Ingestion;
	IngestionCarrier.DefaultResidueHits = 1;

	FPKCarrierDefinition ContactCarrier;
	ContactCarrier.CarrierType = EPKCarrierType::Contact;
	ContactCarrier.DoseCoefficient = 0.5f;
	ContactCarrier.DefaultResidueHits = 3;

	FPKPoisonDefinition Hemlock;
	Hemlock.SingleDose = 4.0f;
	Hemlock.AllowedCarrierTypes = { EPKCarrierType::Ingestion };

	FPKPoisonDefinition LeadSugar;
	LeadSugar.SingleDose = 1.0f;
	LeadSugar.AllowedCarrierTypes = {
		EPKCarrierType::Ingestion,
		EPKCarrierType::Contact
	};

	TestTrue(
		TEXT("Hemlock can be applied to ingestion carriers"),
		FPKCarrierActorTestAccessor::IsAllowed(Hemlock, IngestionCarrier)
	);
	TestFalse(
		TEXT("Hemlock cannot be applied to contact carriers"),
		FPKCarrierActorTestAccessor::IsAllowed(Hemlock, ContactCarrier)
	);
	TestTrue(
		TEXT("Lead sugar can be applied to both carrier types"),
		FPKCarrierActorTestAccessor::IsAllowed(LeadSugar, ContactCarrier)
	);

	APKCarrierActor* Carrier = NewObject<APKCarrierActor>(
		GetTransientPackage(),
		TEXT("CarrierTestActor")
	);
	TestNotNull(TEXT("Carrier is created"), Carrier);
	if (!Carrier)
	{
		return false;
	}

	FPKCarrierActorTestAccessor::Apply(
		*Carrier,
		TEXT("Poison_LeadSugar"),
		1.0f,
		3
	);
	FPKCarrierActorTestAccessor::Apply(
		*Carrier,
		TEXT("Poison_LeadSugar"),
		1.0f,
		3
	);
	TestEqual(
		TEXT("Repeated poison application accumulates dose"),
		Carrier->GetPayload().RemainingDose,
		2.0f
	);

	FPKCarrierActorTestAccessor::Apply(
		*Carrier,
		TEXT("Poison_Hemlock"),
		4.0f,
		1
	);
	TestEqual(
		TEXT("Different poison overwrites the payload"),
		Carrier->GetPayload().PoisonId,
		FName(TEXT("Poison_Hemlock"))
	);
	TestEqual(
		TEXT("Residue count resets from carrier config"),
		Carrier->GetPayload().RemainingResidueHits,
		1
	);

	Carrier->ClearPayload();
	TestFalse(TEXT("Carrier payload can be cleared"), Carrier->HasPayload());
	return true;
}

#endif
