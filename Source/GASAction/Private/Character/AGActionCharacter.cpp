// Copyright Epic Games, Inc. All Rights Reserved.

#include "Character/AGActionCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemLog.h"
#include "GameplayEffectExtension.h"
#include "Ability/AGAttributeSetBase.h"
#include "Ability/AGAbilitySystemComponentBase.h"
#include "ActorComponent/AGCharacterMovementComponent.h"
#include "ActorComponent/AGMotionWarpingComponent.h"
#include "ActorComponent/FootStepComponent.h"
#include "ActorComponent/InventoryComponent.h"
#include "DataAsset/CharacterDataAsset.h"
#include "Net/UnrealNetwork.h"


//////////////////////////////////////////////////////////////////////////
// AGASActionCharacter

AAGActionCharacter::AAGActionCharacter()
{
}

void AAGActionCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void AAGActionCharacter::PostLoad()
{
	Super::PostLoad();

	if (IsValid(CharacterDataAsset))
	{
		SetCharacterData(CharacterDataAsset->CharacterData);
	}
}

void AAGActionCharacter::PawnClientRestart()
{
	Super::PawnClientRestart();

	if(APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if(UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->ClearAllMappings();

			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void AAGActionCharacter::ActivateJumpAbility()
{
	// FGameplayEventData Payload;
	//
	// Payload.Instigator = this;
	// Payload.EventTag = JumpEventTag;
	//
	// UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, JumpEventTag, Payload);

	CharacterMovementComponent->TryTraversal(AbilitySystemComponent);
}

void AAGActionCharacter::OnActivateCrouchAbility()
{
	if(AbilitySystemComponent)
	{
		AbilitySystemComponent->TryActivateAbilitiesByTag(CrouchTags);
	}
}

void AAGActionCharacter::OnDeactivateCrouchAbility()
{
	if(AbilitySystemComponent)
	{
		AbilitySystemComponent->CancelAbilities(&CrouchTags);
	}
}

void AAGActionCharacter::OnActivateSprintAbility()
{
	if(AbilitySystemComponent)
	{
		AbilitySystemComponent->TryActivateAbilitiesByTag(SprintTags);
	}
}

void AAGActionCharacter::OnDeactivateSprintAbility()
{
	if(AbilitySystemComponent)
	{
		AbilitySystemComponent->CancelAbilities(&SprintTags);
	}
}

void AAGActionCharacter::StopJumping()
{
	// Super::StopJumping();
}

void AAGActionCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	if(AbilitySystemComponent)
	{
		AbilitySystemComponent->RemoveActiveEffectsWithTags(InAirTags);
	}
}

void AAGActionCharacter::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);

	if(!CrouchStateEffect.Get())
	{
		return;
	}

	if(AbilitySystemComponent)
	{
		FGameplayEffectContextHandle EffectContextHandle = AbilitySystemComponent->MakeEffectContext();
		
		FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(CrouchStateEffect, 1.0f, EffectContextHandle);

		if(SpecHandle.IsValid())
		{
			FActiveGameplayEffectHandle ActiveGameplayEffectHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

			if(!ActiveGameplayEffectHandle.WasSuccessfullyApplied())
			{
				ABILITY_LOG(Log, TEXT("Ability %s failed to apply crouch effect %s"), *GetName(), *GetNameSafe(CrouchStateEffect));
			}
		}
	}
}

void AAGActionCharacter::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);

	if(AbilitySystemComponent && CrouchStateEffect.Get())
	{
		AbilitySystemComponent->RemoveActiveGameplayEffectBySourceEffect(CrouchStateEffect, AbilitySystemComponent);
	}
}

void AAGActionCharacter::OnDropItem(const FInputActionValue& Value)
{
	FGameplayEventData EventData;
	EventData.EventTag = UInventoryComponent::DropItemTag;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, UInventoryComponent::DropItemTag, EventData);
}

void AAGActionCharacter::OnEquipNextItem(const FInputActionValue& Value)
{
	FGameplayEventData EventData;
	EventData.EventTag = UInventoryComponent::EquipNextTag;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, UInventoryComponent::EquipNextTag, EventData);
}

void AAGActionCharacter::OnUnEquipItem(const FInputActionValue& Value)
{
	FGameplayEventData EventData;
	EventData.EventTag = UInventoryComponent::UnEquipTag;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, UInventoryComponent::UnEquipTag, EventData);
}

void AAGActionCharacter::OnAttackStarted(const FInputActionValue& Value)
{
	FGameplayEventData EventData;
	EventData.EventTag = AttackStartTag;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, AttackStartTag, EventData);
}

void AAGActionCharacter::OnAttackEnded(const FInputActionValue& Value)
{
	FGameplayEventData EventData;
	EventData.EventTag = AttackEndTag;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, AttackEndTag, EventData);
}

void AAGActionCharacter::OnAimActionStarted(const FInputActionValue& Value)
{
	FGameplayEventData EventData;
	EventData.EventTag = AimStartedEventTag;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, AimStartedEventTag, EventData);
}

void AAGActionCharacter::OInAimActionEnded(const FInputActionValue& Value)
{
	FGameplayEventData EventData;
	EventData.EventTag = AimEndedEventTag;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, AimEndedEventTag, EventData);
}

AAGActionCharacter::AAGActionCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UAGCharacterMovementComponent>(
		ACharacter::CharacterMovementComponentName))
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	// Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	CharacterMovementComponent = Cast<UAGCharacterMovementComponent>(GetCharacterMovement());

	// Ability System
	AbilitySystemComponent = CreateDefaultSubobject<UAGAbilitySystemComponentBase>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSet = CreateDefaultSubobject<UAGAttributeSetBase>(TEXT("AttributSet"));

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetMovementSpeedAttribute()).AddUObject(this, &AAGActionCharacter::OnMovementSpeedChanged);
	
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetHealthAttribute()).AddUObject(this, &AAGActionCharacter::OnHealthAttributeChanged);

	AbilitySystemComponent->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(TEXT("State.RagDoll"), EGameplayTagEventType::NewOrRemoved)).AddUObject(this, &AAGActionCharacter::OnRagDollStateChanged);

	FootStepComponent = CreateDefaultSubobject<UFootStepComponent>(TEXT("FootStepComponent"));

	MotionWarpingComponent = CreateDefaultSubobject<UAGMotionWarpingComponent>(TEXT("MotionWarpingComponent"));

	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
	InventoryComponent->SetIsReplicated(true);
}

//////////////////////////////////////////////////////////////
// Gameplay ability

void AAGActionCharacter::GiveAbilities()
{
	if (HasAuthority() && AbilitySystemComponent)
	{
		for (auto DefaultAbility : CharacterData.Abilities)
		{
			AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(DefaultAbility));
		}
	}
}

void AAGActionCharacter::ApplyStartUpEffects()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		FGameplayEffectContextHandle EffectContextHandle = AbilitySystemComponent->MakeEffectContext();
		EffectContextHandle.AddSourceObject(this);

		for (auto CharacterEffect : CharacterData.Effects)
		{
			ApplyGameplayEffectToSelf(CharacterEffect, EffectContextHandle);
		}
	}
}

void AAGActionCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	AbilitySystemComponent->InitAbilityActorInfo(this, this);

	GiveAbilities();
	ApplyStartUpEffects();
}

void AAGActionCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	AbilitySystemComponent->InitAbilityActorInfo(this, this);
}

void AAGActionCharacter::OnRep_CharacterData()
{
	InitFromCharacterData(CharacterData, true);
}

void AAGActionCharacter::InitFromCharacterData(const FCharacterData& InCharacterData, bool bFromReplication)
{
}

void AAGActionCharacter::OnRagDollStateChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	if(NewCount > 0)
	{
		StartRagDoll();
	}
}

FCharacterData AAGActionCharacter::GetCharacterData() const
{
	return CharacterData;
}

void AAGActionCharacter::SetCharacterData(const FCharacterData& InCharacterData)
{
	CharacterData = InCharacterData;

	InitFromCharacterData(CharacterData);
}

void AAGActionCharacter::OnMovementSpeedChanged(const FOnAttributeChangeData& Data)
{
	GetCharacterMovement()->MaxWalkSpeed = Data.NewValue;
}

void AAGActionCharacter::OnHealthAttributeChanged(const FOnAttributeChangeData& Data)
{
	if(Data.NewValue <= 0 && Data.OldValue > 0)
	{
		AAGActionCharacter* OtherCharacter = nullptr;

		if(Data.GEModData)
		{
			const FGameplayEffectContextHandle& EffectContextHandle = Data.GEModData->EffectSpec.GetEffectContext();
			OtherCharacter = Cast<AAGActionCharacter>(EffectContextHandle.GetInstigator());
		}

		FGameplayEventData EventData;
		EventData.EventTag = ZeroHealthEventTag;

		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, ZeroHealthEventTag, EventData);
	}
}

void AAGActionCharacter::StartRagDoll()
{
	USkeletalMeshComponent* SkeletalMeshComponent = GetMesh();
	
	if(SkeletalMeshComponent && !SkeletalMeshComponent->IsSimulatingPhysics())
	{
		SkeletalMeshComponent->SetCollisionProfileName(TEXT("RagDoll"));
		SkeletalMeshComponent->SetSimulatePhysics(true);
		SkeletalMeshComponent->SetAllPhysicsLinearVelocity(FVector::ZeroVector);
		SkeletalMeshComponent->SetAllPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
		SkeletalMeshComponent->WakeAllRigidBodies();
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

bool AAGActionCharacter::ApplyGameplayEffectToSelf(TSubclassOf<UGameplayEffect> Effect,
                                                   FGameplayEffectContextHandle InEffectContext)
{
	if (!Effect.Get())
	{
		return false;
	}

	FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(Effect, 1, InEffectContext);

	if (SpecHandle.IsValid())
	{
		FActiveGameplayEffectHandle ActiveGameplayEffectHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(
			*SpecHandle.Data.Get());

		return ActiveGameplayEffectHandle.WasSuccessfullyApplied();
	}

	return false;
}

UAbilitySystemComponent* AAGActionCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AAGActionCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAGActionCharacter, CharacterData);
	DOREPLIFETIME(AAGActionCharacter, InventoryComponent);
}

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Input

void AAGActionCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AAGActionCharacter::ActivateJumpAbility);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
	
		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAGActionCharacter::Move);
	
		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AAGActionCharacter::Look);

		// Crouch
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &AAGActionCharacter::OnActivateCrouchAbility);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this, &AAGActionCharacter::OnDeactivateCrouchAbility);

		// Sprint
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &AAGActionCharacter::OnActivateSprintAbility);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &AAGActionCharacter::OnDeactivateSprintAbility);

		// Equip next
		EnhancedInputComponent->BindAction(EquipNextItemAction, ETriggerEvent::Triggered, this, &AAGActionCharacter::OnEquipNextItem);

		// Drop
		EnhancedInputComponent->BindAction(DropItemAction, ETriggerEvent::Triggered, this, &AAGActionCharacter::OnDropItem);

		// UnEquip
		EnhancedInputComponent->BindAction(UnEquipItemAction, ETriggerEvent::Triggered, this, &AAGActionCharacter::OnUnEquipItem);

		// Attack
		EnhancedInputComponent->BindAction(AttackInputAction, ETriggerEvent::Started, this, &AAGActionCharacter::OnAttackStarted);
		EnhancedInputComponent->BindAction(AttackInputAction, ETriggerEvent::Completed, this, &AAGActionCharacter::OnAttackEnded);

		// Aim
		EnhancedInputComponent->BindAction(AimInputAction, ETriggerEvent::Started, this, &AAGActionCharacter::OnAimActionStarted);
		EnhancedInputComponent->BindAction(AimInputAction, ETriggerEvent::Completed, this, &AAGActionCharacter::OInAimActionEnded);
	}
}

void AAGActionCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AAGActionCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}
