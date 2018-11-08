// Fill out your copyright notice in the Description page of Project Settings.

#include "ThrowableWeapon.h"
#include "Weapons/Projectiles/Projectile.h"
#include "Weapons/Projectiles/GrenadeProjectile.h"
#include "ConstructorHelpers.h"

AThrowableWeapon::AThrowableWeapon()
{
	/** mesh  */
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);
	WeaponMesh->SetRelativeScale3D(FVector(0.2f, 0.2f, 0.2f));

	/**  set the default static mesh */
	static ConstructorHelpers::FObjectFinder<UStaticMesh> Sphere(TEXT("StaticMesh'/Engine/BasicShapes/Sphere.Sphere'"));
	if (Sphere.Succeeded())
	{
		WeaponMesh->SetStaticMesh(Sphere.Object);
	}

	ThrowProjectileClass = AGrenadeProjectile::StaticClass();
}

bool AThrowableWeapon::UseWeapon()
{
	if (bMustHoldToThrow)
	{
		if (!ThrowProjectileTimer.IsValid())
		{
			GetWorldTimerManager().SetTimer(ThrowProjectileTimer, this, &AThrowableWeapon::ThrowProjectile, HoldTime, false);
		}
	} 
	else
	{
		ThrowProjectile();
	}

	/** we always return true we only call Super::UseWeapon();
	*	if we successfully spawn projectile		*/
	return true;
}

void AThrowableWeapon::StopUseWeapon()
{
	Super::StopUseWeapon();
	GetWorldTimerManager().ClearTimer(ThrowProjectileTimer);
}

void AThrowableWeapon::ThrowProjectile()
{
	GetWorldTimerManager().ClearTimer(ThrowProjectileTimer);
	ThrowProjectileTimer.Invalidate();
	if (APCh* PCh = Cast<APCh>(GetInstigator()))
	{
		if (ThrowProjectileClass && GetOwner() && Role == ROLE_Authority && WeaponOwner)
		{
			UWorld* const World = GetWorld();
			if (World)
			{
				FActorSpawnParameters SpawnParams;
				SpawnParams.Owner = this;
				SpawnParams.Instigator = GetInstigator();
				SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				
				FVector SpawnLocation;
				FRotator SpawnRotation;
				PCh->GetActorEyesViewPoint(SpawnLocation, SpawnRotation);
				SpawnLocation += (FVector(0.f, 0.f, 40.f) + GetActorForwardVector() * 50.f);
				
				/** throw a projectile  */
				AProjectile* SpawnedProjectile = World->SpawnActor<AProjectile>(ThrowProjectileClass, SpawnLocation, SpawnRotation, SpawnParams);
				if (SpawnedProjectile)
				{
					/** we successfully use a weapon  */
					Super::UseWeapon();
					SpawnedProjectile->SetWeapon(this);
					SpawnedProjectile->SetDamage(Damage);
					if (CurrentAmmo <= 0)
					{
						WeaponOwner->GetWeaponManager()->ScrollWeapon(true);
						WeaponOwner->GetWeaponManager()->DropWeapon(this);
					}
				}
			}
		}
	}
}
