// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/ItemActor.h"
#include "WeaponItemActor.generated.h"

/**
 * 
 */
UCLASS()
class GASACTION_API AWeaponItemActor : public AItemActor
{
	GENERATED_BODY()

	AWeaponItemActor();

	const UWeaponStaticData* GetWeaponStaticData() const;

public:
	UFUNCTION(BlueprintPure)
	FORCEINLINE FVector GetMuzzlePosition() const { return MeshComponent ? MeshComponent->GetSocketLocation(TEXT("Muzzle")) : GetActorLocation(); }

	UFUNCTION(BlueprintCallable)
	void PlayWeaponEffects(const FHitResult& InHitResult);

protected:
	UFUNCTION(NetMulticast, Reliable)
	void MultiCastPlayWeaponEffects(const FHitResult& InHitResult);

	void PlayWeaponEffectsInternal(const FHitResult& InHitResult);
	
	UPROPERTY()
	UMeshComponent* MeshComponent = nullptr;

	virtual void InitInternal() override;
};
