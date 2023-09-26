// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability/GA_InventoryCombatAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AGCommonTypes.h"

FGameplayEffectSpecHandle UGA_InventoryCombatAbility::GetWeaponEffectSpec(const FHitResult& InHitResult)
{
	if(UAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponentFromActorInfo())
	{
		if(const UWeaponStaticData* WeaponStaticData = GetEquippedItemWeaponStaticData())
		{
			FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();

			FGameplayEffectSpecHandle OutSpec = AbilitySystemComponent->MakeOutgoingSpec(WeaponStaticData->DamageEffect, 1, EffectContext);

			UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(OutSpec, FGameplayTag::RequestGameplayTag(TEXT("Attribute.Health")), -WeaponStaticData->BaseDamage);

			return OutSpec;
		}
	}

	return FGameplayEffectSpecHandle();
}
