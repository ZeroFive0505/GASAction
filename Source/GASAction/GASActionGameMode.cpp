// Copyright Epic Games, Inc. All Rights Reserved.

#include "GASActionGameMode.h"

#include "Controller/ActionGamerController.h"
#include "UObject/ConstructorHelpers.h"

AGASActionGameMode::AGASActionGameMode()
{
	// set default pawn class to our Blueprinted character
	// static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	// if (PlayerPawnBPClass.Class != NULL)
	// {
	// 	DefaultPawnClass = PlayerPawnBPClass.Class;
	// }


	PlayerControllerClass = AActionGamerController::StaticClass();
}

void AGASActionGameMode::NotifyPlayerDied(AActionGamerController* PlayerController)
{
	if(PlayerController)
	{
		PlayerController->RestartPlayerIn(2.0f);
	}
}
