// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorComponent/InventoryComponent.h"

#include "AGCommonTypes.h"
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


// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	bWantsInitializeComponent = true;
	SetIsReplicatedByDefault(true);
	
	// ...
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
}

bool UInventoryComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	for(FInventoryListItem& Item : InventoryList.GetItem())
	{
		UInventoryItemInstance* ItemInstance = Item.ItemInstance;

		if(IsValid(ItemInstance))
		{
			WroteSomething = Channel->ReplicateSubobject(ItemInstance, *Bunch, *RepFlags); 
		}
	}

	return WroteSomething;
}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInventoryComponent, InventoryList);
}


// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...

	const bool bShowDebug = CVarShowInventory.GetValueOnGameThread() != 0;

	if(bShowDebug)
	{
		for(FInventoryListItem& Item : InventoryList.GetItem())
		{
			UInventoryItemInstance* ItemInstance = Item.ItemInstance;
			const UItemStaticData* ItemStaticData = ItemInstance->GetItemStaticData();

			if(IsValid(ItemInstance) && IsValid(ItemStaticData))
			{
				GEngine->AddOnScreenDebugMessage(-1, 0, FColor::Blue, FString::Printf(TEXT("Item : %s"), *ItemStaticData->Name.ToString()));
			}
		}
	}
}

