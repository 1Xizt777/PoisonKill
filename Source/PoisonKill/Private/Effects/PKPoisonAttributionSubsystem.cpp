#include "Effects/PKPoisonAttributionSubsystem.h"

#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerState.h"

void UPKPoisonAttributionSubsystem::RecordPoisonDeath(AActor* Victim, FName PoisonId, AActor* Instigator)
{
	FPKPoisonDeathRecord Record;
	Record.Victim = Victim;
	Record.PoisonId = PoisonId;
	Record.Instigator = Instigator;
	Record.WorldTimeSeconds = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0f;

	if (const APawn* InstigatorPawn = Cast<APawn>(Instigator))
	{
		Record.InstigatorPlayerState = InstigatorPawn->GetPlayerState();
	}

	DeathRecords.Add(Record);

	if (GEngine)
	{
		const FString Message = FString::Printf(
			TEXT("[PoisonKill] Poison death attributed: Victim=%s Poison=%s Instigator=%s PlayerState=%s Time=%.2f"),
			*GetNameSafe(Record.Victim),
			*Record.PoisonId.ToString(),
			*GetNameSafe(Record.Instigator),
			*GetNameSafe(Record.InstigatorPlayerState),
			Record.WorldTimeSeconds
		);

		GEngine->AddOnScreenDebugMessage(1001, 6.0f, FColor::Yellow, Message);
		GEngine->AddOnScreenDebugMessage(1002, 6.0f, FColor::Cyan, Message);
		GEngine->AddOnScreenDebugMessage(1003, 6.0f, FColor::Green, Message);
	}
}

void UPKPoisonAttributionSubsystem::ClearDeathRecords()
{
	DeathRecords.Reset();
}

bool UPKPoisonAttributionSubsystem::WasLastDeathAttributedTo(AActor* Instigator) const
{
	return DeathRecords.Num() > 0 &&
		DeathRecords.Last().Instigator == Instigator;
}