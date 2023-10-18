// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "InventoryList.generated.h"

class UItemStaticData;
class UInventoryItemInstance;

USTRUCT(BlueprintType)
struct FInventoryListItem : public FFastArraySerializerItem
{
	GENERATED_BODY()

public:
	UPROPERTY()
	UInventoryItemInstance* ItemInstance = nullptr;
};

USTRUCT(BlueprintType)
struct FInventoryList : public FFastArraySerializer
{
	GENERATED_BODY()

	bool NetDeltaSerialization(FNetDeltaSerializeInfo& DeltaParams)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FInventoryListItem, FInventoryList>(Items, DeltaParams, *this);
	}

	void AddItem(TSubclassOf<UItemStaticData> InItemStaticData);
	void AddItem(UInventoryItemInstance* InventoryItemInstance);
	
	void RemoveItem(TSubclassOf<UItemStaticData> InItemStaticData);
	void RemoveItem(UInventoryItemInstance* InItemInstance);

	TArray<FInventoryListItem>& GetItemRef() { return Items; }

	TArray<UInventoryItemInstance*> GetAllInstancesWithTag(FGameplayTag InTag);

	TArray<UInventoryItemInstance*> GetAllAvailableInstancesOfType(TSubclassOf<UItemStaticData> InItemStaticDataClass);
	
protected:
	UPROPERTY()
	TArray<FInventoryListItem> Items;
};

template<>
struct TStructOpsTypeTraits<FInventoryList> : public TStructOpsTypeTraitsBase2<FInventoryList>
{
	enum {WithDeltaSerializer = true};
};
