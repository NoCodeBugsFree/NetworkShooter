// Fill out your copyright notice in the Description page of Project Settings.

#include "FireWeapon.h"
#include "Net/UnrealNetwork.h"
#include "ConstructorHelpers.h"
#include "Materials/MaterialInterface.h"
#include "Weapons/Projectiles/BulletProjectile.h"
#include "Kismet/GameplayStatics.h"

AFireWeapon::AFireWeapon()
{
	/* skeletal mesh component  */
	GunMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunMesh"));
	SetRootComponent(GunMesh);
	GunMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	GunMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	GunMesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Block);
	
	/**  set hole material */
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MaterialTemplate(TEXT("/Game/Assets/Weapons/Fireholes/M_Firehole"));
	if (MaterialTemplate.Succeeded())
	{
		HoleMaterial = MaterialTemplate.Object;
	}
	
	/** class defaults  */
	CurrentAmmo = 20;
	UseRate = 0.1f;
}

void AFireWeapon::ReloadWeapon()
{
	if (CanReload())
	{
		ServerReloadWeapon();

		/** start reloading locally */
		if (!bReloading)
		{
			bReloading = true;
			OnRep_Reloading();
		}
	}
}

FVector AFireWeapon::GetMuzzleLocation() const
{
	if (GunMesh)
	{
		return GunMesh->GetSocketLocation(MuzzleSocketName);
	}
	UE_LOG(LogTemp, Error, TEXT("Error! GetMuzzleLocation is zero vector !"));
	return FVector::ZeroVector;
}

bool AFireWeapon::UseWeapon()
{
	if (Super::UseWeapon())
	{
		if (Role == ROLE_Authority)
		{
			/** projectile or linetrace?  */
			if (ProjectileClass)
			{
				SpawnProjectile();
			} 
			else
			{
				FireTrace();
			}

			/** auto reload weapon if ammo is depleted  */
			if (!IsOtherPlayer())
			{
				if (CurrentAmmo <= 0 && TotalAmmo > 0)
				{
					ReloadWeapon();
				}
			}
		}
		else
		{
			PlayWeaponFX();
		}
		return true;
	}
	return false;
}

void AFireWeapon::SpawnProjectile()
{
	UWorld* const World = GetWorld();
	if (ProjectileClass && World && WeaponOwner)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = GetOwner();
		SpawnParams.Instigator = GetInstigator();
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		
		FVector SpawnLocation = WeaponOwner->GetCameraComponent()->GetComponentLocation() + 
			FRotationMatrix(WeaponOwner->GetControlRotation()).GetScaledAxis(EAxis::X) * 50.f;
		FRotator SpawnRotation = GetShootDirection().Rotation();
		
		/** spawn projectile  */
		ABulletProjectile* SpawnedBulletProjectile = World->SpawnActor<ABulletProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, SpawnParams);
		if (SpawnedBulletProjectile)
		{
			SpawnedBulletProjectile->SetDamage(Damage);
			SpawnedBulletProjectile->SetWeapon(this);

			/** ignore owner and weapon  */
			SpawnedBulletProjectile->GetProjectileCollision()->IgnoreActorWhenMoving(this, true);
			SpawnedBulletProjectile->GetProjectileCollision()->IgnoreActorWhenMoving(WeaponOwner, true);
		}
	}
}

void AFireWeapon::FireTrace()
{
	if (WeaponOwner)
	{
		FVector Direction = GetShootDirection();
		FVector Start = WeaponOwner->GetCameraComponent()->GetComponentLocation();
		FVector End = Start + (Direction * ShootDistance);
		
		FCollisionQueryParams CollisionQueryParams;
		CollisionQueryParams.AddIgnoredActor(this);
		if (GetInstigator())
		{
			CollisionQueryParams.AddIgnoredActor(GetInstigator());
		}
		CollisionQueryParams.bTraceComplex = false;
		
		FCollisionObjectQueryParams ObjectQueryParams;
		ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);
		ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
		ObjectQueryParams.AddObjectTypesToQuery(ECC_PhysicsBody);
		
		FHitResult Hit;

		/** line trace and damage  */
		if (GetWorld()->LineTraceSingleByObjectType(Hit, Start, End, ObjectQueryParams, CollisionQueryParams))
		{
			FVector HitFromDirection = (Hit.TraceStart - Hit.TraceEnd).GetSafeNormal();
			float AppliedDamage = UGameplayStatics::ApplyPointDamage(Hit.GetActor(), Damage, HitFromDirection, Hit, GetInstigatorController(), this, nullptr);
			
			/** notify owner  */
			OnWeaponDamagedActor(AppliedDamage, Hit.GetActor());
		}
	}
}

bool AFireWeapon::CanUseWeapon() const
{
	return Super::CanUseWeapon() && !bReloading;
}

void AFireWeapon::PlayWeaponFX_Implementation()
{
	Super::PlayWeaponFX_Implementation();
}

void AFireWeapon::OnRep_Reloading()
{
	if (bReloading)
	{
		if (WeaponOwner && WeaponOwner->GetVisibleMesh())
		{
			if (UFPSAnimInstance* FPSAnimInstance = Cast<UFPSAnimInstance>(WeaponOwner->GetVisibleMesh()->GetAnimInstance()))
			{
				FPSAnimInstance->PlayReloadAnimation();
			}
		}

		if (!IsOtherPlayer())
		{
			GetWorldTimerManager().SetTimer(ReloadTimer, this, &AFireWeapon::ReloadFinished, ReloadTime, false);
			AmmoToReload = FMath::Min(GetDefault<AFireWeapon>(GetClass())->CurrentAmmo - CurrentAmmo, TotalAmmo);
		}
		OnReloadStarted.Broadcast();
	} 
	else
	{
		GetWorldTimerManager().ClearTimer(ReloadTimer);
		OnReloadFinished.Broadcast();
	}
}

void AFireWeapon::ServerReloadWeapon_Implementation()
{
	if (CanReload())
	{
		if (!bReloading)
		{
			bReloading = true;
			OnRep_Reloading();
		}
	}
}

FVector AFireWeapon::GetShootDirection() const
{
	/** TODO do cone spread by FMath */
	if (WeaponOwner)
	{
		/** rotate Y */
		FVector RotatedPitchVector = WeaponOwner->GetActorForwardVector().RotateAngleAxis(
			WeaponOwner->GetAimPitch() * -1.f,WeaponOwner->GetActorRightVector());

		FRotator Rotation = RotatedPitchVector.Rotation();
		FVector RightVector = FRotationMatrix(Rotation).GetScaledAxis(EAxis::Y);
		FVector UpVector = FRotationMatrix(Rotation).GetScaledAxis(EAxis::Z);
		
		float RandomAngle = RandomStream.FRandRange(SpreadAngle * -1.f, SpreadAngle);
		/** rotate again  */
		FVector RotatedRight = RotatedPitchVector.RotateAngleAxis(RandomAngle, RightVector);

		RandomAngle = RandomStream.FRandRange(SpreadAngle * -1.f, SpreadAngle);

		/** last rotation  */
		return RotatedRight.RotateAngleAxis(RandomAngle, UpVector);
	}
	return FVector::ZeroVector;
}

void AFireWeapon::ReloadFinished()
{
	bReloading = false;
	OnRep_Reloading();
	CurrentAmmo += AmmoToReload;
	TotalAmmo -= AmmoToReload;
}

bool AFireWeapon::CanReload() const
{
	return  GetDefault<AFireWeapon>(GetClass())->CurrentAmmo != CurrentAmmo &&  /** current ammo != max clip ammo  */
			TotalAmmo > 0 &&													/** ammo is positive  */
			!ReloadTimer.IsValid();												/** not reloading right now  */	
}

float AFireWeapon::GetRemainingReloadTime() const
{
	return ReloadTimer.IsValid() ? GetWorld()->GetTimerManager().GetTimerRemaining(ReloadTimer) : 0.f;
}

void AFireWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFireWeapon, bReloading);
	
	DOREPLIFETIME_CONDITION(AFireWeapon, TotalAmmo, COND_OwnerOnly);
}
