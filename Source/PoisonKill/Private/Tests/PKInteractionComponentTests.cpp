#if WITH_DEV_AUTOMATION_TESTS

#include "Interaction/PKInteractionComponent.h"
#include "Misc/AutomationTest.h"
#include "Tests/PKInteractionTestActor.h"

struct FPKInteractionComponentTestAccessor
{
	static void Advance(
		UPKInteractionComponent& Component,
		float DeltaTime
	)
	{
		Component.TickComponent(DeltaTime, LEVELTICK_All, nullptr);
	}

	static void SetTarget(
		UPKInteractionComponent& Component,
		AActor* Target,
		const FPKInteractionRequest& Request
	)
	{
		Component.SetCurrentTarget(Target, Request);
	}
};

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FPKInteractionComponentTest,
	"PoisonKill.Interaction.Component",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FPKInteractionComponentTest::RunTest(const FString& Parameters)
{
	UPKInteractionComponent* Component = NewObject<UPKInteractionComponent>();
	APKInteractionTestActor* Target = NewObject<APKInteractionTestActor>();
	TestNotNull(TEXT("Interaction component is created"), Component);
	TestNotNull(TEXT("Interaction target is created"), Target);
	if (!Component || !Target)
	{
		return false;
	}

	Target->Request.HoldDuration = 1.0f;
	Target->Request.bRetainProgressOnRelease = false;
	FPKInteractionComponentTestAccessor::SetTarget(
		*Component,
		Target,
		Target->Request
	);

	TestTrue(TEXT("Interaction starts"), Component->BeginInteraction());
	TestEqual(TEXT("Started callback runs once"), Target->StartCount, 1);

	FPKInteractionComponentTestAccessor::Advance(*Component, 0.6f);
	TestEqual(TEXT("Progress callback runs"), Target->ProgressCount, 1);
	TestEqual(TEXT("Partial progress is correct"), Component->GetNormalizedProgress(), 0.6f);

	Component->EndInteraction();
	TestEqual(TEXT("Cancel callback runs"), Target->CanceledCount, 1);
	TestEqual(TEXT("Canceled progress is reset"), Component->GetNormalizedProgress(), 0.0f);

	TestTrue(TEXT("Interaction restarts"), Component->BeginInteraction());
	FPKInteractionComponentTestAccessor::Advance(*Component, 1.0f);
	TestEqual(TEXT("Completed callback runs"), Target->CompletedCount, 1);
	TestFalse(TEXT("Interaction is no longer pressed"), Component->IsInteractionPressed());

	Target->Request.HoldDuration = 1.0f;
	Target->Request.bRetainProgressOnRelease = true;
	FPKInteractionComponentTestAccessor::SetTarget(
		*Component,
		Target,
		Target->Request
	);

	TestTrue(TEXT("Retained interaction starts"), Component->BeginInteraction());
	FPKInteractionComponentTestAccessor::Advance(*Component, 0.4f);
	Component->EndInteraction();
	TestTrue(TEXT("Retained interaction restarts"), Component->BeginInteraction());
	TestEqual(TEXT("Retained progress is restored"), Component->GetNormalizedProgress(), 0.4f);

	FPKInteractionComponentTestAccessor::Advance(*Component, 0.6f);
	TestEqual(TEXT("Retained interaction eventually completes"), Target->CompletedCount, 2);

	return true;
}

#endif
