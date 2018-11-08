// Fill out your copyright notice in the Description page of Project Settings.

#include "GrenadeProjectile.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "ConstructorHelpers.h"
#include "Particles/ParticleSystem.h"

AGrenadeProjectile::AGrenadeProjectile()
{
	ProjectileCollision->BodyInstance.bSimulatePhysics = true;
	ProjectileMesh->SetRelativeScale3D(FVector(0.05f, 0.05f, 0.07f));
	ProjectileMovementComponent->InitialSpeed = 1000.f;
	ProjectileMovementComponent->MaxSpeed = 1000.f;
	ProjectileMovementComponent->bRotationFollowsVelocity = false;
	ProjectileMovementComponent->ProjectileGravityScale = 1.f;
	ProjectileMovementComponent->bShouldBounce = true;

	/** set the explosion emitter  */
	static ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleSystem(TEXT("/Engine/Tutorial/SubEditors/TutorialAssets/TutorialParticleSystem"));
	if (ParticleSystem.Object)
	{
		EmitterTemplate = ParticleSystem.Object;
	}

	/** set the sound asset  */
	static ConstructorHelpers::FObjectFinder<USoundBase> EXplosionSoundTemplate(TEXT("/Game/Assets/Sounds/335152__apenguin73__explosion-test"));
	if (EXplosionSoundTemplate.Object)
	{
		ExplodeSound = EXplosionSoundTemplate.Object;
	}

	/** set the sound attenuation asset  */
	static ConstructorHelpers::FObjectFinder<USoundAttenuation> SoundAttenuationTemplate(TEXT("/Game/Assets/Sounds/SA_Explosion"));
	if (SoundAttenuationTemplate.Object)
	{
		AttenuationSettings = SoundAttenuationTemplate.Object;
	}
}

void AGrenadeProjectile::BeginPlay()
{
	Super::BeginPlay();

	if (Role == ROLE_Authority)
	{
		/** init grenade speed  */
		ProjectileCollision->AddImpulse(GetActorForwardVector() * ImpulseStrength);
		FVector Impulse;
		Impulse.X = FMath::FRandRange(-5.f, 5.f);
		Impulse.Y = FMath::FRandRange(-5.f, 5.f);
		Impulse.Z = FMath::FRandRange(-5.f, 5.f);
		Impulse *= 500.f;
		ProjectileCollision->AddAngularImpulseInDegrees(Impulse);

		/** explode by timer  */
		FTimerHandle ExplodeTimer;
		GetWorldTimerManager().SetTimer(ExplodeTimer, this, &AGrenadeProjectile::Explode, TimeToExplode, false);
	}
}

void AGrenadeProjectile::NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	/** we don't use parent explosion on touch, we use delayed explosion  */
}

void AGrenadeProjectile::OnRep_Exploded()
{
	if (GetWorld()->GetNetMode() != NM_DedicatedServer)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), EmitterTemplate, GetActorLocation());
		UGameplayStatics::PlaySoundAtLocation(this, ExplodeSound, GetActorLocation(), 1.f, 1.f, 0.f, AttenuationSettings);
	}
}

void AGrenadeProjectile::Explode()
{
	bExploded = true;
	OnRep_Exploded();
	
	TArray<AActor*> IgnoredActors;
	IgnoredActors.Add(this);

	UGameplayStatics::ApplyRadialDamageWithFalloff(this, Damage, MinimumDamage, GetActorLocation(), DamageInnerRadius, DamageOuterRadius, DamageFalloff, nullptr, IgnoredActors, this, GetInstigatorController(), ECC_Visibility);
	if (Weapon)
	{
		/** TODO make a sphere trace and collect an array of victims, for each victim DO */
		// Weapon->OnWeaponDamagedActor(Damage, Victim);
	}
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	SetLifeSpan(10.f);
}

void AGrenadeProjectile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGrenadeProjectile, bExploded);
}
