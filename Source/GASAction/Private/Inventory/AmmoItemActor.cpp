// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/AmmoItemActor.h"

#include "Inventory/InventoryItemInstance.h"

const UAmmoItemStaticData* AAmmoItemActor::GetAmmoItemStaticData() const
{
	return ItemInstance ? Cast<UAmmoItemStaticData>(ItemInstance->GetItemStaticData()) : nullptr;
}

void AAmmoItemActor::InitInternal()
{
	Super::InitInternal();

	if(const UAmmoItemStaticData* WeaponData = GetAmmoItemStaticData())
	{
		if(WeaponData->StaticMesh)
		{
			UStaticMeshComponent* StaticMeshComponent = NewObject<UStaticMeshComponent>(this, UStaticMeshComponent::StaticClass(), TEXT("MeshComponent"));
			StaticMeshComponent->RegisterComponent();
			StaticMeshComponent->SetStaticMesh(WeaponData->StaticMesh);
			StaticMeshComponent->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::SnapToTargetIncludingScale);

			MeshComponent = StaticMeshComponent;
		}
	}
}
