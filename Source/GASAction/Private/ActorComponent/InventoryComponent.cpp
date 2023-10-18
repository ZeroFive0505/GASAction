// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorComponent/InventoryComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AGCommonTypes.h"
#include "GameplayTagsManager.h"
#include "GameplayTasksComponent.h"
#include "IDetailTreeNode.h"
#include "Engine/ActorChannel.h"
#include "Net/UnrealNetwork.h"
#include "Inventory/InventoryList.h"
#include "Inventory/InventoryItemInstance.h"

static TAutoConsoleVariable<int32> CVarShowInventory(
TEXT("ShowDebugInventory"),
	0,
	TEXT("Draws debug info about inventory")
	TEXT("  0: off/n")
	TEXT("  1: on/n"),
	ECVF_Cheat);

FGameplayTag UInventoryComponent::EquipItemActorTag;
FGameplayTag UInventoryComponent::DropItemTag;
FGameplayTag UInventoryComponent::EquipNextTag;
FGameplayTag UInventoryComponent::UnEquipTag;

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	bWantsInitializeComponent = true;
	SetIsReplicatedByDefault(true);

	static bool bHandleAddingTags = false;

	if(!bHandleAddingTags)
	{
		bHandleAddingTags = true;
		UGameplayTagsManager::Get().OnLastChanceToAddNativeTags().AddUObject(this, &UInventoryComponent::AddInventoryTags);
	}
}

void UInventoryComponent::InitializeComponent()
{
	Super::InitializeComponent();

	if(GetOwner()->HasAuthority())
	{
		for(auto ItemClass : DefaultItems)
		{
			InventoryList.AddItem(ItemClass);
		}
	}

	if(UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner()))
	{
		AbilitySystemComponent->GenericGameplayEventCallbacks.FindOrAdd(UInventoryComponent::EquipItemActorTag).AddUObject(this, &UInventoryComponent::GameplayEventCallback);
		AbilitySystemComponent->GenericGameplayEventCallbacks.FindOrAdd(UInventoryComponent::DropItemTag).AddUObject(this, &UInventoryComponent::GameplayEventCallback);
		AbilitySystemComponent->GenericGameplayEventCallbacks.FindOrAdd(UInventoryComponent::EquipNextTag).AddUObject(this, &UInventoryComponent::GameplayEventCallback);
		AbilitySystemComponent->GenericGameplayEventCallbacks.FindOrAdd(UInventoryComponent::UnEquipTag).AddUObject(this, &UInventoryComponent::GameplayEventCallback);
	}
}

bool UInventoryComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	for(FInventoryListItem& Item : InventoryList.GetItemRef())
	{
		UInventoryItemInstance* ItemInstance = Item.ItemInstance;

		if(IsValid(ItemInstance))
		{
			WroteSomething |= Channel->ReplicateSubobject(ItemInstance, *Bunch, *RepFlags); 
		}
	}

	return WroteSomething;
}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInventoryComponent, InventoryList);
	DOREPLIFETIME(UInventoryComponent, CurrentItem);
	DOREPLIFETIME(UInventoryComponent, InventoryTags);
}

void UInventoryComponent::AddItem(TSubclassOf<UItemStaticData> InItemStaticClass)
{
	if(GetOwner()->HasAuthority())
	{
		InventoryList.AddItem(InItemStaticClass);
	}
}

void UInventoryComponent::AddItemInstance(UInventoryItemInstance* InventoryItemInstance)
{
	if(GetOwner()->HasAuthority())
	{
		TArray<UInventoryItemInstance*> Items = InventoryList.GetAllAvailableInstancesOfType(InventoryItemInstance->ItemStaticDataClass);

		Algo::Sort(Items, [](const UInventoryItemInstance* InA, const UInventoryItemInstance* InB)
		{
			return InA->GetQuantity() < InB->GetQuantity();
		});

		const int32 MaxItemStackCount = InventoryItemInstance->GetItemStaticData()->MaxStackCount;

		int32 ItemsLeft = InventoryItemInstance->GetQuantity();

		for(auto& Item : Items)
		{
			const int32 EmptySlots = MaxItemStackCount - Item->GetQuantity();

			int32 SlotsToAdd = ItemsLeft;

			if(ItemsLeft > EmptySlots)
			{
				SlotsToAdd = EmptySlots;
			}

			ItemsLeft -= SlotsToAdd;
			
			Item->AddItem(SlotsToAdd);
			InventoryItemInstance->AddItem(SlotsToAdd);

			for(FGameplayTag InventoryTag : Item->GetItemStaticData()->InventoryTags)
			{
				InventoryTags.AddTagCount(InventoryTag, SlotsToAdd);
			}

			if(ItemsLeft <= 0)
			{
				ItemsLeft = 0;

				return;
			}
		}

		while(ItemsLeft > MaxItemStackCount)
		{
			AddItem(InventoryItemInstance->GetItemStaticData()->GetClass());


			for(FGameplayTag InventoryTag : InventoryItemInstance->GetItemStaticData()->InventoryTags)
			{
				InventoryTags.AddTagCount(InventoryTag, MaxItemStackCount);
			}

			ItemsLeft -= MaxItemStackCount;
			InventoryItemInstance->AddItem(-MaxItemStackCount);
		}

		InventoryList.AddItem(InventoryItemInstance);

		for(FGameplayTag InventoryTag : InventoryItemInstance->GetItemStaticData()->InventoryTags)
		{
			InventoryTags.AddTagCount(InventoryTag, InventoryItemInstance->GetQuantity());
		}
	}
}

void UInventoryComponent::RemoveItemInstance(UInventoryItemInstance* InItemInstance)
{
	if(GetOwner()->HasAuthority())
	{
		InventoryList.RemoveItem(InItemInstance);

		for(FGameplayTag InventoryTag : InItemInstance->GetItemStaticData()->InventoryTags)
		{
			InventoryTags.AddTagCount(InventoryTag, -InItemInstance->GetQuantity());
		}
	}
}

void UInventoryComponent::RemoveItemWithInventoryTag(FGameplayTag Tag, int32 Count)
{
	if(GetOwner()->HasAuthority())
	{
		int32 CountLeft = Count;

		TArray<UInventoryItemInstance*> Items = GetAllInstancesWithTags(Tag);

		Algo::Sort(Items, [](const UInventoryItemInstance* InA, const UInventoryItemInstance* InB)
		{
			return InA->GetQuantity() < InB->GetQuantity();
		});

		for(auto& Item : Items)
		{
			int32 AvailableCount = Item->GetQuantity();
			int32 ItemsToRemove = CountLeft;

			if(ItemsToRemove >= AvailableCount)
			{
				ItemsToRemove = AvailableCount;

				RemoveItemInstance(Item);
			}
			else
			{
				Item->AddItem(-ItemsToRemove);

				for(FGameplayTag InventoryTag : Item->GetItemStaticData()->InventoryTags)
				{
					InventoryTags.AddTagCount(InventoryTag, -ItemsToRemove);
				}
			}

			CountLeft -= ItemsToRemove;
		}
	}
}

void UInventoryComponent::RemoveItem(TSubclassOf<UItemStaticData> InItemStaticClass)
{
	if(GetOwner()->HasAuthority())
	{
		InventoryList.RemoveItem(InItemStaticClass);
	}
}

void UInventoryComponent::EquipItem(TSubclassOf<UItemStaticData> InItemStaticClass)
{
	if(GetOwner()->HasAuthority())
	{
		for(auto Item : InventoryList.GetItemRef())
		{
			if(Item.ItemInstance->ItemStaticDataClass == InItemStaticClass)
			{
				Item.ItemInstance->OnEquipped(GetOwner());
				CurrentItem = Item.ItemInstance;
				break;
			}
		}
	}
}

void UInventoryComponent::EquipItemInstance(UInventoryItemInstance* InventoryItemInstance)
{
	if(GetOwner()->HasAuthority())
	{
		for(auto& Item : InventoryList.GetItemRef())
		{
			if(Item.ItemInstance == InventoryItemInstance)
			{
				Item.ItemInstance->OnEquipped(GetOwner());
				CurrentItem = Item.ItemInstance;
				break;
			}
		}
	}
}

void UInventoryComponent::UnEquipItem()
{
	if(GetOwner()->HasAuthority())
	{
		if(IsValid(CurrentItem))
		{
			CurrentItem->OnUnequipped(GetOwner());
			CurrentItem = nullptr;
		}
	}
}

void UInventoryComponent::DropItem()
{
	if(GetOwner()->HasAuthority())
	{
		if(IsValid(CurrentItem))
		{
			CurrentItem->OnDropped(GetOwner());
			RemoveItem(CurrentItem->ItemStaticDataClass);
			CurrentItem = nullptr;
		}
	}
}

void UInventoryComponent::EquipNext()
{
	TArray<FInventoryListItem>& Items = InventoryList.GetItemRef();

	const bool bNoItems = Items.Num() == 0;
	const bool bOneAndEquipped = Items.Num() ==  1 && CurrentItem;

	if(bNoItems || bOneAndEquipped)
	{
		return;
	}

	UInventoryItemInstance* TargetItem = CurrentItem;

	for(auto& Item : Items)
	{
		if(Item.ItemInstance->GetItemStaticData()->bCanBeEquipped)
		{
			if(Item.ItemInstance != CurrentItem)
			{
				TargetItem = Item.ItemInstance;
				break;
			}
		}
	}

	if(CurrentItem)
	{
		if(TargetItem == CurrentItem)
		{
			return;
		}

		UnEquipItem();
	}

	EquipItemInstance(TargetItem);
}

void UInventoryComponent::GameplayEventCallback(const FGameplayEventData* Payload)
{
	ENetRole NetRole = GetOwnerRole();

	if(NetRole == ROLE_Authority)
	{
		HandleGameplayEventInternal(*Payload);
	}
	else if(NetRole == ROLE_AutonomousProxy)
	{
		ServerHandleGameplayEvent(*Payload);
	}
}

int32 UInventoryComponent::GetInventoryTagCount(FGameplayTag InTag) const
{
	return InventoryTags.GetTagCount(InTag);
}

void UInventoryComponent::AddInventoryTagCount(FGameplayTag InTag, int32 CountDelta)
{
	InventoryTags.AddTagCount(InTag, CountDelta);
}

// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UInventoryComponent::AddInventoryTags()
{
	UGameplayTagsManager& TagsManager = UGameplayTagsManager::Get();

	UInventoryComponent::EquipItemActorTag = TagsManager.AddNativeGameplayTag(TEXT("Event.Inventory.EquipItemActor"), TEXT("Equip item from item actor event"));
	UInventoryComponent::DropItemTag = TagsManager.AddNativeGameplayTag(TEXT("Event.Inventory.DropItem"), TEXT("Drop equipped item"));
	UInventoryComponent::EquipNextTag = TagsManager.AddNativeGameplayTag(TEXT("Event.Inventory.EquipNext"), TEXT("Try to equip next item"));
	UInventoryComponent::UnEquipTag = TagsManager.AddNativeGameplayTag(TEXT("Event.Inventory.UnEquip"), TEXT("Unequip current item"));

	TagsManager.OnLastChanceToAddNativeTags().RemoveAll(this);
}

TArray<UInventoryItemInstance*> UInventoryComponent::GetAllInstancesWithTags(FGameplayTag InTag)
{
	TArray<UInventoryItemInstance*> OutInstances;

	OutInstances = InventoryList.GetAllInstancesWithTag(InTag);

	return OutInstances;
}

void UInventoryComponent::HandleGameplayEventInternal(FGameplayEventData Payload)
{
	ENetRole NetRole = GetOwnerRole();
	
	if(NetRole == ROLE_Authority)
	{
		FGameplayTag EventTag = Payload.EventTag;

		if(EventTag == UInventoryComponent::EquipItemActorTag)
		{
			if(const UInventoryItemInstance* ItemInstance = Cast<UInventoryItemInstance>(Payload.OptionalObject))
			{
				AddItemInstance(const_cast<UInventoryItemInstance*>(ItemInstance));

				if(Payload.Instigator)
				{
					const_cast<AActor*>(ToRawPtr(Payload.Instigator))->Destroy();
				}
			}
		}
		else if(EventTag == UInventoryComponent::EquipNextTag)
		{
			EquipNext();
		}
		else if(EventTag == UInventoryComponent::DropItemTag)
		{
			DropItem();
		}
		else if(EventTag == UInventoryComponent::UnEquipTag)
		{
			UnEquipItem();
		}
	}
}

void UInventoryComponent::ServerHandleGameplayEvent_Implementation(FGameplayEventData Payload)
{
	HandleGameplayEventInternal(Payload);
}

// Called every frame
void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...

	const bool bShowDebug = CVarShowInventory.GetValueOnGameThread() != 0;

	if(bShowDebug)
	{
		for(FInventoryListItem& Item : InventoryList.GetItemRef())
		{
			UInventoryItemInstance* ItemInstance = Item.ItemInstance;
			const UItemStaticData* ItemStaticData = ItemInstance->GetItemStaticData();

			if(IsValid(ItemInstance) && IsValid(ItemStaticData))
			{
				GEngine->AddOnScreenDebugMessage(-1, 0, FColor::Blue, FString::Printf(TEXT("Item : %s"), *ItemStaticData->Name.ToString()));
			}
		}

		const TArray<FFastArrayTagCounterRecord>& InventoryTagArray = InventoryTags.GetTagArray();

		for(auto& TagRecord : InventoryTagArray)
		{
			GEngine->AddOnScreenDebugMessage(-1, 0, FColor::Purple, FString::Printf(TEXT("Tag: %s %d"), *TagRecord.Tag.ToString(), TagRecord.Count));
		}
	}
}

