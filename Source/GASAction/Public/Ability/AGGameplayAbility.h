// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "AGGameplayAbility.generated.h"

class AAGActionCharacter;
/**
 * 
 */
UCLASS()
class GASACTION_API UAGGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	TArray<TSubclassOf<UGameplayEffect>> OnGoingEffectsToRemoveOnEnd;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	TArray<TSubclassOf<UGameplayEffect>> OnGoingEffectsToJustApplyOnStart;

	TArray<FActiveGameplayEffectHandle> RemoveOnEndEffectHandles;

	UFUNCTION(BlueprintPure, BlueprintCallable)
	AAGActionCharacter* GetActionGameCharacterFromActorInfo() const;
};
