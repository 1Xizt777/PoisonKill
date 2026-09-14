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
	if (!bRemoved)
	{
		return false;
	}

	const int32 NewCount = GetMaterialCount(MaterialId);
	OnInventoryChanged.Broadcast(MaterialId, NewCount, EPKInventoryItemCategory::Material);

	if (SelectedMaterialId == MaterialId && NewCount <= 0)
	{
		ClearSelectedMaterial();
	}
	return true;
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

		if (GetSelectedItemId().IsNone())
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

	if (SelectedItemCategory == EPKInventoryItemCategory::Poison && SelectedPoisonId == PoisonId)
	{
		return true;
	}

	SelectedPoisonId = PoisonId;
	SelectedMaterialId = NAME_None;
	SelectedItemCategory = EPKInventoryItemCategory::Poison;
	OnSelectedPoisonChanged.Broadcast(SelectedPoisonId);
	BroadcastSelectedItemChanged();
	return true;
}

void UPKInventoryComponent::ClearSelectedPoison()
{
	if (SelectedItemCategory != EPKInventoryItemCategory::Poison)
	{
		return;
	}

	SelectedPoisonId = NAME_None;
	SelectedItemCategory = EPKInventoryItemCategory::None;
	OnSelectedPoisonChanged.Broadcast(SelectedPoisonId);
	BroadcastSelectedItemChanged();
}

bool UPKInventoryComponent::SetSelectedMaterial(const FName MaterialId)
{
	if (MaterialId.IsNone() || !HasMaterial(MaterialId))
	{
		return false;
	}

	if (SelectedItemCategory == EPKInventoryItemCategory::Material && SelectedMaterialId == MaterialId)
	{
		return true;
	}

	SelectedMaterialId = MaterialId;
	SelectedPoisonId = NAME_None;
	SelectedItemCategory = EPKInventoryItemCategory::Material;
	OnSelectedMaterialChanged.Broadcast(SelectedMaterialId);
	BroadcastSelectedItemChanged();
	return true;
}

void UPKInventoryComponent::ClearSelectedMaterial()
{
	if (SelectedItemCategory != EPKInventoryItemCategory::Material)
	{
		return;
	}

	SelectedMaterialId = NAME_None;
	SelectedItemCategory = EPKInventoryItemCategory::None;
	OnSelectedMaterialChanged.Broadcast(SelectedMaterialId);
	BroadcastSelectedItemChanged();
}

bool UPKInventoryComponent::CycleSelectedItem(const int32 Direction)
{
	TArray<TPair<EPKInventoryItemCategory, FName>> Items = GetSelectableItems();
	if (Items.Num() == 0)
	{
		if (SelectedItemCategory != EPKInventoryItemCategory::None)
		{
			SelectedPoisonId = NAME_None;
			SelectedMaterialId = NAME_None;
			SelectedItemCategory = EPKInventoryItemCategory::None;
			OnSelectedPoisonChanged.Broadcast(SelectedPoisonId);
			OnSelectedMaterialChanged.Broadcast(SelectedMaterialId);
			BroadcastSelectedItemChanged();
		}
		return false;
	}

	int32 CurrentIndex = INDEX_NONE;
	for (int32 Index = 0; Index < Items.Num(); ++Index)
	{
		if (Items[Index].Key == SelectedItemCategory &&
			Items[Index].Value == GetSelectedItemId())
		{
			CurrentIndex = Index;
			break;
		}
	}

	if (CurrentIndex == INDEX_NONE)
	{
		CurrentIndex = 0;
	}
	else
	{
		CurrentIndex = (CurrentIndex + (Direction >= 0 ? 1 : -1) + Items.Num()) % Items.Num();
	}

	const TPair<EPKInventoryItemCategory, FName>& Item = Items[CurrentIndex];
	if (Item.Key == EPKInventoryItemCategory::Poison)
	{
		SetSelectedPoison(Item.Value);
	}
	else
	{
		SetSelectedMaterial(Item.Value);
	}
	return true;
}

FName UPKInventoryComponent::GetSelectedPoisonId() const
{
	return SelectedPoisonId;
}

int32 UPKInventoryComponent::GetSelectedPoisonCount() const
{
	return GetPoisonCount(SelectedPoisonId);
}

FName UPKInventoryComponent::GetSelectedMaterialId() const
{
	return SelectedMaterialId;
}

int32 UPKInventoryComponent::GetSelectedMaterialCount() const
{
	return GetMaterialCount(SelectedMaterialId);
}

FName UPKInventoryComponent::GetSelectedItemId() const
{
	return SelectedItemCategory == EPKInventoryItemCategory::Poison
		? SelectedPoisonId
		: SelectedMaterialId;
}

int32 UPKInventoryComponent::GetSelectedItemCount() const
{
	return SelectedItemCategory == EPKInventoryItemCategory::Poison
		? GetSelectedPoisonCount()
		: GetSelectedMaterialCount();
}

int32 UPKInventoryComponent::AddToMap(TMap<FName, int32>& Map, const FName ItemId, const int32 Amount)
{
	if (ItemId.IsNone() || Amount <= 0)
	{
		return -1;
	}

	int32& Count = Map.FindOrAdd(ItemId);
	Count += Amount;
	return Count;
}

bool UPKInventoryComponent::RemoveFromMap(TMap<FName, int32>& Map, const FName ItemId, const int32 Amount)
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

void UPKInventoryComponent::BroadcastSelectedItemChanged()
{
	OnSelectedItemChanged.Broadcast(
		GetSelectedItemId(),
		SelectedItemCategory
	);
}

TArray<TPair<EPKInventoryItemCategory, FName>> UPKInventoryComponent::GetSelectableItems() const
{
	TArray<FName> MaterialIds;
	Materials.GetKeys(MaterialIds);
	MaterialIds.Sort([](const FName& A, const FName& B) { return A.LexicalLess(B); });

	TArray<FName> PoisonIds;
	Poisons.GetKeys(PoisonIds);
	PoisonIds.Sort([](const FName& A, const FName& B) { return A.LexicalLess(B); });

	TArray<TPair<EPKInventoryItemCategory, FName>> Items;
	for (const FName& MaterialId : MaterialIds)
	{
		Items.Add(TPair<EPKInventoryItemCategory, FName>(
			EPKInventoryItemCategory::Material,
			MaterialId
		));
	}

	for (const FName& PoisonId : PoisonIds)
	{
		Items.Add(TPair<EPKInventoryItemCategory, FName>(
			EPKInventoryItemCategory::Poison,
			PoisonId
		));
	}

	return Items;
}