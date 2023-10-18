// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Projectile.h"

#include "NiagaraFunctionLibrary.h"
#include "BlueprintFuction/ActionGameStatics.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

static TAutoConsoleVariable<int32> CVarShowProjectiles(
	TEXT("ShowDebugProjectiles"),
	0,
	TEXT("Draws debug info about projectiles")
	TEXT("  0: off/n")
	TEXT("  1: on/n"),
	ECVF_Cheat);


// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;


    AActor::SetReplicateMovement(true);
	bReplicates = true;

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));

	ProjectileMovementComponent->ProjectileGravityScale = 0.0f;
	ProjectileMovementComponent->Velocity = FVector::ZeroVector;
	ProjectileMovementComponent->OnProjectileStop.AddDynamic(this, &AProjectile::OnProjectileStop);

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));

	StaticMeshComponent->SetupAttachment(GetRootComponent());
	StaticMeshComponent->SetIsReplicated(true);
	StaticMeshComponent->SetCollisionProfileName(TEXT("Projectile"));
	StaticMeshComponent->bReceivesDecals = false;
}

const UProjectileStaticData* AProjectile::GetProjectileStaticData() const
{
	if(IsValid(ProjectileDataClass))
	{
		return GetDefault<UProjectileStaticData>(ProjectileDataClass);
	}

	return nullptr;
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	const UProjectileStaticData* ProjectileStaticData = GetProjectileStaticData();

	if(ProjectileStaticData && ProjectileMovementComponent)
	{
		if(ProjectileStaticData->StaticMesh)
		{
			StaticMeshComponent->SetStaticMesh(ProjectileStaticData->StaticMesh);
		}

		ProjectileMovementComponent->bInitialVelocityInLocalSpace = false;
		ProjectileMovementComponent->InitialSpeed = ProjectileStaticData->InitialSpeed;
		ProjectileMovementComponent->MaxSpeed = ProjectileStaticData->MaxSpeed;
		ProjectileMovementComponent->bRotationFollowsVelocity = true;
		ProjectileMovementComponent->bShouldBounce = false;
		ProjectileMovementComponent->Bounciness = 0.0f;
		ProjectileMovementComponent->ProjectileGravityScale = ProjectileStaticData->GravityMultiplier;

		ProjectileMovementComponent->Velocity = ProjectileStaticData->InitialSpeed * GetActorForwardVector();
	}

	const int32 DebugShowProjectile = CVarShowProjectiles.GetValueOnAnyThread();

	if(DebugShowProjectile)
	{
		DrawDebugPath();
	}
}

void AProjectile::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	const UProjectileStaticData* ProjectileStaticData = GetProjectileStaticData();

	if(ProjectileStaticData)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ProjectileStaticData->OnStopSFX, GetActorLocation(), 1.0f);

		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ProjectileStaticData->OnStopVFX, GetActorLocation());
	}
	
	Super::EndPlay(EndPlayReason);
}

void AProjectile::DrawDebugPath() const
{
	const UProjectileStaticData* ProjectileStaticData = GetProjectileStaticData();

	if(ProjectileStaticData)
	{
		FPredictProjectilePathParams ProjectilePathParams;
		ProjectilePathParams.StartLocation = GetActorLocation();
		ProjectilePathParams.LaunchVelocity = ProjectileStaticData->InitialSpeed * GetActorForwardVector();
		ProjectilePathParams.TraceChannel = ECC_Visibility;
		ProjectilePathParams.bTraceComplex = true;
		ProjectilePathParams.bTraceWithCollision = true;
		ProjectilePathParams.DrawDebugType = EDrawDebugTrace::ForDuration;
		ProjectilePathParams.DrawDebugTime = 3.0f;
		ProjectilePathParams.OverrideGravityZ = ProjectileStaticData->GravityMultiplier == 0.0f ? 0.0001f : ProjectileStaticData->GravityMultiplier;

		FPredictProjectilePathResult PredictProjectilePathResult;
		if(UGameplayStatics::PredictProjectilePath(this, ProjectilePathParams, PredictProjectilePathResult))
		{
			DrawDebugSphere(GetWorld(), PredictProjectilePathResult.HitResult.Location, 50.0f, 10.0f, FColor::Red);
		}
	}
}

void AProjectile::OnProjectileStop(const FHitResult& ImpactResult)
{
	const UProjectileStaticData* ProjectileStaticData = GetProjectileStaticData();

	if(ProjectileStaticData)
	{
		UActionGameStatics::ApplyRadialDamage(this, GetOwner(), GetActorLocation(),
			ProjectileStaticData->DamageRadius,
			ProjectileStaticData->BaseDamage * 0.2f,
			ProjectileStaticData->Effects,
			ProjectileStaticData->RadialDamageQueryType,
			ProjectileStaticData->RadialDamageTraceType);
	}

	Destroy();
}

void AProjectile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AProjectile, ProjectileDataClass);
}
