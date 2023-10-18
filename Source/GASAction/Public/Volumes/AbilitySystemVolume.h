// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySpecHandle.h"
#include "ActiveGameplayEffectHandle.h"
#include "GameFramework/PhysicsVolume.h"
#include "AbilitySystemVolume.generated.h"

class UGameplayAbility;
struct FGameplayTag;
class UGameplayEffect;

USTRUCT(BlueprintType)
struct FAbilityVolumeEnteredActorInfo
{
	GENERATED_BODY()

	TArray<FGameplayAbilitySpecHandle> AppliedAbilities;

	TArray<FActiveGameplayEffectHandle> AppliedEffects;
};

/**
 * 
 */
UCLASS()
class GASACTION_API AAbilitySystemVolume : public APhysicsVolume
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<UGameplayEffect>> OnGoingEffectsToApply;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<UGameplayEffect>> OnExitEffectsToApply;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bDrawDebug = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FGameplayTag> GameplayEventsToSendOnEnter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FGameplayTag> GameplayEventsToSendOnExit;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<UGameplayAbility>> OnGoingAbilitiesToGive;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<UGameplayAbility>> PermanentAbilitiesToGive;

	UPROPERTY()
	TMap<AActor*, FAbilityVolumeEnteredActorInfo> EnteredActorsInfoMap;

public:
	AAbilitySystemVolume();

	virtual void Tick(float DeltaSeconds) override;

	virtual void ActorEnteredVolume(AActor* Other) override;

	virtual void ActorLeavingVolume(AActor* Other) override;
};
