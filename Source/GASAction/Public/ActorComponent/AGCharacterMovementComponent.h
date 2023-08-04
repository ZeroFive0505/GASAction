// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AGCharacterMovementComponent.generated.h"

class UAbilitySystemComponent;
class UGameplayAbility;

UCLASS()
class GASACTION_API UAGCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	bool TryTraversal(UAbilitySystemComponent* AbilitySystemComponent);

protected:
	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<UGameplayAbility>> TraversalAbilitiesOrdered;
};
