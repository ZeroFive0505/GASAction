// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Abilities/GameplayAbility.h"
#include "InventoryItemInstance.generated.h"

struct FGameplayAbilitySpecHandle;
class AItemActor;
class UItemStaticData;
class UInventoryComponent;
/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class GASACTION_API UInventoryItemInstance : public UObject
{
	GENERATED_BODY()
public:
	virtual void Init(TSubclassOf<UItemStaticData> InItemStaticDataClass, int32 InQuantity = 1);

	virtual bool IsSupportedForNetworking() const override { return true; }

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	const UItemStaticData* GetItemStaticData() const;

	UPROPERTY(Replicated)
	TSubclassOf<UItemStaticData> ItemStaticDataClass;

	UPROPERTY(ReplicatedUsing = OnRep_Equipped)
	bool bEquipped = false;

	UFUNCTION()
	void OnRep_Equipped();

	virtual void OnEquipped(AActor* InOwner = nullptr);
	virtual void OnUnequipped(AActor* InOwner = nullptr);
	virtual void OnDropped(AActor* InOwner = nullptr);

	UFUNCTION(BlueprintPure)
	FORCEINLINE AItemActor* GetItemActor() const { return ItemActor; }

	FORCEINLINE int32 GetQuantity() const { return Quantity; }

	void AddItem(int32 Count);

protected:
	UPROPERTY(Replicated)
	AItemActor* ItemActor = nullptr;

	UPROPERTY(Replicated)
	int32 Quantity = 1;

	void TryGrantAbilities(AActor* InOwner);

	void TryRemoveAbilities(AActor* InOwner);

	void TryApplyEffects(AActor* InOwner);

	void TryRemoveEffects(AActor* InOwner);

	UPROPERTY()
	TArray<FGameplayAbilitySpecHandle> GrantedAbilityHandles;

	UPROPERTY()
	TArray<FActiveGameplayEffectHandle> OnGoingEffectsHandles;
};
