// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NiagaraSystem.h"
#include "AGCommonTypes.generated.h"

USTRUCT(BlueprintType)
struct FCharacterData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GAS")
	TArray<TSubclassOf<class UGameplayEffect>> Effects;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GAS")
	TArray<TSubclassOf<class UGameplayAbility>> Abilities;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Animation")
	class UCharacterAnimDataAsset* CharacterAnimDataAsset;
};

USTRUCT(BlueprintType)
struct FCharacterAnimationData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	class UBlendSpace* MovementBlendSpace = nullptr;

	UPROPERTY(EditDefaultsOnly)
	class UAnimSequenceBase* IdleAnimationAsset = nullptr;

	UPROPERTY(EditDefaultsOnly)
	class UBlendSpace* CrouchBlendSpace = nullptr;

	UPROPERTY(EditDefaultsOnly)
	class UAnimSequenceBase* CrouchIdleAnimationAsset = nullptr;
};

UENUM(BlueprintType)
enum class EFoot : uint8
{
	Left UMETA(DisplayName = "Left"),
	Right UMETA(DisplayName = "Right")
};

UENUM(BlueprintType)
enum class EItemState : uint8
{
	None UMETA(DisplayName = "None"),
	Equipped UMETA(DisplayName = "Equipped"),
	Dropped UMETA(DisplayName = "Dropped")
};


UENUM()
enum class EMovementDirectionType : uint8
{
	None UMETA(DisplayName = "None"),
	OrientToMovement UMETA(DisplayName = "OrientToMovment"),
	Strafe UMETA(DisplayName = "Strafe")
};


UCLASS(BlueprintType, Blueprintable)
class UItemStaticData : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText Name;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<class AItemActor> ItemActorClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName AttachmentSocket = NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bCanBeEquipped = false;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FCharacterAnimationData CharacterAnimationData;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<TSubclassOf<UGameplayAbility>> GrantedAbilities;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<TSubclassOf<UGameplayEffect>> OnGoingEffects;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FGameplayTag> InventoryTags;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 MaxStackCount = 1;
};

UCLASS(BlueprintType, Blueprintable)
class UWeaponStaticData : public UItemStaticData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> DamageEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	USkeletalMesh* SkeletalMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UStaticMesh* StaticMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UAnimMontage* AnimMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float FireRate;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float BaseDamage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	USoundBase* AttackSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag AmmoTag;
};

UCLASS(BlueprintType, Blueprintable)
class UAmmoItemStaticData : public UItemStaticData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UStaticMesh* StaticMesh = nullptr;
};

UCLASS(BlueprintType, Blueprintable)
class UProjectileStaticData : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float BaseDamage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float DamageRadius;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float GravityMultiplier = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float InitialSpeed = 3000.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MaxSpeed = 3000.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UStaticMesh* StaticMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<TSubclassOf<UGameplayEffect>> Effects;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<TEnumAsByte<EObjectTypeQuery>> RadialDamageQueryType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TEnumAsByte<ETraceTypeQuery> RadialDamageTraceType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UNiagaraSystem* OnStopVFX = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	USoundBase* OnStopSFX = nullptr;
};
