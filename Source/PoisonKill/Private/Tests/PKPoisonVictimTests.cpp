#if WITH_DEV_AUTOMATION_TESTS

#include "Data/PKGameplayTypes.h"
#include "Effects/PKPoisonVictimComponent.h"
#include "Misc/AutomationTest.h"

struct FPKPoisonVictimTestAccessor
{
	static float ActualThreshold(const FPKPoisonDefinition& Poison, const FPKNpcDefinition& Npc)
	{
		return UPKPoisonVictimComponent::CalculateActualLethalThreshold(Poison, Npc);
	}

	static bool WarningReached(float TotalDose, const FPKPoisonDefinition& Poison, const FPKNpcDefinition& Npc)
	{
		return UPKPoisonVictimComponent::HasReachedWarningThreshold(TotalDose, Poison, Npc);
	}

	static bool LethalReached(float TotalDose, const FPKPoisonDefinition& Poison, const FPKNpcDefinition& Npc)
	{
		return UPKPoisonVictimComponent::HasReachedLethalThreshold(TotalDose, Poison, Npc);
	}
};

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FPKPoisonVictimTest,
	"PoisonKill.PoisonVictim.Rules",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FPKPoisonVictimTest::RunTest(const FString& Parameters)
{
	FPKPoisonDefinition Hemlock;
	Hemlock.SingleDose = 4.0f;
	Hemlock.LethalThreshold = 4.0f;
	Hemlock.IncubationTime = 0.0f;

	FPKPoisonDefinition LeadSugar;
	LeadSugar.SingleDose = 1.0f;
	LeadSugar.LethalThreshold = 4.0f;
	LeadSugar.IncubationTime = 40.0f;

	FPKNpcDefinition TargetNpc;
	TargetNpc.ThresholdCoefficient = 1.0f;
	TargetNpc.WarningThresholdRatio = 0.5f;

	FPKNpcDefinition SecondaryNpc;
	SecondaryNpc.ThresholdCoefficient = 0.5f;
	SecondaryNpc.WarningThresholdRatio = 0.5f;

	TestEqual(
		TEXT("Target NPC lead sugar threshold is four"),
		FPKPoisonVictimTestAccessor::ActualThreshold(LeadSugar, TargetNpc),
		4.0f
	);
	TestEqual(
		TEXT("Secondary NPC lead sugar threshold is two"),
		FPKPoisonVictimTestAccessor::ActualThreshold(LeadSugar, SecondaryNpc),
		2.0f
	);
	TestTrue(
		TEXT("Target NPC warning starts at two doses"),
		FPKPoisonVictimTestAccessor::WarningReached(2.0f, LeadSugar, TargetNpc)
	);
	TestFalse(
		TEXT("Target NPC is not lethal at two doses"),
		FPKPoisonVictimTestAccessor::LethalReached(2.0f, LeadSugar, TargetNpc)
	);
	TestTrue(
		TEXT("Target NPC is lethal at four doses"),
		FPKPoisonVictimTestAccessor::LethalReached(4.0f, LeadSugar, TargetNpc)
	);
	TestTrue(
		TEXT("Hemlock kills with one dose"),
		FPKPoisonVictimTestAccessor::LethalReached(4.0f, Hemlock, TargetNpc)
	);
	TestTrue(
		TEXT("Secondary NPC dies at two lead sugar doses"),
		FPKPoisonVictimTestAccessor::LethalReached(2.0f, LeadSugar, SecondaryNpc)
	);
	return true;
}

#endif
