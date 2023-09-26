// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Ability/GA_InventoryAbility.h"
#include "GA_InventoryCombatAbility.generated.h"

/**
 * 
 */
UCLASS()
class GASACTION_API UGA_InventoryCombatAbility : public UGA_InventoryAbility
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FGameplayEffectSpecHandle GetWeaponEffectSpec(const FHitResult& InHitResult);
};
