// Fill out your copyright notice in the Description page of Project Settings.

#include "WeaponManager.h"
#include "Weapons/Weapon.h"
#include "Net/UnrealNetwork.h"
#include "Weapons/Projectiles/GrenadeProjectile.h"
#include "Weapons/Specials/WeaponSpecial.h"
#include "Weapons/ThrowableWeapon.h"
#include "GameObjects/WeaponSpawner.h"

UWeaponManager::UWeaponManager()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	
	SetIsReplicated(true);
}

void UWeaponManager::BeginPlay()
{
	Super::BeginPlay();

	/** set owner player character reference  */
	if(APCh* TestPCh = Cast<APCh>(GetOwner()))
	{
		PCh = TestPCh;
	}
}

void UWeaponManager::OnRep_CurrentWeapon()
{
	/** TODO listen server?  */
	if (GetOwnerRole() < ROLE_Authority)
	{
		/** attach new weapon accordingly   */
		if (CurrentWeapon)
		{
			CurrentWeapon->SetActorHiddenInGame(false);
			if (ClientWeaponAttached)
			{
				ClientWeaponAttached->AttachToActor(PCh, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
				ClientWeaponAttached->SetActorHiddenInGame(true);
			}
			CurrentWeapon->AttachToComponent(PCh->GetVisibleMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, "Weapon");
			ClientWeaponAttached = CurrentWeapon;
		} 
		else
		{
			ClientWeaponAttached = nullptr;
		}
		OnWeaponSelected.Broadcast(CurrentWeapon);
	}
}

AWeapon* UWeaponManager::SelectWeapon(EWeaponType WeaponType, AWeapon* Weapon, int32 Index)
{
	ServerSelectWeapon(WeaponType, Weapon, Index);
	return CurrentWeapon;
}

void UWeaponManager::ServerSelectWeapon_Implementation(EWeaponType WeaponType, AWeapon* SelectedWeapon, int32 Index)
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		if (SelectedWeapon) /** select by reference  */
		{
			CurrentWeapon = SelectedWeapon;
			OnRep_CurrentWeapon();
		}
		else /** otherwise select by type  */
		{
			switch (WeaponType)
			{
			case EWeaponType::WT_Main:
				SelectedWeapon = MainWeapon;
				break;
			case EWeaponType::WT_Secondary:
				SelectedWeapon = SecondWeapon;
				break;
			case EWeaponType::WT_Melee:
				SelectedWeapon = MeleeWeapon;
				break;
			case EWeaponType::WT_Grenade:
				if (Grenades.IsValidIndex(Index))
				{
					SelectedWeapon = Grenades[Index];
				}
				break;
			case EWeaponType::WT_Special:
				if (Specials.IsValidIndex(Index))
				{
					SelectedWeapon = Specials[Index];
				}
				break;
			}
			if (SelectedWeapon)/** success - update current weapon  */
			{
				CurrentWeapon = SelectedWeapon;
				OnRep_CurrentWeapon();
			}
		}
	}
}

AWeapon* UWeaponManager::CreateWeapon(TSubclassOf<AWeapon> WeaponClass)
{
	AWeapon* SpawnedWeapon = nullptr;
	if (WeaponClass && GetOwnerRole() == ROLE_Authority)
	{
		UWorld* const World = GetWorld();
		if (World)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = PCh->GetController();
			SpawnParams.Instigator = PCh;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			
			FVector SpawnLocation = FVector(0.f, 0.f, 0.f);
			FRotator SpawnRotation = FRotator(0.f, 0.f, 0.f);
			
			SpawnedWeapon = World->SpawnActor<AWeapon>(WeaponClass, SpawnLocation, SpawnRotation, SpawnParams);
			if (SpawnedWeapon)
			{
				SpawnedWeapon->SetWeaponOwner(PCh);
				AddWeapon(SpawnedWeapon);
			}
		}
	}
	return SpawnedWeapon;
}

bool UWeaponManager::ScrollWeapon(bool bNext)
{
	bool Success = false;

	/** client scroll weapon, if success select this weapon on server  */

	/** build weapons array  */
	TArray<AWeapon*> Weapons;
	BuildWeaponsArray(Weapons);
	
	if (Weapons.Num() > 0)
	{
		int32 Index = Weapons.Find(CurrentWeapon);
		if (Index >= 0)
		{
			int32 NextWeaponIndex = 0;

			if (bNext)
			{	
				/** should we set index to 0 ?  */
				if (Index + 1 > Weapons.Num() - 1)
				{
					NextWeaponIndex = 0;
				}
				else
				{
					NextWeaponIndex = Index + 1;
				}
			} 
			else
			{
				/** should we set index to last index ?  */
				if (Index - 1 < 0)
				{
					NextWeaponIndex = Weapons.Num() - 1;
				} 
				else
				{
					NextWeaponIndex = Index - 1;
				}
			}
			
			Success = true;
			if (Weapons.IsValidIndex(NextWeaponIndex))
			{
				CurrentWeapon = Weapons[NextWeaponIndex];
				OnRep_CurrentWeapon();
			}
		}
		else
		{
			Success = true;
			if (Weapons.IsValidIndex(0))
			{
				CurrentWeapon = Weapons[0];
				OnRep_CurrentWeapon();
			}
		}
	}
	
	if (Success)
	{
		ServerSelectWeapon(EWeaponType::WT_Main, CurrentWeapon, 0);
	}
	
	return Success;
}

void UWeaponManager::BuildWeaponsArray(TArray<AWeapon *> &Weapons)
{
	if (MainWeapon)
	{
		Weapons.Add(MainWeapon);
	}
	if (SecondWeapon)
	{
		Weapons.Add(SecondWeapon);
	}
	if (MeleeWeapon)
	{
		Weapons.Add(MeleeWeapon);
	}
	Weapons.Append(Grenades);
	Weapons.Append(Specials);
}

void UWeaponManager::AddWeapon(AWeapon* Weapon)
{
	if (Weapon && PCh && GetOwnerRole() == ROLE_Authority)
	{
		EWeaponType WeaponType = Weapon->GetWeaponType();
		switch (WeaponType)
		{
		case EWeaponType::WT_Main:
			DropWeapon(MainWeapon);
			MainWeapon = Weapon;
			break;
		case EWeaponType::WT_Secondary:
			DropWeapon(SecondWeapon);
			SecondWeapon = Weapon;
			break;
		case EWeaponType::WT_Melee:
			DropWeapon(MeleeWeapon);
			MeleeWeapon = Weapon;
			break;
		case EWeaponType::WT_Grenade:
			Grenades.Add(Weapon);
			break;
		case EWeaponType::WT_Special:
			Specials.Add(Weapon);
			break;
		}

		Weapon->SetWeaponOwner(PCh);
		Weapon->SetOwner(PCh);
		Weapon->Instigator = PCh;
		if (!CurrentWeapon)
		{
			CurrentWeapon = Weapon;
			OnRep_CurrentWeapon();
		}
	}
}

void UWeaponManager::DropWeapon(AWeapon* Weapon)
{
	if (Weapon)
	{
		ServerDropWeapon(Weapon);
	}
}

void UWeaponManager::ServerDropWeapon_Implementation(class AWeapon* Weapon)
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		if (Weapon)
		{
			/** our weapon ? */
			if (Weapon->GetWeaponOwner() == PCh)
			{
				EWeaponType WeaponType = Weapon->GetWeaponType();
				int32 Index = INDEX_NONE;
				switch (WeaponType)
				{
				case EWeaponType::WT_Main:
					MainWeapon = nullptr;
					break;
				case EWeaponType::WT_Secondary:
					SecondWeapon = nullptr;
					break;
				case EWeaponType::WT_Melee:
					MeleeWeapon = nullptr;
					break;
				case EWeaponType::WT_Special:
					Index = Specials.Find(Weapon);
					if (Index != INDEX_NONE && Specials.IsValidIndex(Index))
					{
						Specials.RemoveAt(Index);
					}
					break;
				case EWeaponType::WT_Grenade:
					Index = Grenades.Find(Weapon);
					if (Index != INDEX_NONE && Grenades.IsValidIndex(Index))
					{
						Grenades.RemoveAt(Index);
					}
					break;
				case EWeaponType::WT_None:

					break;
				}
				
				Weapon->OnWeaponDropped();
				Weapon->SetActorHiddenInGame(true);
				Weapon->SetLifeSpan(1.f);
				ScrollWeapon(true);
			}
		}
	}
}

bool UWeaponManager::HasWeaponByType(EWeaponType WeaponType) const
{
	switch (WeaponType)
	{
	case EWeaponType::WT_Main:
		if (MainWeapon)
		{
			return true;
		}
		break;
	case EWeaponType::WT_Secondary:
		if (SecondWeapon)
		{
			return true;
		}
		break;
	case EWeaponType::WT_Melee:
		if (MeleeWeapon)
		{
			return true;
		}
		break;
	case EWeaponType::WT_Grenade:
		if (Grenades.Num() > 0)
		{
			return true;
		}
		break;
	case EWeaponType::WT_Special:
		if (Specials.Num() > 0)
		{
			return true;
		}
		break;
	}
	return false;
}

bool UWeaponManager::FindWeaponByClass(TSubclassOf<class AWeapon> WeaponClass, class AWeapon* FoundedWeapon) const
{
	FoundedWeapon = nullptr;

	if (MainWeapon && MainWeapon->GetClass() == WeaponClass)
	{
		FoundedWeapon = MainWeapon;
		return true;
	} 
	else if(SecondWeapon && SecondWeapon->GetClass() == WeaponClass)
	{
		FoundedWeapon = SecondWeapon;
		return true;
	}
	else if (MeleeWeapon && MeleeWeapon->GetClass() == WeaponClass)
	{
		FoundedWeapon = MeleeWeapon;
		return true;
	}

	for (AWeapon* TestWeapon : Specials)
	{
		if (TestWeapon && TestWeapon->GetClass() == WeaponClass)
		{
			FoundedWeapon = TestWeapon;
			return true;
		}
	}

	for (AWeapon* TestWeapon : Grenades)
	{
		if (TestWeapon && TestWeapon->GetClass() == WeaponClass)
		{
			FoundedWeapon = TestWeapon;
			return true;
		}
	}
	
	return false;
}

void UWeaponManager::SetCurrentWeapon(class AWeapon* NewWeapon)
{
	CurrentWeapon = NewWeapon;
	OnRep_CurrentWeapon();
}

void UWeaponManager::DestroyAllWeapons()
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		/** build weapons array  */
		TArray<AWeapon*> Weapons;
		BuildWeaponsArray(Weapons);

		for (AWeapon* Weapon : Weapons)
		{
			if (Weapon)
			{
				Weapon->Destroy();
			}
		}
	}
}

void UWeaponManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UWeaponManager, CurrentWeapon);
	DOREPLIFETIME(UWeaponManager, PCh);

	DOREPLIFETIME_CONDITION(UWeaponManager, MainWeapon, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UWeaponManager, SecondWeapon, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UWeaponManager, MeleeWeapon, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UWeaponManager, Specials, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UWeaponManager, Grenades, COND_OwnerOnly);
}
