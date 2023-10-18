// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/ActionGamerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GASActionGameMode.h"

void AActionGamerController::RestartPlayerIn(float InTime)
{
	ChangeState(NAME_Spectating);

	GetWorld()->GetTimerManager().SetTimer(RestartPlayerTimerHandle, this, &AActionGamerController::RestartPlayer, InTime, false);
}

void AActionGamerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if(UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InPawn))
	{
		DeathStateTagDelegate = AbilitySystemComponent->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(TEXT("State.Dead")),
			EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AActionGamerController::OnPawnDeathStateChanged);
	}
}

void AActionGamerController::OnUnPossess()
{
	Super::OnUnPossess();

	if(DeathStateTagDelegate.IsValid())
	{
		if(UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn()))
		{
			AbilitySystemComponent->UnregisterGameplayTagEvent(DeathStateTagDelegate, FGameplayTag::RequestGameplayTag(TEXT("State.Dead")), EGameplayTagEventType::NewOrRemoved);
		}
	}
}

void AActionGamerController::OnPawnDeathStateChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	if(NewCount > 0)
	{
		UWorld* World = GetWorld();

		AGASActionGameMode* GameMode = World ? Cast<AGASActionGameMode>(World->GetAuthGameMode()) : nullptr;

		if(GameMode)
		{
			GameMode->NotifyPlayerDied(this);
		}

		if(DeathStateTagDelegate.IsValid())
		{
			if(UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn()))
			{
				AbilitySystemComponent->UnregisterGameplayTagEvent(DeathStateTagDelegate, FGameplayTag::RequestGameplayTag(TEXT("State.Dead")), EGameplayTagEventType::NewOrRemoved);
			}
		}
	}
}

void AActionGamerController::RestartPlayer()
{
	UWorld* World = GetWorld();

	AGASActionGameMode* GameMode = World ? Cast<AGASActionGameMode>(World->GetAuthGameMode()) : nullptr;

	if(GameMode)
	{
		GameMode->RestartPlayer(this);
	}
}
