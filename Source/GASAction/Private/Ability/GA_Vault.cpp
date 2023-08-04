// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability/GA_Vault.h"
#include "Kismet/KismetSystemLibrary.h"
#include "DrawDebugHelpers.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "ActorComponent/AGMotionWarpingComponent.h"
#include "Character/AGActionCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

UGA_Vault::UGA_Vault()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

bool UGA_Vault::CommitCheck(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                            const FGameplayAbilityActivationInfo ActivationInfo, FGameplayTagContainer* OptionalRelevantTags)
{
	if(!Super::CommitCheck(Handle, ActorInfo, ActivationInfo, OptionalRelevantTags))
	{
		return false;
	}

	AAGActionCharacter* Character = GetActionGameCharacterFromActorInfo();

	if(!IsValid(Character))
	{
		return false;
	}

	const FVector StartLocation = Character->GetActorLocation();
	const FVector ForwardVector = Character->GetActorForwardVector();
	const FVector UpVector = Character->GetActorUpVector();

	TArray<AActor*> ActorsToIgnore = {Character};

	static const auto CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("ShowDebugTraversal"));
	const bool bShowTraversal = CVar->GetInt() > 0;

	EDrawDebugTrace::Type DebugDrawType = bShowTraversal ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;

	bool bJumpToLocationSet = false;

	int32 JumpToLocationIndex = INDEX_NONE;

	int32 Index = 0;

	FHitResult TraceHit;

	float MaxJumpDistance = HorizontalTraceLength;

	for(; Index < HorizontalTraceCount; Index++)
	{
		const FVector TraceStart = StartLocation + Index * UpVector * HorizontalTraceStep;
		const FVector TraceEnd = TraceStart + ForwardVector * HorizontalTraceLength;

		if(UKismetSystemLibrary::SphereTraceSingleForObjects(this, TraceStart, TraceEnd, HorizontalTraceRadius, TraceObjectTypes, true, ActorsToIgnore, DebugDrawType, TraceHit, true))
		{
			if(JumpToLocationIndex == INDEX_NONE && (Index < HorizontalTraceCount - Index))
			{
				JumpToLocationIndex = Index;
				JumpToLocation = TraceHit.Location;
			}
			else if(JumpToLocationIndex == (Index - 1))
			{
				MaxJumpDistance = FVector::Dist2D(TraceHit.Location, TraceStart);
				break;
			}
		}
		else
		{
			if(JumpToLocationIndex != INDEX_NONE)
			{
				break;
			}
		}
	}

	if(JumpToLocationIndex == INDEX_NONE)
	{
		return false;
	}

	const float DistanceToJumpTo = FVector::Dist2D(StartLocation, JumpToLocation);

	const float MaxVerticalTraceDistance = MaxJumpDistance - DistanceToJumpTo;

	if(MaxVerticalTraceDistance < 0.0f)
	{
		return false;
	}

	if(Index == HorizontalTraceCount)
	{
		Index = HorizontalTraceCount - 1;
	}

	const float VerticalTraceLength = FMath::Abs(JumpToLocation.Z - (StartLocation + Index * UpVector * HorizontalTraceStep).Z);

	FVector VerticalStartLocation = JumpToLocation + UpVector * VerticalTraceLength;

	Index = 0;

	const float VerticalTraceCount = MaxVerticalTraceDistance / VerticalTraceStep;

	bool bJumpOverLocationSet = false;

	for(; Index <= VerticalTraceCount; Index++)
	{
		const FVector TraceStart = VerticalStartLocation + Index * ForwardVector * VerticalTraceStep;
		const FVector TraceEnd = TraceStart + UpVector * VerticalTraceLength * -1.0f;

		if(UKismetSystemLibrary::SphereTraceSingleForObjects(this, TraceStart, TraceEnd, VerticalTraceRadius, TraceObjectTypes, true, ActorsToIgnore, DebugDrawType, TraceHit, true))
		{
			JumpOverLocation = TraceHit.ImpactPoint;

			if(Index == 0)
			{
				JumpToLocation = JumpOverLocation;
			}
		}
		else if(Index != 0)
		{
			bJumpOverLocationSet = true;
			break;
		}
	}

	if(!bJumpOverLocationSet)
	{
		return false;
	}

	const FVector TraceStart = JumpOverLocation + ForwardVector * VerticalTraceStep;

	if(UKismetSystemLibrary::SphereTraceSingleForObjects(this, TraceStart, JumpOverLocation, HorizontalTraceRadius, TraceObjectTypes, true, ActorsToIgnore, DebugDrawType, TraceHit, true))
	{
		JumpOverLocation = TraceHit.ImpactPoint;
	}

	if(bShowTraversal)
	{
		DrawDebugSphere(GetWorld(), JumpToLocation, 15, 16, FColor::White, false, 7);
		DrawDebugSphere(GetWorld(), JumpOverLocation, 15, 16, FColor::White, false, 7);
	}

	return true;
}

void UGA_Vault::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if(!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		K2_EndAbility();
		return;
	}

	AAGActionCharacter* Character = GetActionGameCharacterFromActorInfo();

	UCharacterMovementComponent* CharacterMovementComponent = Character ? Character->GetCharacterMovement() : nullptr;

	if(CharacterMovementComponent)
	{
		CharacterMovementComponent->SetMovementMode(MOVE_Flying);
	}

	UCapsuleComponent* CapsuleComponent = Character ? Character->GetCapsuleComponent() : nullptr;

	if(CapsuleComponent)
	{
		for(ECollisionChannel Channel : CollisionChannelsToIgnore)
		{
			CapsuleComponent->SetCollisionResponseToChannel(Channel, ECR_Ignore);
		}
	}

	UAGMotionWarpingComponent* MotionWarpingComponent = Character ? Character->GetMotionWarpingComponent() : nullptr;

	if(MotionWarpingComponent)
	{
		MotionWarpingComponent->AddOrUpdateWarpTargetFromLocationAndRotation(TEXT("JumpToLocation"), JumpToLocation, Character->GetActorRotation());
		MotionWarpingComponent->AddOrUpdateWarpTargetFromLocationAndRotation(TEXT("JumpOverLocation"), JumpOverLocation, Character->GetActorRotation());
	}

	MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, VaultMontage);

	MontageTask->OnBlendOut.AddDynamic(this, &UGA_Vault::K2_EndAbility);
	MontageTask->OnCompleted.AddDynamic(this, &UGA_Vault::K2_EndAbility);
	MontageTask->OnInterrupted.AddDynamic(this, &UGA_Vault::K2_EndAbility);
	MontageTask->OnCancelled.AddDynamic(this, &UGA_Vault::K2_EndAbility);
	MontageTask->ReadyForActivation();
}

void UGA_Vault::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if(IsValid(MontageTask))
	{
		MontageTask->EndTask();
	}

	AAGActionCharacter* Character = GetActionGameCharacterFromActorInfo();

	UCapsuleComponent* CapsuleComponent = Character ? Character->GetCapsuleComponent() : nullptr;

	if(CapsuleComponent)
	{
		for(ECollisionChannel Channel : CollisionChannelsToIgnore)
		{
			CapsuleComponent->SetCollisionResponseToChannel(Channel, ECR_Block);
		}
	}

	UCharacterMovementComponent* CharacterMovementComponent = Character ? Character->GetCharacterMovement() : nullptr;

	if(CharacterMovementComponent && CharacterMovementComponent->IsFlying())
	{
		CharacterMovementComponent->SetMovementMode(MOVE_Falling);
	}

	UAGMotionWarpingComponent* MotionWarpingComponent = Character ? Character->GetMotionWarpingComponent() : nullptr;

	if(MotionWarpingComponent)
	{
		MotionWarpingComponent->RemoveWarpTarget(TEXT("JumpToLocation"));
		MotionWarpingComponent->RemoveWarpTarget(TEXT("JumpOverLocation"));
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
