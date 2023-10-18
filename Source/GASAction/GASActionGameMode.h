// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GASActionGameMode.generated.h"

class AActionGamerController;

UCLASS(minimalapi)
class AGASActionGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AGASActionGameMode();

	void NotifyPlayerDied(AActionGamerController* PlayerController);
};



