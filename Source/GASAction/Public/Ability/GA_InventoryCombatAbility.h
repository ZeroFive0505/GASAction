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
	virtual bool CommitAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, FGameplayTagContainer* OptionalRelevantTags) override;
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FGameplayEffectSpecHandle GetWeaponEffectSpec(const FHitResult& InHitResult);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool GetWeaponToFocusTraceResult(float TraceDistance, ETraceTypeQuery TraceTypeQuery, FHitResult& OutHitResult);

	UFUNCTION(BlueprintPure)
	bool HasEnoughAmmo() const;

	UFUNCTION(BlueprintCallable)
	void DecreaseAmmo();
};
