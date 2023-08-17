// Fill out your copyright notice in the Description page of Project Settings.


#include "BlueprintFuction/ActionGameStatics.h"
#include "AGCommonTypes.h"

const UItemStaticData* UActionGameStatics::GetItemStaticData(TSubclassOf<UItemStaticData> ItemDataClass)
{
	if(IsValid(ItemDataClass))
	{
		return GetDefault<UItemStaticData>(ItemDataClass);
	}

	return nullptr;
}
