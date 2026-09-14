#include "Effects/PKPoisonAttributionSubsystem.h"

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

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("[PoisonKill] Poison death attributed: Victim=%s Poison=%s Instigator=%s PlayerState=%s Time=%.2f"),
		*GetNameSafe(Record.Victim),
		*Record.PoisonId.ToString(),
		*GetNameSafe(Record.Instigator),
		*GetNameSafe(Record.InstigatorPlayerState),
		Record.WorldTimeSeconds
	);
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