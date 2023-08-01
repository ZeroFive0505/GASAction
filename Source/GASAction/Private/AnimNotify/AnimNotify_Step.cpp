// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify/AnimNotify_Step.h"

#include "ActorComponent/FootStepComponent.h"
#include "Character/AGActionCharacter.h"

void UAnimNotify_Step::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	check(MeshComp);

	AAGActionCharacter* Character = MeshComp ? Cast<AAGActionCharacter>(MeshComp->GetOwner()) : nullptr;

	if(Character)
	{
		if(UFootStepComponent* FootStepComponent = Character->GetFootStepComponent())
		{
			FootStepComponent->HandleFootStep(Foot);
		}
	}
}
