#include "Items/PKInventoryComponent.h"

UPKInventoryComponent::UPKInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPKInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	
	//如果InitialMaterials有内容就直接加入实际材料背包
	for (const TPair<FName, int32>& Entry : InitialMaterials) 
	{
		if (!Entry.Key.IsNone() && Entry.Value > 0)
		{
			AddMaterial(Entry.Key, Entry.Value);
		}
	}

	//如果InitialPoisons有内容就直接加入实际毒物背包
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
	const int32 NewCount = AddToMap(Materials, MaterialId, Amount);		//添加至Materials
	if (NewCount >= 0)
	{
		OnInventoryChanged.Broadcast(MaterialId, NewCount, EPKInventoryItemCategory::Material);	//广播给表现层
	}
	return NewCount;
}

bool UPKInventoryComponent::RemoveMaterial(const FName MaterialId, const int32 Amount)
{
	const bool bRemoved = RemoveFromMap(Materials, MaterialId, Amount);		//从Materials减少数量
	if (bRemoved)
	{
		OnInventoryChanged.Broadcast(MaterialId,GetMaterialCount(MaterialId),EPKInventoryItemCategory::Material	//广播给表现层
		);
	}
	return bRemoved;
}

bool UPKInventoryComponent::HasMaterial(const FName MaterialId, const int32 Amount) const
{
	return Amount > 0 && GetMaterialCount(MaterialId) >= Amount;	//数量大于0 且 大于所需Amount
}

int32 UPKInventoryComponent::GetMaterialCount(const FName MaterialId) const
{
	const int32* Count = Materials.Find(MaterialId);	//从实际背包查数量
	return Count ? *Count : 0;
}

const TMap<FName, int32>& UPKInventoryComponent::GetMaterials() const
{
	return Materials;	//返回实际材料背包
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

	SelectedPoisonId = PoisonId;		//所持毒物ID
	OnSelectedPoisonChanged.Broadcast(SelectedPoisonId);	//广播
	return true;
}

void UPKInventoryComponent::ClearSelectedPoison()
{
	if (SelectedPoisonId.IsNone())
	{
		return;
	}

	SelectedPoisonId = NAME_None;		//清除所持毒物ID为None
	OnSelectedPoisonChanged.Broadcast(SelectedPoisonId);	//广播
}

FName UPKInventoryComponent::GetSelectedPoisonId() const
{
	return SelectedPoisonId;	//返回所持毒物ID
}

int32 UPKInventoryComponent::GetSelectedPoisonCount() const
{
	return GetPoisonCount(SelectedPoisonId);	//返回所持毒物数量
}

int32 UPKInventoryComponent::AddToMap(TMap<FName, int32>& Map,const FName ItemId,const int32 Amount)
{
	if (ItemId.IsNone() || Amount <= 0)
	{
		return -1;
	}

	int32& Count = Map.FindOrAdd(ItemId);
	Count += Amount;
	return Count;
}

bool UPKInventoryComponent::RemoveFromMap(TMap<FName, int32>& Map,const FName ItemId,const int32 Amount)
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
	if (*Count <= 0)	//如果数量<=0
	{
		Map.Remove(ItemId);		//直接从Map移除
	}
	return true;
}
