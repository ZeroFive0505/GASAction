// Fill out your copyright notice in the Description page of Project Settings.


#include "Volumes/AbilitySystemVolume.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

AAbilitySystemVolume::AAbilitySystemVolume()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AAbilitySystemVolume::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if(bDrawDebug)
	{
		DrawDebugBox(GetWorld(), GetActorLocation(), GetBounds().BoxExtent, FColor::Red, false, 0, 0, 5);
	}
}

void AAbilitySystemVolume::ActorEnteredVolume(AActor* Other)
{
	Super::ActorEnteredVolume(Other);

	if(!HasAuthority())
	{
		return;
	}

	if(UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Other))
	{
		for(auto& PermanentAbility : PermanentAbilitiesToGive)
		{
			AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(*PermanentAbility));
		}

		EnteredActorsInfoMap.Add(Other);

		for(auto& Ability : OnGoingAbilitiesToGive)
		{
			FGameplayAbilitySpecHandle AbilitySpecHandle = AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(Ability));

			EnteredActorsInfoMap[Other].AppliedAbilities.Add(AbilitySpecHandle);
		}

		for(auto& GameplayEffect : OnGoingEffectsToApply)
		{
			FGameplayEffectContextHandle EffectContextHandle = AbilitySystemComponent->MakeEffectContext();

			EffectContextHandle.AddInstigator(Other, Other);

			FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(GameplayEffect, 1, EffectContextHandle);

			if(SpecHandle.IsValid())
			{
				FActiveGameplayEffectHandle ActiveGameplayEffectHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

				if(ActiveGameplayEffectHandle.WasSuccessfullyApplied())
				{
					EnteredActorsInfoMap[Other].AppliedEffects.Add(ActiveGameplayEffectHandle);
				}
			}
		}

		for(auto& EventTag : GameplayEventsToSendOnEnter)
		{
			FGameplayEventData EventData;
			EventData.EventTag = EventTag;

			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Other, EventTag, EventData);
		}
	}
}

void AAbilitySystemVolume::ActorLeavingVolume(AActor* Other)
{
	Super::ActorLeavingVolume(Other);

	if(!HasAuthority())
	{
		return;
	}

	if(UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Other))
	{
		if(EnteredActorsInfoMap.Find(Other))
		{
			for(auto& GameplayEffectHandle : EnteredActorsInfoMap[Other].AppliedEffects)
			{
				AbilitySystemComponent->RemoveActiveGameplayEffect(GameplayEffectHandle);
			}

			for(auto& GameplayAbilityHandle : EnteredActorsInfoMap[Other].AppliedAbilities)
			{
				AbilitySystemComponent->ClearAbility(GameplayAbilityHandle);
			}

			EnteredActorsInfoMap.Remove(Other);
		}

		for(auto& GameplayEffect : OnExitEffectsToApply)
		{
			FGameplayEffectContextHandle EffectContextHandle = AbilitySystemComponent->MakeEffectContext();

			EffectContextHandle.AddInstigator(Other, Other);

			FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(GameplayEffect, 1, EffectContextHandle);

			if(SpecHandle.IsValid())
			{
				FActiveGameplayEffectHandle ActiveGameplayEffectHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
			}
		}
		
		for(auto& EventTag : GameplayEventsToSendOnExit)
		{
			FGameplayEventData EventData;
			EventData.EventTag = EventTag;

			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Other, EventTag, EventData);
		}
	}
}
