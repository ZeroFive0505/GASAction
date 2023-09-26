// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "AGPhysicalMaterial.generated.h"

class USoundBase;
class UNiagaraSystem;

UCLASS()
class GASACTION_API UAGPhysicalMaterial : public UPhysicalMaterial
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PhysicalMaterial")
	USoundBase* FootStepSoundBase;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PhysicalMaterial")
	USoundBase* PointImpactSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PhysicalMaterial")
	UNiagaraSystem* PointImpactParticle;
};
