// Fill out your copyright notice in the Description page of Project Settings.

#include "Projectile.h"
#include "Components/SphereComponent.h"
#include "ConstructorHelpers.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/DecalActor.h"
#include "Components/DecalComponent.h"

AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	/* sphere collision  */
	ProjectileCollision = CreateDefaultSubobject<USphereComponent>(TEXT("ProjectileCollision"));
	SetRootComponent(ProjectileCollision);
	ProjectileCollision->SetSphereRadius(10.f);
	ProjectileCollision->BodyInstance.bNotifyRigidBodyCollision = true;
	ProjectileCollision->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
	ProjectileCollision->SetCollisionProfileName("BlockAll");
	ProjectileCollision->BodyInstance.bUseCCD = true;
	ProjectileCollision->SetCanEverAffectNavigation(false);

	/** mesh  */
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
	ProjectileMesh->SetupAttachment(RootComponent);
	ProjectileMesh->SetRelativeScale3D(FVector(0.05f, 0.05f, 0.05f));
	ProjectileMesh->SetCollisionProfileName("NoCollision");
	ProjectileMesh->SetGenerateOverlapEvents(false);
	ProjectileMesh->SetCanEverAffectNavigation(false);

	/**  set the default static mesh */
	static ConstructorHelpers::FObjectFinder<UStaticMesh> Sphere(TEXT("StaticMesh'/Engine/BasicShapes/Sphere.Sphere'"));
	if (Sphere.Succeeded())
	{
		ProjectileMesh->SetStaticMesh(Sphere.Object);
	}

	/* projectile movement component  */
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->InitialSpeed = 2000.f;
	ProjectileMovementComponent->MaxSpeed = 2000.f;
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->ProjectileGravityScale = 0.2f;

	InitialLifeSpan = 10.f;
	
	SetReplicates(true);
	SetReplicateMovement(true);
}

void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	/** ignore our weapon and character  */
	if (Weapon && Weapon->GetWeaponOwner())
	{
		ProjectileCollision->IgnoreActorWhenMoving(Weapon->GetWeaponOwner(), true);
		ProjectileCollision->IgnoreActorWhenMoving(Weapon, true);
	}
}

void AProjectile::NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	if (Other && Other != this && !IsPendingKill())
	{
		if (Role == ROLE_Authority)/** deal damage  */
		{
			FVector HitFromDirection = (Hit.TraceStart - Hit.TraceEnd).GetSafeNormal();
			float AppliedDamage = UGameplayStatics::ApplyPointDamage(Other, Damage, HitFromDirection, Hit,
				GetInstigatorController(), GetOwner(), nullptr);
			if (Weapon)
			{
				Weapon->OnWeaponDamagedActor(Damage, Other);
			} 
			Destroy();
		}
		else /** spawn decal  */
		{
			UWorld* const World = GetWorld();
			if (World)
			{
				FActorSpawnParameters SpawnParams;
				SpawnParams.Owner = this;
				SpawnParams.Instigator = Instigator;
				SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
					
				FTransform SpawnTransform;

				ADecalActor* SpawnedDecal = World->SpawnActor<ADecalActor>(ADecalActor::StaticClass(), SpawnTransform, SpawnParams);
				if (SpawnedDecal)
				{
					SpawnedDecal->SetLifeSpan(30.f);
					if(AFireWeapon* FireWeapon = Cast<AFireWeapon>(Weapon))
					{
						SpawnedDecal->GetDecal()->SetDecalMaterial(FireWeapon->GetHoleMaterial());
						FRotator NewRotation = (HitLocation * -1.f).Rotation();
						SpawnedDecal->SetActorRotation(NewRotation);
						SpawnedDecal->GetDecal()->SetFadeScreenSize(0.001);
					}
				}
			}
		}
	}
}
