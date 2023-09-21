// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "AGCommonTypes.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/Character.h"
#include "Abilities/GameplayAbility.h"
#include "InputActionValue.h"
#include "AGActionCharacter.generated.h"

class UInventoryComponent;
class UAGAbilitySystemComponentBase;
class UAGAttributeSetBase;

class UGameplayEffect;
class UGameplayAbility;
class UAGMotionWarpingComponent;
class UAGCharacterMovementComponent;

UCLASS(config=Game)
class AAGActionCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	class UInputAction* CrouchAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	class UInputAction* SprintAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* DropItemAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	class UInputAction* EquipNextItemAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	class UInputAction* UnEquipItemAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	class UInputAction* AttackInputAction;


public:
	AAGActionCharacter();
	

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);
			

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();

	virtual void PostInitializeComponents() override;

	virtual void PawnClientRestart() override;

	virtual void ActivateJumpAbility();

	virtual void OnActivateCrouchAbility();

	virtual void OnDeactivateCrouchAbility();

	virtual void OnActivateSprintAbility();

	virtual void OnDeactivateSprintAbility();
	
	virtual void StopJumping() override;

	virtual void Landed(const FHitResult& Hit) override;

	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

	virtual void OnDropItem(const FInputActionValue& Value);

	virtual void OnEquipNextItem(const FInputActionValue& Value);

	virtual void OnUnEquipItem(const FInputActionValue& Value);
	
	virtual void OnAttackStarted(const FInputActionValue& Value);

	virtual void OnAttackEnded(const FInputActionValue& Value);

	AAGActionCharacter(const FObjectInitializer& ObjectInitializer);


public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	////////////////////////////////////////////////////////////
	// Gameplay ability
protected:
	void GiveAbilities();
	void ApplyStartUpEffects();

	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

	UPROPERTY(EditDefaultsOnly)
	UAGAbilitySystemComponentBase* AbilitySystemComponent;

	UPROPERTY(Transient)
	UAGAttributeSetBase* AttributeSet;

	UPROPERTY(ReplicatedUsing = OnRep_CharacterData)
	FCharacterData CharacterData;

	UFUNCTION()
	void OnRep_CharacterData();

	UFUNCTION()
	virtual void InitFromCharacterData(const FCharacterData& InCharacterData, bool bFromReplication = false);

	UPROPERTY(EditDefaultsOnly)
	class UCharacterDataAsset* CharacterDataAsset;

	UPROPERTY(BlueprintReadOnly)
	class UFootStepComponent* FootStepComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MotionWarp")
	UAGMotionWarpingComponent* MotionWarpingComponent;

	UPROPERTY()
	UAGCharacterMovementComponent* CharacterMovementComponent;

	// Gameplay Events
protected:
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag JumpEventTag;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag AttackStartTag;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag AttackEndTag;

	// Gameplay Tags
protected:
	UPROPERTY(EditDefaultsOnly)
	FGameplayTagContainer InAirTags;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTagContainer CrouchTags;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTagContainer SprintTags;

	// Gameplay effects
protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> CrouchStateEffect;

	// Delegates
protected:
	FDelegateHandle MaxMovementSpeedChangeDelegateHandle;
	
public:
	bool ApplyGameplayEffectToSelf(TSubclassOf<UGameplayEffect> Effect, FGameplayEffectContextHandle InEffectContext);

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps ) const override;

	UFUNCTION(BlueprintCallable)
	FCharacterData GetCharacterData() const;

	UFUNCTION(BlueprintCallable)
	void SetCharacterData(const FCharacterData& InCharacterData);

	UFUNCTION(BlueprintCallable)
	FORCEINLINE class UFootStepComponent* GetFootStepComponent() const { return FootStepComponent; }
	
	FORCEINLINE UAGMotionWarpingComponent* GetMotionWarpingComponent() const { return MotionWarpingComponent; }
	
	void OnMovementSpeedChanged(const FOnAttributeChangeData& Data);

	FORCEINLINE UInventoryComponent* GetInventoryComponent() const { return InventoryComponent; }

	// Inventory
protected:
	UPROPERTY(EditAnywhere, Replicated)
	UInventoryComponent* InventoryComponent;
};

