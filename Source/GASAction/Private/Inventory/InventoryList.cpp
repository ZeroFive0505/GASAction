// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/InventoryList.h"
#include "AGCommonTypes.h"
#include "BlueprintFuction/ActionGameStatics.h"
#include "Inventory/InventoryItemInstance.h"

void FInventoryList::AddItem(TSubclassOf<UItemStaticData> InItemStaticData)
{
	FInventoryListItem& Item = Items.AddDefaulted_GetRef();
	Item.ItemInstance = NewObject<UInventoryItemInstance>();
	Item.ItemInstance->Init(InItemStaticData, UActionGameStatics::GetItemStaticData(InItemStaticData)->MaxStackCount);
	MarkItemDirty(Item);
}

void FInventoryList::AddItem(UInventoryItemInstance* InventoryItemInstance)
{
	FInventoryListItem& Item = Items.AddDefaulted_GetRef();
	Item.ItemInstance = InventoryItemInstance;
	MarkItemDirty(Item);
}

void FInventoryList::RemoveItem(TSubclassOf<UItemStaticData> InItemStaticData)
{
	for(auto ItemIter = Items.CreateIterator(); ItemIter; ++ItemIter)
	{
		FInventoryListItem& Item = *ItemIter;

		if(Item.ItemInstance && Item.ItemInstance->GetItemStaticData()->IsA(InItemStaticData))
		{
			ItemIter.RemoveCurrent();
			MarkArrayDirty();
			break;
		}
	}
}

void FInventoryList::RemoveItem(UInventoryItemInstance* InItemInstance)
{
	for(auto ItemIter = Items.CreateIterator(); ItemIter; ++ItemIter)
	{
		FInventoryListItem& Item = *ItemIter;

		if(Item.ItemInstance && Item.ItemInstance == InItemInstance)
		{
			ItemIter.RemoveCurrent();
			MarkArrayDirty();
			break;
		}
	}
}

TArray<UInventoryItemInstance*> FInventoryList::GetAllInstancesWithTag(FGameplayTag InTag)
{
	TArray<UInventoryItemInstance*> OutInstances;

	for(auto ItemIter = Items.CreateIterator(); ItemIter; ++ItemIter)
	{
		FInventoryListItem& Item = *ItemIter;

		if(Item.ItemInstance->GetItemStaticData()->InventoryTags.Contains(InTag))
		{
			OutInstances.Add(Item.ItemInstance);
		}
	}

	return OutInstances;
}

TArray<UInventoryItemInstance*> FInventoryList::GetAllAvailableInstancesOfType(
	TSubclassOf<UItemStaticData> InItemStaticDataClass)
{
	TArray<UInventoryItemInstance*> OutInstances;

	for(auto ItemIter = Items.CreateIterator(); ItemIter; ++ItemIter)
	{
		FInventoryListItem& Item = *ItemIter;

		const UItemStaticData* StaticData = Item.ItemInstance->GetItemStaticData();

		const bool bSameType = StaticData->IsA(InItemStaticDataClass);
		const bool bWasCapacity = StaticData->MaxStackCount > Item.ItemInstance->GetQuantity();

		if(bSameType && bWasCapacity)
		{
			OutInstances.Add(Item.ItemInstance);
		}
	}

	return OutInstances;
}
