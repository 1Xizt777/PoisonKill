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

	static bool Detect(const FPKPoisonDefinition& Poison, const FPKCarrierDefinition& Carrier)
	{
		return APKCarrierActor::ShouldDetectPoison(Poison, Carrier);
	}

	static float ContactDose(const FPKPoisonDefinition& Poison, const FPKCarrierDefinition& Carrier)
	{
		return APKCarrierActor::CalculateContactDose(Poison, Carrier);
	}

	static void Apply(APKCarrierActor& CarrierState, FName PoisonId, float SingleDose, int32 ResidueHits, AActor* Instigator)
	{
		CarrierState.ApplyPayload(PoisonId, SingleDose, ResidueHits, Instigator);
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

	FPKCarrierDefinition ClearIngestionCarrier;
	ClearIngestionCarrier.CarrierType = EPKCarrierType::Ingestion;
	ClearIngestionCarrier.bConcealsVisuals = false;

	FPKCarrierDefinition ConcealingIngestionCarrier;
	ConcealingIngestionCarrier.CarrierType = EPKCarrierType::Ingestion;
	ConcealingIngestionCarrier.bConcealsVisuals = true;

	FPKCarrierDefinition ContactCarrier;
	ContactCarrier.CarrierType = EPKCarrierType::Contact;
	ContactCarrier.DoseCoefficient = 0.5f;
	ContactCarrier.DefaultResidueHits = 3;

	FPKPoisonDefinition Hemlock;
	Hemlock.SingleDose = 4.0f;
	Hemlock.VisualSignificance = EPKVisualSignificance::High;
	Hemlock.AllowedCarrierTypes = { EPKCarrierType::Ingestion };

	FPKPoisonDefinition LeadSugar;
	LeadSugar.SingleDose = 1.0f;
	LeadSugar.VisualSignificance = EPKVisualSignificance::Low;
	LeadSugar.AllowedCarrierTypes = {
		EPKCarrierType::Ingestion,
		EPKCarrierType::Contact
	};

	TestTrue(TEXT("Hemlock can be applied to ingestion carriers"), FPKCarrierActorTestAccessor::IsAllowed(Hemlock, IngestionCarrier));
	TestFalse(TEXT("Hemlock cannot be applied to contact carriers"), FPKCarrierActorTestAccessor::IsAllowed(Hemlock, ContactCarrier));
	TestTrue(TEXT("Lead sugar can be applied to both carrier types"), FPKCarrierActorTestAccessor::IsAllowed(LeadSugar, ContactCarrier));
	TestTrue(TEXT("Hemlock is detected in clear wine cup"), FPKCarrierActorTestAccessor::Detect(Hemlock, ClearIngestionCarrier));
	TestFalse(TEXT("Hemlock is concealed in soup pot"), FPKCarrierActorTestAccessor::Detect(Hemlock, ConcealingIngestionCarrier));
	TestFalse(TEXT("Lead sugar is not detected in clear wine cup"), FPKCarrierActorTestAccessor::Detect(LeadSugar, ClearIngestionCarrier));
	TestEqual(TEXT("Contact dose uses carrier coefficient"), FPKCarrierActorTestAccessor::ContactDose(LeadSugar, ContactCarrier), 0.5f);

	APKCarrierActor* Carrier = NewObject<APKCarrierActor>(
		GetTransientPackage(),
		TEXT("CarrierTestActor")
	);
	TestNotNull(TEXT("Carrier is created"), Carrier);
	if (!Carrier)
	{
		return false;
	}

	FPKCarrierActorTestAccessor::Apply(*Carrier, TEXT("Poison_LeadSugar"), 1.0f, 3, nullptr);
	FPKCarrierActorTestAccessor::Apply(*Carrier, TEXT("Poison_LeadSugar"), 1.0f, 3, nullptr);
	TestEqual(TEXT("Repeated poison application accumulates dose"), Carrier->GetPayload().RemainingDose, 2.0f);

	FPKCarrierActorTestAccessor::Apply(*Carrier, TEXT("Poison_Hemlock"), 4.0f, 1, nullptr);
	TestEqual(TEXT("Different poison overwrites the payload"), Carrier->GetPayload().PoisonId, FName(TEXT("Poison_Hemlock")));
	TestEqual(TEXT("Residue count resets from carrier config"), Carrier->GetPayload().RemainingResidueHits, 1);

	Carrier->ClearPayload();
	TestFalse(TEXT("Carrier payload can be cleared"), Carrier->HasPayload());
	return true;
}

#endif