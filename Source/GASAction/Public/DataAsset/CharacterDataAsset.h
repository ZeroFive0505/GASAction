// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AGCommonTypes.h"
#include "Engine/DataAsset.h"
#include "CharacterDataAsset.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class GASACTION_API UCharacterDataAsset : public UDataAsset
{
	GENERATED_BODY()


public:
	UPROPERTY(EditDefaultsOnly)
	FCharacterData CharacterData;
};
