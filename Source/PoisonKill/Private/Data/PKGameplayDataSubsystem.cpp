#include "Data/PKGameplayDataSubsystem.h"

#include "Data/PKGameplayTypes.h"
#include "Engine/DataTable.h"

UPKGameplayDataSubsystem::UPKGameplayDataSubsystem()
{
	PoisonTablePath = FSoftObjectPath(TEXT("/Game/PoisonKill/Data/DT_Poisons.DT_Poisons"));
	CarrierTablePath = FSoftObjectPath(TEXT("/Game/PoisonKill/Data/DT_Carriers.DT_Carriers"));
	NpcTablePath = FSoftObjectPath(TEXT("/Game/PoisonKill/Data/DT_NPCs.DT_NPCs"));
	RecipeTablePath = FSoftObjectPath(TEXT("/Game/PoisonKill/Data/DT_Recipes.DT_Recipes"));
}

void UPKGameplayDataSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	LoadTable(PoisonTablePath, PoisonTable, TEXT("Poison"));
	LoadTable(CarrierTablePath, CarrierTable, TEXT("Carrier"));
	LoadTable(NpcTablePath, NpcTable, TEXT("NPC"));
	LoadTable(RecipeTablePath, RecipeTable, TEXT("Recipe"));
}

bool UPKGameplayDataSubsystem::GetPoisonDefinition(
	const FName RowName,
	FPKPoisonDefinition& OutDefinition
) const
{
	if (!PoisonTable)
	{
		return false;
	}

	const FPKPoisonDefinition* Row = PoisonTable->FindRow<FPKPoisonDefinition>(
		RowName,
		TEXT("UPKGameplayDataSubsystem::GetPoisonDefinition"),
		false
	);
	if (!Row)
	{
		return false;
	}

	OutDefinition = *Row;
	return true;
}

bool UPKGameplayDataSubsystem::GetCarrierDefinition(
	const FName RowName,
	FPKCarrierDefinition& OutDefinition
) const
{
	if (!CarrierTable)
	{
		return false;
	}

	const FPKCarrierDefinition* Row = CarrierTable->FindRow<FPKCarrierDefinition>(
		RowName,
		TEXT("UPKGameplayDataSubsystem::GetCarrierDefinition"),
		false
	);
	if (!Row)
	{
		return false;
	}

	OutDefinition = *Row;
	return true;
}

bool UPKGameplayDataSubsystem::GetNpcDefinition(
	const FName RowName,
	FPKNpcDefinition& OutDefinition
) const
{
	if (!NpcTable)
	{
		return false;
	}

	const FPKNpcDefinition* Row = NpcTable->FindRow<FPKNpcDefinition>(
		RowName,
		TEXT("UPKGameplayDataSubsystem::GetNpcDefinition"),
		false
	);
	if (!Row)
	{
		return false;
	}

	OutDefinition = *Row;
	return true;
}

bool UPKGameplayDataSubsystem::GetRecipeDefinition(
	const FName RowName,
	FPKRecipeDefinition& OutDefinition
) const
{
	if (!RecipeTable)
	{
		return false;
	}

	const FPKRecipeDefinition* Row = RecipeTable->FindRow<FPKRecipeDefinition>(
		RowName,
		TEXT("UPKGameplayDataSubsystem::GetRecipeDefinition"),
		false
	);
	if (!Row)
	{
		return false;
	}

	OutDefinition = *Row;
	return true;
}

bool UPKGameplayDataSubsystem::GetRecipeForPoison(
	const FName OutputPoisonId,
	FPKRecipeDefinition& OutDefinition
) const
{
	if (!RecipeTable)
	{
		return false;
	}

	for (const FName& RowName : RecipeTable->GetRowNames())
	{
		FPKRecipeDefinition Row;
		if (GetRecipeDefinition(RowName, Row) && Row.OutputPoisonId == OutputPoisonId)
		{
			OutDefinition = Row;
			return true;
		}
	}

	return false;
}

TArray<FName> UPKGameplayDataSubsystem::GetPoisonRowNames() const
{
	return PoisonTable ? PoisonTable->GetRowNames() : TArray<FName>();
}

TArray<FName> UPKGameplayDataSubsystem::GetCarrierRowNames() const
{
	return CarrierTable ? CarrierTable->GetRowNames() : TArray<FName>();
}

TArray<FName> UPKGameplayDataSubsystem::GetNpcRowNames() const
{
	return NpcTable ? NpcTable->GetRowNames() : TArray<FName>();
}

TArray<FName> UPKGameplayDataSubsystem::GetRecipeRowNames() const
{
	return RecipeTable ? RecipeTable->GetRowNames() : TArray<FName>();
}

bool UPKGameplayDataSubsystem::LoadTable(
	const FSoftObjectPath& Path,
	TObjectPtr<UDataTable>& OutTable,
	const TCHAR* DebugName
)
{
	if (!Path.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("%s DataTable path is not configured."), DebugName);
		return false;
	}

	OutTable = Cast<UDataTable>(Path.TryLoad());
	if (!OutTable)
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("%s DataTable could not be loaded from '%s'."),
			DebugName,
			*Path.ToString()
		);
		return false;
	}

	UE_LOG(
		LogTemp,
		Log,
		TEXT("%s DataTable loaded from '%s' with %d rows."),
		DebugName,
		*Path.ToString(),
		OutTable->GetRowNames().Num()
	);
	return true;
}
