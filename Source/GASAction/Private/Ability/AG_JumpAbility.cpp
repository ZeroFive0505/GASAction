// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability/AG_JumpAbility.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemLog.h"
#include "GameFramework/Character.h"

UAG_JumpAbility::UAG_JumpAbility()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::NonInstanced;
}

bool UAG_JumpAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if(!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	const ACharacter* Character = CastChecked<ACharacter>(ActorInfo->AvatarActor.Get(), ECastCheckedType::NullAllowed);
	return Character->CanJump();
}

void UAG_JumpAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	if(HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		if(!CommitAbility(Handle, ActorInfo, ActivationInfo))
		{
			return;
		}

		Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
		
		ACharacter* Character = CastChecked<ACharacter>(ActorInfo->AvatarActor.Get(), ECastCheckedType::NullAllowed);
		Character->Jump();
	}
}
