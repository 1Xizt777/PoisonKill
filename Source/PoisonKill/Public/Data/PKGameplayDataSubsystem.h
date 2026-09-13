#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "PKGameplayDataSubsystem.generated.h"

class UDataTable;
struct FPKCarrierDefinition;
struct FPKNpcDefinition;
struct FPKPoisonDefinition;
struct FPKRecipeDefinition;

UCLASS(Config = Game)
class POISONKILL_API UPKGameplayDataSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UPKGameplayDataSubsystem();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION(BlueprintCallable, Category = "PK|Data")
	bool GetPoisonDefinition(FName RowName, FPKPoisonDefinition& OutDefinition) const;

	UFUNCTION(BlueprintCallable, Category = "PK|Data")
	bool GetCarrierDefinition(FName RowName, FPKCarrierDefinition& OutDefinition) const;

	UFUNCTION(BlueprintCallable, Category = "PK|Data")
	bool GetNpcDefinition(FName RowName, FPKNpcDefinition& OutDefinition) const;

	UFUNCTION(BlueprintCallable, Category = "PK|Data")
	bool GetRecipeDefinition(FName RowName, FPKRecipeDefinition& OutDefinition) const;

	UFUNCTION(BlueprintCallable, Category = "PK|Data")
	bool GetRecipeForPoison(FName OutputPoisonId, FPKRecipeDefinition& OutDefinition) const;

	UFUNCTION(BlueprintPure, Category = "PK|Data")
	TArray<FName> GetPoisonRowNames() const;

	UFUNCTION(BlueprintPure, Category = "PK|Data")
	TArray<FName> GetCarrierRowNames() const;

	UFUNCTION(BlueprintPure, Category = "PK|Data")
	TArray<FName> GetNpcRowNames() const;

	UFUNCTION(BlueprintPure, Category = "PK|Data")
	TArray<FName> GetRecipeRowNames() const;

private:
	UPROPERTY(Config, EditAnywhere, Category = "PK|Data")
	FSoftObjectPath PoisonTablePath;

	UPROPERTY(Config, EditAnywhere, Category = "PK|Data")
	FSoftObjectPath CarrierTablePath;

	UPROPERTY(Config, EditAnywhere, Category = "PK|Data")
	FSoftObjectPath NpcTablePath;

	UPROPERTY(Config, EditAnywhere, Category = "PK|Data")
	FSoftObjectPath RecipeTablePath;

	UPROPERTY(Transient)
	TObjectPtr<UDataTable> PoisonTable;

	UPROPERTY(Transient)
	TObjectPtr<UDataTable> CarrierTable;

	UPROPERTY(Transient)
	TObjectPtr<UDataTable> NpcTable;

	UPROPERTY(Transient)
	TObjectPtr<UDataTable> RecipeTable;

	bool LoadTable(const FSoftObjectPath& Path, TObjectPtr<UDataTable>& OutTable, const TCHAR* DebugName);
};
