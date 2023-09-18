// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/InventoryItemInstance.h"

#include "AGCommonTypes.h"
#include "Actors/ItemActor.h"
#include "BlueprintFuction/ActionGameStatics.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"

void UInventoryItemInstance::Init(TSubclassOf<UItemStaticData> InItemStaticDataClass)
{
	ItemStaticDataClass = InItemStaticDataClass;
}

void UInventoryItemInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	UObject::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInventoryItemInstance, ItemStaticDataClass);
	DOREPLIFETIME(UInventoryItemInstance, bEquipped);
	DOREPLIFETIME(UInventoryItemInstance, ItemActor);
}

const UItemStaticData* UInventoryItemInstance::GetItemStaticData() const
{
	return UActionGameStatics::GetItemStaticData(ItemStaticDataClass);
}

void UInventoryItemInstance::OnRep_Equipped()
{
	
}

void UInventoryItemInstance::OnEquipped(AActor* InOwner)
{
	if(UWorld* World = InOwner->GetWorld())
	{
		const UItemStaticData* StaticData = GetItemStaticData();
		
		FTransform Transform;
		ItemActor = World->SpawnActorDeferred<AItemActor>(GetItemStaticData()->ItemActorClass, Transform);
		ItemActor->InitItemActor(this);
		ItemActor->OnEquipped();
		ItemActor->FinishSpawning(Transform);

		ACharacter* Character = Cast<ACharacter>(InOwner);
		USkeletalMeshComponent* SkeletalMeshComponent = Character ? Character->GetMesh() : nullptr;

		if(SkeletalMeshComponent)
		{
			ItemActor->AttachToComponent(SkeletalMeshComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale, StaticData->AttachmentSocket);
		}
	}

	bEquipped = true;
}

void UInventoryItemInstance::OnUnequipped()
{
	if(ItemActor)
	{
		ItemActor->Destroy();
		ItemActor = nullptr;
	}

	bEquipped = false;
}

void UInventoryItemInstance::OnDropped()
{
	if(ItemActor)
	{
		ItemActor->OnDropped();
	}

	bEquipped = false;
}
