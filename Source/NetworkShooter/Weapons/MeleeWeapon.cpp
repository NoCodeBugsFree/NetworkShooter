// Fill out your copyright notice in the Description page of Project Settings.

#include "MeleeWeapon.h"
#include "ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/EngineTypes.h"

AMeleeWeapon::AMeleeWeapon()
{
	/** mesh  */
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);

	/**  set the default static mesh */
	static ConstructorHelpers::FObjectFinder<UStaticMesh> Cube(TEXT("StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
	if (Cube.Succeeded())
	{
		WeaponMesh->SetStaticMesh(Cube.Object);
	}

	/** default knife-like weapon  */
	WeaponMesh->SetRelativeLocation(FVector(0.f, 0.f, 20.f));
	WeaponMesh->SetRelativeScale3D(FVector(0.05f, 0.05f, 0.4f));
}

bool AMeleeWeapon::UseWeapon()
{
	if (Super::UseWeapon())
	{
		if (Role == ROLE_Authority)/** server  */
		{
			FHitResult Hit;
			/** if we hit anyone - deal damage, send notify  */
			if (GetOverlapInfo(Hit))
			{
				FVector HitFromDirection = (Hit.TraceStart - Hit.TraceEnd).GetSafeNormal();
				float AppliedDamage = UGameplayStatics::ApplyPointDamage(Hit.GetActor(), Damage, HitFromDirection,
					Hit, GetInstigatorController(), this, nullptr);
				/** send notify  */
				OnWeaponDamagedActor(AppliedDamage, Hit.GetActor());
			}
		}
		else/** client FX  */
		{
			PlayWeaponFX();
		}
		return true;
	} 
	return false;
}

bool AMeleeWeapon::CanUseWeapon() const
{
	return !CooldownTimer.IsValid();
}

bool AMeleeWeapon::GetOverlapInfo(FHitResult& Hit)
{
	if (APCh* PCh = Cast<APCh>(GetInstigator()))
	{
		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_PhysicsBody));

		TArray<AActor*> ActorsToIgnore;
		ActorsToIgnore.Add(this);
		ActorsToIgnore.Add(PCh);

		TArray<AActor*> OutActors;

		FVector Start = PCh->GetCameraComponent()->GetComponentLocation();
		FVector End = Start + GetOwner()->GetActorForwardVector() * Radius;
		
		if (UKismetSystemLibrary::SphereTraceSingleForObjects(this, Start, End, Radius, ObjectTypes, false, ActorsToIgnore, EDrawDebugTrace::None, Hit, true))
		{
			return true;
		}
	}
	return false;
}

