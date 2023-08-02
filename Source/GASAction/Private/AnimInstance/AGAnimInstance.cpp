// Fill out your copyright notice in the Description page of Project Settings.

#include "AnimInstance/AGAnimInstance.h"
#include "Character/AGActionCharacter.h"
#include "DataAsset/CharacterAnimDataAsset.h"

UBlendSpace* UAGAnimInstance::GetLocomotionBlendSpace() const
{
	if(AAGActionCharacter* ActionCharacter = Cast<AAGActionCharacter>(GetOwningActor()))
	{
		FCharacterData Data = ActionCharacter->GetCharacterData();

		if(Data.CharacterAnimDataAsset)
		{
			return Data.CharacterAnimDataAsset->CharacterAnimationData.MovementBlendSpace;
		}
	}

	return DefaultCharacterAnimDataAsset ? DefaultCharacterAnimDataAsset->CharacterAnimationData.MovementBlendSpace : nullptr;
}

UAnimSequenceBase* UAGAnimInstance::GetIdleAnimationAsset() const
{
	if(AAGActionCharacter* ActionCharacter = Cast<AAGActionCharacter>(GetOwningActor()))
	{
		FCharacterData Data = ActionCharacter->GetCharacterData();

		if(Data.CharacterAnimDataAsset)
		{
			return Data.CharacterAnimDataAsset->CharacterAnimationData.IdleAnimationAsset;
		}
	}

	return DefaultCharacterAnimDataAsset ? DefaultCharacterAnimDataAsset->CharacterAnimationData.IdleAnimationAsset : nullptr;
}

UBlendSpace* UAGAnimInstance::GetCrouchLocomotionBlendSpace() const
{
	if(AAGActionCharacter* ActionCharacter = Cast<AAGActionCharacter>(GetOwningActor()))
	{
		FCharacterData Data = ActionCharacter->GetCharacterData();

		if(Data.CharacterAnimDataAsset)
		{
			return Data.CharacterAnimDataAsset->CharacterAnimationData.CrouchBlendSpace;
		}
	}

	return DefaultCharacterAnimDataAsset ? DefaultCharacterAnimDataAsset->CharacterAnimationData.CrouchBlendSpace : nullptr;
}

UAnimSequenceBase* UAGAnimInstance::GetCrouchIdleAnimationAsset() const
{
	if(AAGActionCharacter* ActionCharacter = Cast<AAGActionCharacter>(GetOwningActor()))
	{
		FCharacterData Data = ActionCharacter->GetCharacterData();

		if(Data.CharacterAnimDataAsset)
		{
			return Data.CharacterAnimDataAsset->CharacterAnimationData.CrouchIdleAnimationAsset;
		}
	}

	return DefaultCharacterAnimDataAsset ? DefaultCharacterAnimDataAsset->CharacterAnimationData.CrouchIdleAnimationAsset : nullptr;
}
