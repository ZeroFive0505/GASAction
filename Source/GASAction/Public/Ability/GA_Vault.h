// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Ability/AGGameplayAbility.h"
#include "GA_Vault.generated.h"

/**
 * 
 */

class UAbilityTask_PlayMontageAndWait;

UCLASS()
class GASACTION_API UGA_Vault : public UAGGameplayAbility
{
	GENERATED_BODY()

	UGA_Vault();

	virtual bool CommitCheck(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, FGameplayTagContainer* OptionalRelevantTags) override;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "HorizontalTrace")
	float HorizontalTraceRadius = 30.0f;

	UPROPERTY(EditDefaultsOnly, Category = "HorizontalTrace")
	float HorizontalTraceLength = 500.0f;

	UPROPERTY(EditDefaultsOnly, Category = "HorizontalTrace")
	int32 HorizontalTraceCount = 5;

	UPROPERTY(EditDefaultsOnly, Category = "HorizontalTrace")
	float HorizontalTraceStep = 30.0f;

	UPROPERTY(EditDefaultsOnly, Category = "VerticalTrace")
	float VerticalTraceRadius = 30.0f;

	UPROPERTY(EditDefaultsOnly, Category = "VerticalTrace")
	float VerticalTraceStep = 30.0f;

	UPROPERTY(EditDefaultsOnly)
	TArray<TEnumAsByte<EObjectTypeQuery>> TraceObjectTypes;

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* VaultMontage;

	UPROPERTY()
	UAbilityTask_PlayMontageAndWait* MontageTask;

	FVector JumpToLocation;
	FVector JumpOverLocation;

	UPROPERTY(EditDefaultsOnly)
	TArray<TEnumAsByte<ECollisionChannel>> CollisionChannelsToIgnore;
};
