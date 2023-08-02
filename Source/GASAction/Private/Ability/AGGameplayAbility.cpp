// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability/AGGameplayAbility.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemLog.h"
#include "Character/AGActionCharacter.h"

void UAGGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                         const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                         const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	FGameplayEffectContextHandle EffectContextHandle = ActorInfo->AbilitySystemComponent->MakeEffectContext();

	for(auto GameplayEffect : OnGoingEffectsToJustApplyOnStart)
	{
		if(!GameplayEffect.Get())
		{
			continue;
		}

		if(UAbilitySystemComponent* AbilitySystemComponent = ActorInfo->AbilitySystemComponent.Get())
		{
			FGameplayEffectSpecHandle SpecHandle = ActorInfo->AbilitySystemComponent->MakeOutgoingSpec(GameplayEffect, 1.0f, EffectContextHandle);

			if(SpecHandle.IsValid())
			{
				FActiveGameplayEffectHandle ActiveGameplayEffectHandle = ActorInfo->AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
				
				if(!ActiveGameplayEffectHandle.WasSuccessfullyApplied())
				{
					ABILITY_LOG(Log, TEXT("Ability %s failed to apply startup effect %s"), *GetName(), *GetNameSafe(GameplayEffect));
				}
			}
		}
	}

	if(IsInstantiated())
	{
		for(auto GameplayEffect : OnGoingEffectsToRemoveOnEnd)
		{
			if(!GameplayEffect.Get())
			{
				continue;
			}

			if(UAbilitySystemComponent* AbilitySystemComponent = ActorInfo->AbilitySystemComponent.Get())
			{
				FGameplayEffectSpecHandle SpecHandle = ActorInfo->AbilitySystemComponent->MakeOutgoingSpec(GameplayEffect, 1.0f, EffectContextHandle);

				if(SpecHandle.IsValid())
				{
					FActiveGameplayEffectHandle ActiveGameplayEffectHandle = ActorInfo->AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
				
					if(!ActiveGameplayEffectHandle.WasSuccessfullyApplied())
					{
						ABILITY_LOG(Log, TEXT("Ability %s failed to apply runtime effect %s"), *GetName(), *GetNameSafe(GameplayEffect));
					}
					else
					{
						RemoveOnEndEffectHandles.Add(ActiveGameplayEffectHandle);
					}
				}
			}
			
		}
	}
}

void UAGGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if(IsInstantiated())
	{
		for(FActiveGameplayEffectHandle ActiveGameplayEffectHandle : RemoveOnEndEffectHandles)
		{
			if(ActiveGameplayEffectHandle.IsValid())
			{
				ActorInfo->AbilitySystemComponent->RemoveActiveGameplayEffect(ActiveGameplayEffectHandle);
			}
		}

		RemoveOnEndEffectHandles.Empty();
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

AAGActionCharacter* UAGGameplayAbility::GetActionGameCharacterFromActorInfo() const
{
	return Cast<AAGActionCharacter>(GetAvatarActorFromActorInfo());
}
