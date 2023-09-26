// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/WeaponItemActor.h"

#include "NiagaraFunctionLibrary.h"
#include "Inventory/InventoryItemInstance.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicalMaterial/AGPhysicalMaterial.h"

AWeaponItemActor::AWeaponItemActor()
{
}

const UWeaponStaticData* AWeaponItemActor::GetWeaponStaticData() const
{
	return ItemInstance ? Cast<UWeaponStaticData>(ItemInstance->GetItemStaticData()) : nullptr;
}

void AWeaponItemActor::PlayWeaponEffects(const FHitResult& InHitResult)
{
	if(HasAuthority())
	{
		MultiCastPlayWeaponEffects(InHitResult);
	}
	else
	{
		PlayWeaponEffectsInternal(InHitResult);
	}
}

void AWeaponItemActor::MultiCastPlayWeaponEffects_Implementation(const FHitResult& InHitResult)
{
	if(!Owner || Owner->GetLocalRole() != ROLE_AutonomousProxy)
	{
		PlayWeaponEffectsInternal(InHitResult);
	}
}

void AWeaponItemActor::PlayWeaponEffectsInternal(const FHitResult& InHitResult)
{
	if(InHitResult.PhysMaterial.Get())
	{
		UAGPhysicalMaterial* PhysicsMaterial = Cast<UAGPhysicalMaterial>(InHitResult.PhysMaterial.Get());

		if(PhysicsMaterial)
		{
			UGameplayStatics::PlaySoundAtLocation(this, PhysicsMaterial->PointImpactSound, InHitResult.Location, 1.0f);

			UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, PhysicsMaterial->PointImpactParticle, InHitResult.Location);
		}

		if(const UWeaponStaticData* WeaponStaticData = GetWeaponStaticData())
		{
			UGameplayStatics::PlaySoundAtLocation(this, WeaponStaticData->AttackSound, GetActorLocation(), 1.0f);
		}
	}
}

void AWeaponItemActor::InitInternal()
{
	Super::InitInternal();

	if(const UWeaponStaticData* WeaponData = GetWeaponStaticData())
	{
		if(WeaponData->SkeletalMesh)
		{
			USkeletalMeshComponent* SkeletalMeshComponent = NewObject<USkeletalMeshComponent>(this, USkeletalMeshComponent::StaticClass(), TEXT("MeshComponent"));
			if(SkeletalMeshComponent)
			{
				SkeletalMeshComponent->RegisterComponent();
				SkeletalMeshComponent->SetSkeletalMesh(WeaponData->SkeletalMesh);
				SkeletalMeshComponent->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);

				MeshComponent = SkeletalMeshComponent;
			}
		}
		else if(WeaponData->StaticMesh)
		{
			UStaticMeshComponent* StaticMeshComponent = NewObject<UStaticMeshComponent>(this, UStaticMeshComponent::StaticClass(), TEXT("MeshComponent"));
			if(StaticMeshComponent)
			{
				StaticMeshComponent->RegisterComponent();
				StaticMeshComponent->SetStaticMesh(WeaponData->StaticMesh);
				StaticMeshComponent->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);

				MeshComponent = StaticMeshComponent;
			}
		}
	}
}
