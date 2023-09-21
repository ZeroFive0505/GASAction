// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorComponent/AGCharacterMovementComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/Character.h"

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

void UAGCharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	HandleMovementDirection();

	if(UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner()))
	{
		AbilitySystemComponent->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(TEXT("Movement.Enforced.Strafe"), EGameplayTagEventType::NewOrRemoved)).AddUObject(this, &UAGCharacterMovementComponent::OnEnforcedStrafeTargetChanged);
	}
}

void UAGCharacterMovementComponent::OnEnforcedStrafeTargetChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	if(NewCount)
	{
		SetMovementDirectionType(EMovementDirectionType::Strafe);
	}
	else
	{
		SetMovementDirectionType(EMovementDirectionType::OrientToMovement);
	}
}

void UAGCharacterMovementComponent::HandleMovementDirection()
{
	switch (MovementDirectionType)
	{
	case EMovementDirectionType::Strafe:
		bUseControllerDesiredRotation = true;
		bOrientRotationToMovement = false;
		CharacterOwner->bUseControllerRotationYaw = true;
		break;
	default:
		bUseControllerDesiredRotation = false;
		bOrientRotationToMovement = true;
		CharacterOwner->bUseControllerRotationYaw = false;
		break;
	}
}
