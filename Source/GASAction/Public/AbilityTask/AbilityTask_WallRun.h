// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_WallRun.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWallRunSideDeterminedDelegate, bool, bLeftSide);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWallRunFinishedDelegate);

class UCharacterMovementComponent;
/**
 * 
 */
UCLASS()
class GASACTION_API UAbilityTask_WallRun : public UAbilityTask
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnWallRunFinishedDelegate OnFinished;

	UPROPERTY(BlueprintAssignable)
	FOnWallRunSideDeterminedDelegate OnWallRunSideDetermined;

	UFUNCTION(BlueprintCallable, Category = "Ability|Task", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility"))
	static UAbilityTask_WallRun* CreateWallRunTask(UGameplayAbility* OwningAbility, ACharacter* InCharacter, UCharacterMovementComponent* InCharacterMovement, TArray<TEnumAsByte<EObjectTypeQuery>> InTraceObjectTypes);

	virtual void Activate() override;

	virtual void OnDestroy(bool bInOwnerFinished) override;

	virtual void TickTask(float DeltaTime) override;

protected:
	UPROPERTY()
	UCharacterMovementComponent* CharacterMovementComponent;

	UPROPERTY()
	ACharacter* Character;

	TArray<TEnumAsByte<EObjectTypeQuery>> TraceObjectTypes;

	bool FindRunnableWall(FHitResult& OnWallHit);

	bool IsWallOnTheLeft(const FHitResult& InHitResult) const;
};
