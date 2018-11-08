// Fill out your copyright notice in the Description page of Project Settings.

#include "WeaponSpecial.h"
#include "Kismet/KismetSystemLibrary.h"
#include "DrawDebugHelpers.h"

AWeaponSpecial::AWeaponSpecial()
{
	PrimaryActorTick.bCanEverTick = true;

	WeaponType = EWeaponType::WT_Special;
	CurrentAmmo = 1;
	SpecialClass = ASpecialTurret::StaticClass();
}

void AWeaponSpecial::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	/** not dedicated server  */
	if (GetWorld() && GetWorld()->GetNetMode() != NM_DedicatedServer)
	{
		/** this visualization is only for our player  */
		if (WeaponOwner && WeaponOwner->IsLocallyControlled())
		{
			/** is this weapon equipped (in arms ?)  */
			if (WeaponOwner->GetWeaponManager()->GetCurrentWeapon() == this)
			{
				/** move previously spawned special to hit point  */
				if (SpecialBeforeSet)
				{
					float Distance = MaxDistanceToSetWeapon * 2.f;
					
					FVector Direction = WeaponOwner->GetCameraComponent()->GetForwardVector();
					FVector Start = WeaponOwner->GetCameraComponent()->GetComponentLocation();
					FVector End = Start + (Direction * Distance);
					
					FHitResult Hit;
					FCollisionQueryParams CollisionQueryParams;
					CollisionQueryParams.AddIgnoredActor(this);
					CollisionQueryParams.AddIgnoredActor(SpecialBeforeSet);
					CollisionQueryParams.AddIgnoredActor(WeaponOwner);
					CollisionQueryParams.bTraceComplex = false;
					
					if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, CollisionQueryParams))
					{	
						/** line trace successful - place special weapon at hit location */
						SpecialBeforeSet->SetActorHiddenInGame(false);
						SpecialBeforeSet->SetActorLocation(Hit.Location);
						SpecialBeforeSet->SetActorRotation(WeaponOwner->GetActorForwardVector().Rotation());

						/** collision check and highlight special according can we place it here or not */
						HighlightSpecial(Start, End, Hit);
					}
					else
					{
						SpecialBeforeSet->Destroy();
						SpecialBeforeSet = nullptr;
					}
				} 
				else /** spawn special  */
				{
					SpawnSpecial();
				}
			} 
			else
			{
				if (SpecialBeforeSet)
				{
					SpecialBeforeSet->Destroy();
					SpecialBeforeSet = nullptr;
				}
			}
		}
	}
}

void AWeaponSpecial::HighlightSpecial(FVector &Start, FVector &End, FHitResult &Hit)
{
	Start = SpecialBeforeSet->GetBlockingCollision()->GetComponentLocation();
	End = Start;
	FVector HalfSize = SpecialBeforeSet->GetBlockingCollision()->GetScaledBoxExtent();
	FRotator Orientation = SpecialBeforeSet->GetBlockingCollision()->GetComponentRotation();

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_PhysicsBody));

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(SpecialBeforeSet);
	ActorsToIgnore.Add(this);
	ActorsToIgnore.Add(WeaponOwner);

	FHitResult OutHit;
	bool bBoxOverlaps = UKismetSystemLibrary::BoxTraceSingleForObjects(this, Start, End, HalfSize, Orientation, ObjectTypes, false, ActorsToIgnore, EDrawDebugTrace::None, OutHit, true);
	float Distance = (SpecialBeforeSet->GetActorLocation() - Hit.TraceStart).Size();

	/** highlight can we setup the special weapon or not  */
	bCanSetupSpecial = !(bBoxOverlaps || Distance > MaxDistanceToSetWeapon);

	if (bCanSetupSpecial)
	{
		SpecialBeforeSet->SetColor(FLinearColor::Green);
	}
	else
	{
		SpecialBeforeSet->SetColor();
	}
}

void AWeaponSpecial::SpawnSpecial()
{
	if (SpecialClass)
	{
		UWorld* const World = GetWorld();
		if (World)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			FVector SpawnLocation = FVector::ZeroVector;
			FRotator SpawnRotation = FRotator::ZeroRotator;

			ASpecial* SpawnedSpecial = World->SpawnActor<ASpecial>(SpecialClass, SpawnLocation, SpawnRotation, SpawnParams);
			if (SpawnedSpecial)
			{
				SpecialBeforeSet = SpawnedSpecial;
				SpecialBeforeSet->SetActorEnableCollision(false);
				SpecialBeforeSet->SetActorHiddenInGame(true);
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("SpecialClass == NULL!"));
	}
}

void AWeaponSpecial::SetSpecial()
{
	if (WeaponOwner && WeaponOwner->GetController())
	{
		float Distance = 500.f;
		
		FVector Direction = WeaponOwner->GetControlRotation().Vector();
		FVector Start = WeaponOwner->GetActorLocation() + FVector(0.f, 0.f, 50.f);
		FVector End = Start + (Direction * Distance);
		
		FHitResult Hit;
		FCollisionQueryParams CollisionQueryParams;
		CollisionQueryParams.AddIgnoredActor(this);
		CollisionQueryParams.AddIgnoredActor(WeaponOwner);
		CollisionQueryParams.bTraceComplex = false;
		
		if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, CollisionQueryParams))
		{
			if (SpecialClass)
			{
				UWorld* const World = GetWorld();
				if (World)
				{
					FActorSpawnParameters SpawnParams;
					SpawnParams.Owner = this;
					SpawnParams.Instigator = Instigator; //  GetController();
					SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
					
					FVector SpawnLocation = Hit.Location;
					FRotator SpawnRotation = WeaponOwner->GetActorRotation();
					
					ASpecial* SpawnedSpecial = World->SpawnActor<ASpecial>(SpecialClass, SpawnLocation, SpawnRotation, SpawnParams);
					if (SpawnedSpecial)
					{
						SpawnedSpecial->SetDamage(Damage);
					}
				}
			}
		}
	}
}

void AWeaponSpecial::ToggleUseWeapon(bool bUse)
{
	/** set special only if we can  */
	if (bCanSetupSpecial)
	{
		Super::ToggleUseWeapon(true);
	} 
	else
	{
		Super::ToggleUseWeapon(false);
	}
}

void AWeaponSpecial::OnWeaponDropped()
{
	Super::OnWeaponDropped();

	if (SpecialBeforeSet)
	{
		SpecialBeforeSet->Destroy();
	}
}

void AWeaponSpecial::Destroyed()
{
	Super::Destroyed();

	if (Role < ROLE_Authority)
	{
		if (SpecialBeforeSet)
		{
			SpecialBeforeSet->Destroy();
		}
	}
}

bool AWeaponSpecial::UseWeapon()
{
	if (Super::UseWeapon())
	{
		if (Role == ROLE_Authority)
		{
			SetSpecial();
			if (CurrentAmmo <= 0)
			{
				if (WeaponOwner)
				{
					WeaponOwner->DropWeapon();
				}
			}
			return true;
		}
	} 
	return false;
}

