// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilityTask/AbilityTask_WallRun.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"

UAbilityTask_WallRun* UAbilityTask_WallRun::CreateWallRunTask(UGameplayAbility* OwningAbility, ACharacter* InCharacter,
                                                              UCharacterMovementComponent* InCharacterMovement, TArray<TEnumAsByte<EObjectTypeQuery>> InTraceObjectTypes)
{
	UAbilityTask_WallRun* WallRunTask = NewAbilityTask<UAbilityTask_WallRun>(OwningAbility);

	WallRunTask->CharacterMovementComponent = InCharacterMovement;
	WallRunTask->Character = InCharacter;
	WallRunTask->bTickingTask = true;
	WallRunTask->TraceObjectTypes = InTraceObjectTypes;

	return WallRunTask;
}

void UAbilityTask_WallRun::Activate()
{
	Super::Activate();

	FHitResult OnWallHit;

	const FVector CurrentAcceleration = CharacterMovementComponent->GetCurrentAcceleration();

	if(!FindRunnableWall(OnWallHit))
	{
		if(ShouldBroadcastAbilityTaskDelegates())
		{
			OnFinished.Broadcast();
		}

		EndTask();

		return;
	}

	OnWallRunSideDetermined.Broadcast(IsWallOnTheLeft(OnWallHit));

	Character->Landed(OnWallHit);

	Character->SetActorLocation(OnWallHit.ImpactPoint + OnWallHit.ImpactNormal * 60.0f);

	CharacterMovementComponent->SetMovementMode(MOVE_Flying);
}

void UAbilityTask_WallRun::OnDestroy(bool bInOwnerFinished)
{
	CharacterMovementComponent->SetPlaneConstraintEnabled(false);

	CharacterMovementComponent->SetMovementMode(MOVE_Falling);
	
	Super::OnDestroy(bInOwnerFinished);
}

void UAbilityTask_WallRun::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);

	FHitResult OnWallHit;

	const FVector CurrentAcceleration = CharacterMovementComponent->GetCurrentAcceleration();
	
	if(!FindRunnableWall(OnWallHit))
	{
		if(ShouldBroadcastAbilityTaskDelegates())
		{
			OnFinished.Broadcast();
		}

		EndTask();

		return;
	}

	FRotator DirectionRotator = IsWallOnTheLeft(OnWallHit) ? FRotator(0, -90, 0) : FRotator(0, 90, 0);

	const FVector WallRunDirection = DirectionRotator.RotateVector(OnWallHit.ImpactNormal);

	CharacterMovementComponent->Velocity = WallRunDirection * 700.0f;

	CharacterMovementComponent->Velocity.Z = CharacterMovementComponent->GetGravityZ() * DeltaTime;

	Character->SetActorRotation(WallRunDirection.Rotation());

	CharacterMovementComponent->SetPlaneConstraintEnabled(true);
	CharacterMovementComponent->SetPlaneConstraintOrigin(OnWallHit.ImpactPoint);
	CharacterMovementComponent->SetPlaneConstraintNormal(OnWallHit.ImpactNormal);
}

bool UAbilityTask_WallRun::FindRunnableWall(FHitResult& OnWallHit)
{
	const FVector CharacterLocation = Character->GetActorLocation();
	const FVector RightVector = Character->GetActorRightVector();
	const FVector ForwardVector = Character->GetActorForwardVector();

	const float TraceLength = Character->GetCapsuleComponent()->GetScaledCapsuleRadius() + 30.0f;

	TArray<AActor*> ActorsToIgnore = {Character};

	FHitResult TraceHit;


	static const auto CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("ShowDebugTraversal"));
	const bool bShowTraversal = CVar->GetInt() > 0;

	EDrawDebugTrace::Type DebugDrawType = bShowTraversal ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;

	if(UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(), CharacterLocation, CharacterLocation + ForwardVector * TraceLength, TraceObjectTypes, true, ActorsToIgnore, DebugDrawType, OnWallHit, true))
	{
		return false;
	}

	if(UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(), CharacterLocation, CharacterLocation + -RightVector * TraceLength, TraceObjectTypes, true, ActorsToIgnore, DebugDrawType, OnWallHit, true))
	{
		if(FVector::DotProduct(OnWallHit.ImpactNormal, RightVector) > 0.3f)
		{
			return true;
		}
	}

	if(UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(), CharacterLocation, CharacterLocation + RightVector * TraceLength, TraceObjectTypes, true, ActorsToIgnore, DebugDrawType, OnWallHit, true))
	{
		if(FVector::DotProduct(OnWallHit.ImpactNormal, -RightVector) > 0.3f)
		{
			return true;
		}
	}

	return false;
}

bool UAbilityTask_WallRun::IsWallOnTheLeft(const FHitResult& InHitResult) const
{
	return FVector::DotProduct(Character->GetActorRightVector(), InHitResult.ImpactNormal) > 0.0f;
}
