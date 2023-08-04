// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorComponent/AGCharacterMovementComponent.h"

#include "AbilitySystemComponent.h"

static TAutoConsoleVariable<int32> CVarTraversal(
TEXT("ShowDebugTraversal"),
	0,
	TEXT("Draws debug info about traversal")
	TEXT("  0: off/n")
	TEXT("  1: on/n"),
	ECVF_Cheat);

bool UAGCharacterMovementComponent::TryTraversal(UAbilitySystemComponent* AbilitySystemComponent)
{
	for(TSubclassOf<UGameplayAbility> AbilityClass : TraversalAbilitiesOrdered)
	{
		if(AbilitySystemComponent->TryActivateAbilityByClass(AbilityClass))
		{
			FGameplayAbilitySpec* Spec;

			Spec = AbilitySystemComponent->FindAbilitySpecFromClass(AbilityClass);

			if(Spec && Spec->IsActive())
			{
				return true;
			}
		}
	}

	return false;
}
