// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability/GA_InventoryCombatAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AGCommonTypes.h"
#include "ActorComponent/InventoryComponent.h"
#include "Actors/WeaponItemActor.h"
#include "Camera/CameraComponent.h"
#include "Character/AGActionCharacter.h"
#include "Kismet/KismetSystemLibrary.h"

bool UGA_InventoryCombatAbility::CommitAbility(const FGameplayAbilitySpecHandle Handle,
                                               const FGameplayAbilityActorInfo* ActorInfo,
                                               const FGameplayAbilityActivationInfo ActivationInfo,
                                               FGameplayTagContainer* OptionalRelevantTags)
{
	return Super::CommitAbility(Handle, ActorInfo, ActivationInfo, OptionalRelevantTags) && HasEnoughAmmo();
}

FGameplayEffectSpecHandle UGA_InventoryCombatAbility::GetWeaponEffectSpec(const FHitResult& InHitResult)
{
	if (UAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponentFromActorInfo())
	{
		if (const UWeaponStaticData* WeaponStaticData = GetEquippedItemWeaponStaticData())
		{
			FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();

			FGameplayEffectSpecHandle OutSpec = AbilitySystemComponent->MakeOutgoingSpec(
				WeaponStaticData->DamageEffect, 1, EffectContext);

			UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
				OutSpec, FGameplayTag::RequestGameplayTag(TEXT("Attribute.Health")), -WeaponStaticData->BaseDamage);

			return OutSpec;
		}
	}

	return FGameplayEffectSpecHandle();
}

bool UGA_InventoryCombatAbility::GetWeaponToFocusTraceResult(float TraceDistance, ETraceTypeQuery TraceTypeQuery,
                                                             FHitResult& OutHitResult)
{
	AWeaponItemActor* WeaponItemActor = GetEquippedWeaponItemActor();

	AAGActionCharacter* ActionCharacter = GetActionGameCharacterFromActorInfo();

	const FTransform& CameraTransform = ActionCharacter->GetFollowCamera()->GetComponentTransform();

	const FVector FocusTraceEnd = CameraTransform.GetLocation() + CameraTransform.GetRotation().Vector() *
		TraceDistance;

	TArray<AActor*> ActorsToIgnore = {GetAvatarActorFromActorInfo()};

	FHitResult FocusHit;

	UKismetSystemLibrary::LineTraceSingle(this, CameraTransform.GetLocation(), FocusTraceEnd, TraceTypeQuery, false,
	                                      ActorsToIgnore, EDrawDebugTrace::None, FocusHit, true);

	FVector MuzzleLocation = WeaponItemActor->GetMuzzlePosition();

	const FVector WeaponTraceEnd = MuzzleLocation + (FocusHit.Location - MuzzleLocation).GetSafeNormal() *
		TraceDistance;

	UKismetSystemLibrary::LineTraceSingle(this, MuzzleLocation, WeaponTraceEnd, TraceTypeQuery, false, ActorsToIgnore,
	                                      EDrawDebugTrace::None, OutHitResult, true);

	return OutHitResult.bBlockingHit;
}

bool UGA_InventoryCombatAbility::HasEnoughAmmo() const
{
	if (const UWeaponStaticData* WeaponStaticData = GetEquippedItemWeaponStaticData())
	{
		if (UInventoryComponent* Inventory = GetInventoryComponent())
		{
			return !WeaponStaticData->AmmoTag.IsValid() || Inventory->GetInventoryTagCount(
				WeaponStaticData->AmmoTag) > 0;
		}
	}
	
	return false;
}

void UGA_InventoryCombatAbility::DecreaseAmmo()
{
	if (const UWeaponStaticData* WeaponStaticData = GetEquippedItemWeaponStaticData())
	{
		if (!WeaponStaticData->AmmoTag.IsValid())
		{
			return;
		}

		if (UInventoryComponent* Inventory = GetInventoryComponent())
		{
			InventoryComponent->RemoveItemWithInventoryTag(WeaponStaticData->AmmoTag, 1);
		}
	}
}
