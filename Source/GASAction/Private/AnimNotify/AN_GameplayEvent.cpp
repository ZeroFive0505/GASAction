// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify/AN_GameplayEvent.h"

#include "AbilitySystemBlueprintLibrary.h"

void UAN_GameplayEvent::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(MeshComp->GetOwner(), Payload.EventTag, Payload);
}
