// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorComponent/FootStepComponent.h"

#include "Character/AGActionCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicalMaterial/AGPhysicalMaterial.h"

static TAutoConsoleVariable<int32> CVarShowFootSteps(
	TEXT("ShowDebugFootSteps"),
	0,
	TEXT("Draws debug info about footsteps")
	TEXT("  0: off/n")
	TEXT("  1: on/n"),
	ECVF_Cheat);

// Sets default values for this component's properties
UFootStepComponent::UFootStepComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UFootStepComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UFootStepComponent::HandleFootStep(EFoot Foot)
{
	if(AAGActionCharacter* Character = Cast<AAGActionCharacter>(GetOwner()))
	{

		const int32 DebugShowFootSteps = CVarShowFootSteps.GetValueOnAnyThread();
		
		if(USkeletalMeshComponent* MeshComponent = Character->GetMesh())
		{
			FHitResult HitResult;

			const FVector SocketLocation = MeshComponent->GetSocketLocation(Foot == EFoot::Left ? LeftFootSocketName : RightFootSocketName);
			const FVector Location = SocketLocation + FVector::UpVector * 20.0f;

			FCollisionQueryParams Params;
			Params.bReturnPhysicalMaterial = true;
			Params.AddIgnoredActor(Character);

			if(GetWorld()->LineTraceSingleByChannel(HitResult, Location, Location + FVector::UpVector * -50.0f, ECC_WorldStatic, Params))
			{
				if(HitResult.bBlockingHit)
				{
					if(HitResult.PhysMaterial.Get())
					{
						UAGPhysicalMaterial* PhysicalMaterial = Cast<UAGPhysicalMaterial>(HitResult.PhysMaterial.Get());

						if(PhysicalMaterial)
						{
							UGameplayStatics::PlaySoundAtLocation(this, PhysicalMaterial->FootStepSoundBase, Location, 1.0f);
						}

						if(DebugShowFootSteps > 0)
						{
							DrawDebugString(GetWorld(), Location, GetNameSafe(PhysicalMaterial), nullptr, FColor::White, 4.0f);
						}
					}

					if(DebugShowFootSteps > 0)
					{
						DrawDebugSphere(GetWorld(), Location, 16.0f, 16.0f, FColor::Red, false, 4.0f);
					}
				}
				else
				{
					if(DebugShowFootSteps > 0)
					{
						DrawDebugLine(GetWorld(), Location, Location + FVector::UpVector * -50.0f, FColor::Red, false, 4.0f, 0, 1.0f);
					}
				}
			}
			else
			{
				if(DebugShowFootSteps > 0)
				{
					DrawDebugLine(GetWorld(), Location, Location + FVector::UpVector * -50.0f, FColor::Red, false, 4.0f, 0, 1.0f);
					DrawDebugSphere(GetWorld(), Location, 16.0f, 16.0f, FColor::Red, false, 4.0f);
				}
			}
		}
	}
}
