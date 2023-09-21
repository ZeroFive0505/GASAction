// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AGCommonTypes.h"
#include "GameplayTagContainer.h"
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

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintPure, BlueprintCallable)
	FORCEINLINE EMovementDirectionType GetMovementDirectionType() const { return MovementDirectionType; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetMovementDirectionType(EMovementDirectionType Direction) { MovementDirectionType = Direction; HandleMovementDirection(); }

	UFUNCTION()
	void OnEnforcedStrafeTargetChanged(const FGameplayTag CallbackTag, int32 NewCount);
	
protected:
	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<UGameplayAbility>> TraversalAbilitiesOrdered;

	UPROPERTY(EditAnywhere)
	EMovementDirectionType MovementDirectionType;

	void HandleMovementDirection();
};
