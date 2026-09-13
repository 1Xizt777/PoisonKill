#include "Items/PKInventoryComponent.h"

UPKInventoryComponent::UPKInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPKInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	for (const TPair<FName, int32>& Entry : InitialMaterials)
	{
		if (!Entry.Key.IsNone() && Entry.Value > 0)
		{
			AddMaterial(Entry.Key, Entry.Value);
		}
	}

	for (const TPair<FName, int32>& Entry : InitialPoisons)
	{
		if (!Entry.Key.IsNone() && Entry.Value > 0)
		{
			AddPoison(Entry.Key, Entry.Value);
		}
	}
}

int32 UPKInventoryComponent::AddMaterial(const FName MaterialId, const int32 Amount)
{
	const int32 NewCount = AddToMap(Materials, MaterialId, Amount);
	if (NewCount >= 0)
	{
		OnInventoryChanged.Broadcast(MaterialId, NewCount, EPKInventoryItemCategory::Material);
	}
	return NewCount;
}

bool UPKInventoryComponent::RemoveMaterial(const FName MaterialId, const int32 Amount)
{
	const bool bRemoved = RemoveFromMap(Materials, MaterialId, Amount);
	if (bRemoved)
	{
		OnInventoryChanged.Broadcast(
			MaterialId,
			GetMaterialCount(MaterialId),
			EPKInventoryItemCategory::Material
		);
	}
	return bRemoved;
}

bool UPKInventoryComponent::HasMaterial(const FName MaterialId, const int32 Amount) const
{
	return Amount > 0 && GetMaterialCount(MaterialId) >= Amount;
}

int32 UPKInventoryComponent::GetMaterialCount(const FName MaterialId) const
{
	const int32* Count = Materials.Find(MaterialId);
	return Count ? *Count : 0;
}

const TMap<FName, int32>& UPKInventoryComponent::GetMaterials() const
{
	return Materials;
}

int32 UPKInventoryComponent::AddPoison(const FName PoisonId, const int32 Amount)
{
	const int32 NewCount = AddToMap(Poisons, PoisonId, Amount);
	if (NewCount >= 0)
	{
		OnInventoryChanged.Broadcast(PoisonId, NewCount, EPKInventoryItemCategory::Poison);

		if (SelectedPoisonId.IsNone())
		{
			SetSelectedPoison(PoisonId);
		}
	}
	return NewCount;
}

bool UPKInventoryComponent::RemovePoison(const FName PoisonId, const int32 Amount)
{
	const bool bRemoved = RemoveFromMap(Poisons, PoisonId, Amount);
	if (!bRemoved)
	{
		return false;
	}

	const int32 NewCount = GetPoisonCount(PoisonId);
	OnInventoryChanged.Broadcast(PoisonId, NewCount, EPKInventoryItemCategory::Poison);

	if (SelectedPoisonId == PoisonId && NewCount <= 0)
	{
		ClearSelectedPoison();
	}
	return true;
}

bool UPKInventoryComponent::HasPoison(const FName PoisonId, const int32 Amount) const
{
	return Amount > 0 && GetPoisonCount(PoisonId) >= Amount;
}

int32 UPKInventoryComponent::GetPoisonCount(const FName PoisonId) const
{
	const int32* Count = Poisons.Find(PoisonId);
	return Count ? *Count : 0;
}

const TMap<FName, int32>& UPKInventoryComponent::GetPoisons() const
{
	return Poisons;
}

bool UPKInventoryComponent::SetSelectedPoison(const FName PoisonId)
{
	if (PoisonId.IsNone() || !HasPoison(PoisonId))
	{
		return false;
	}

	if (SelectedPoisonId == PoisonId)
	{
		return true;
	}

	SelectedPoisonId = PoisonId;
	OnSelectedPoisonChanged.Broadcast(SelectedPoisonId);
	return true;
}

void UPKInventoryComponent::ClearSelectedPoison()
{
	if (SelectedPoisonId.IsNone())
	{
		return;
	}

	SelectedPoisonId = NAME_None;
	OnSelectedPoisonChanged.Broadcast(SelectedPoisonId);
}

FName UPKInventoryComponent::GetSelectedPoisonId() const
{
	return SelectedPoisonId;
}

int32 UPKInventoryComponent::GetSelectedPoisonCount() const
{
	return GetPoisonCount(SelectedPoisonId);
}

int32 UPKInventoryComponent::AddToMap(
	TMap<FName, int32>& Map,
	const FName ItemId,
	const int32 Amount
)
{
	if (ItemId.IsNone() || Amount <= 0)
	{
		return -1;
	}

	int32& Count = Map.FindOrAdd(ItemId);
	Count += Amount;
	return Count;
}

bool UPKInventoryComponent::RemoveFromMap(
	TMap<FName, int32>& Map,
	const FName ItemId,
	const int32 Amount
)
{
	if (ItemId.IsNone() || Amount <= 0)
	{
		return false;
	}

	int32* Count = Map.Find(ItemId);
	if (!Count || *Count < Amount)
	{
		return false;
	}

	*Count -= Amount;
	if (*Count <= 0)
	{
		Map.Remove(ItemId);
	}
	return true;
}
