// Fill out your copyright notice in the Description page of Project Settings.

#include "AnimInstance/AGAnimInstance.h"

#include "ActorComponent/InventoryComponent.h"
#include "Character/AGActionCharacter.h"
#include "DataAsset/CharacterAnimDataAsset.h"
#include "Inventory/InventoryItemInstance.h"


const UItemStaticData* UAGAnimInstance::GetEquippedItemData() const
{
	const AAGActionCharacter* ActionCharacter = Cast<AAGActionCharacter>(GetOwningActor());
	const UInventoryComponent* InventoryComponent = ActionCharacter ? ActionCharacter->GetInventoryComponent() : nullptr;
	const UInventoryItemInstance* ItemInstance = InventoryComponent ? InventoryComponent->GetCurrentEquippedItem() : nullptr;

	return ItemInstance ? ItemInstance->GetItemStaticData() : nullptr;
}

UBlendSpace* UAGAnimInstance::GetLocomotionBlendSpace() const
{
	if(AAGActionCharacter* ActionCharacter = Cast<AAGActionCharacter>(GetOwningActor()))
	{
		if(const UItemStaticData* ItemData = GetEquippedItemData())
		{
			if(ItemData->CharacterAnimationData.MovementBlendSpace)
			{
				return ItemData->CharacterAnimationData.MovementBlendSpace;
			}
		}
		
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
		if(const UItemStaticData* ItemData = GetEquippedItemData())
		{
			if(ItemData->CharacterAnimationData.IdleAnimationAsset)
			{
				return ItemData->CharacterAnimationData.IdleAnimationAsset;
			}
		}
		
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
		if(const UItemStaticData* ItemData = GetEquippedItemData())
		{
			if(ItemData->CharacterAnimationData.CrouchBlendSpace)
			{
				return ItemData->CharacterAnimationData.CrouchBlendSpace;
			}
		}
		
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
		if(const UItemStaticData* ItemData = GetEquippedItemData())
		{
			if(ItemData->CharacterAnimationData.CrouchIdleAnimationAsset)
			{
				return ItemData->CharacterAnimationData.CrouchIdleAnimationAsset;
			}
		}
		
		FCharacterData Data = ActionCharacter->GetCharacterData();

		if(Data.CharacterAnimDataAsset)
		{
			return Data.CharacterAnimDataAsset->CharacterAnimationData.CrouchIdleAnimationAsset;
		}
	}

	return DefaultCharacterAnimDataAsset ? DefaultCharacterAnimDataAsset->CharacterAnimationData.CrouchIdleAnimationAsset : nullptr;
}
