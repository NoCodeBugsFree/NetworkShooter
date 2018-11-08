// Fill out your copyright notice in the Description page of Project Settings.

#include "HealthComp.h"
#include "Net/UnrealNetwork.h"
#include "Player/PCh.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Game/GM.h"
#include "Player/PC.h"
#include "Components/CapsuleComponent.h"
#include "Components/Player/WeaponManager.h"

UHealthComp::UHealthComp()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicated(true);
}

void UHealthComp::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwner() && GetOwnerRole() == ROLE_Authority)
	{
		/** init stats  */
		Health = MaxHealth;
		Armor = MaxArmor;
		Stamina = MaxStamina;

		/** cache owner player reference  */
		if(APCh* TestPCh = Cast<APCh>(GetOwner()))
		{
			PCh = TestPCh;

			/** bind delegates  */
			PCh->OnTakeAnyDamage.AddDynamic(this, &UHealthComp::OnOwnerTakeAnyDamage);
			PCh->OnTakePointDamage.AddDynamic(this, &UHealthComp::OnOwnerTakePointDamage);
		}
	}
}

void UHealthComp::OnOwnerTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	/** InstigatedBy can be null (level-placed turret in death match)  */
	APC* InstigatorPC = Cast<APC>(InstigatedBy);
	if (ShouldDamageTeammate(InstigatorPC))
	{
		ApplyDamage(Damage);
		if (!bDamageNotified)
		{
			ClientNotifyDamage(Damage, FVector::ZeroVector);
			bDamageNotified = false;
		} 
			
		/** if alive  */
		if (Health <= 0.f && !bDead)
		{
			Health = 0.f;
			bDead = true;
			OnRep_Dead();

			if (PCh)
			{
				/** ragdoll the corpse  */
				PCh->MulticastRagdoll();

				/** destroy all weapons  */
				if (PCh->GetWeaponManager())
				{
					PCh->GetWeaponManager()->DestroyAllWeapons();
				}
			}
			
			if(AGM* GM = Cast<AGM>(GetWorld()->GetAuthGameMode()))
			{
				GM->CharacterDead(PCh, InstigatorPC, DamageCauser);
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Teammate!"));
	}
}

void UHealthComp::OnOwnerTakePointDamage(AActor* DamagedActor, float Damage, class AController* InstigatedBy, FVector HitLocation, class UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection, const class UDamageType* DamageType, AActor* DamageCauser)
{
	APC* PC = Cast<APC>(InstigatedBy);
	if (ShouldDamageTeammate(PC))
	{
		ClientNotifyDamage(Damage, ShotFromDirection);
		bDamageNotified = true;
	}
}

void UHealthComp::ClientNotifyDamage_Implementation(float Damage, FVector DamageVector)
{
	/** broadcasts whenever we hit by other actor to notify damage direction */
	OnClientDamageReceived.Broadcast(Damage, DamageVector);
}

void UHealthComp::OnRep_Dead()
{
	/** broadcasts whenever owning character is dead */
	OnOwnerCharacterDead.Broadcast();
}

void UHealthComp::ApplyDamage(float DamageAmount)
{
	const float Damage = DamageAmount * (1 - (Armor / 200.f));
	Health -= Damage;
	Armor -= Damage * 2.f;

	/** avoid negatives  */
	Health = FMath::Clamp(Health, 0.f, MaxHealth);
	Armor = FMath::Clamp(Armor, 0.f, MaxArmor);
}

void UHealthComp::StartUsingStamina()
{
	/** start decrease stamina  */
	GetWorld()->GetTimerManager().SetTimer(DecreaseStaminaTimer, this, &UHealthComp::DecreaseStamina, StaminaDecreaseDelay, true);
	
	/** stop regen  */
	StopRegenerateStamina();
}

void UHealthComp::DecreaseStamina()
{
	if (PCh && PCh->GetCharacterMovement())
	{
		float CurrentVelocity = PCh->GetCharacterMovement()->Velocity.Size();

		/** if we are moving  */
		if (CurrentVelocity >= 50.f)
		{
			/** stop regen anyway  */
			if (StartRegenTimer.IsValid())
			{
				StopRegenerateStamina();
			}

			/** drain stamina  */
			Stamina -= StaminaDecreaseAmount * StaminaDecreaseDelay;
			Stamina = FMath::Clamp(Stamina, 0.f, MaxStamina);
			
			/** stop draining stamina  */
			if (Stamina <= 0.f)
			{
				OnStaminaEnded.Broadcast();
				StopUsingStamina();
			}
		} 
		else/** start regen stamina  */
		{
			if (!StartRegenTimer.IsValid() && !StaminaRegenTimer.IsValid())
			{
				GetWorld()->GetTimerManager().SetTimer(StartRegenTimer, this, &UHealthComp::DecreaseStamina, StaminaDecreaseDelay, true);
			}
		}
	}
}

void UHealthComp::StopUsingStamina()
{
	/** stop stamina drain  */
	GetWorld()->GetTimerManager().ClearTimer(DecreaseStaminaTimer);

	/** start regen stamina  */
	GetWorld()->GetTimerManager().SetTimer(StartRegenTimer, this, &UHealthComp::StartRegenerateStamina, StartRegenStaminaDelay, false);
}

void UHealthComp::StartRegenerateStamina()
{
	GetWorld()->GetTimerManager().ClearTimer(StartRegenTimer);
	GetWorld()->GetTimerManager().SetTimer(StaminaRegenTimer, this, &UHealthComp::RegenerateStamina, StaminaDecreaseDelay, true);
}

void UHealthComp::RegenerateStamina()
{
	Stamina += StaminaRegenAmount * StaminaDecreaseDelay;
	FMath::Clamp(Stamina, 0.f, MaxStamina);
	if (Stamina >= MaxStamina)
	{
		StopRegenerateStamina();
	}
}

void UHealthComp::StopRegenerateStamina()
{
	GetWorld()->GetTimerManager().ClearTimer(StartRegenTimer);
	GetWorld()->GetTimerManager().ClearTimer(StaminaRegenTimer);
}

bool UHealthComp::ShouldDamageTeammate(APC* InstigatorPC) const
{
	/** owner valid (InstigatorPC can be null (level-placed turret)) */
	if (PCh)
	{
		/** owner PC is valid  */
		if(APC* OnwerPC = Cast<APC>(PCh->GetController()))
		{
			/** game mode is valid  */
			if (AGM* GM = Cast<AGM>(GetWorld()->GetAuthGameMode()))
			{
				/** if we are alive - follow game mode rules  */
				return GM->ShouldDamagePlayer(OnwerPC, InstigatorPC) && !bDead;
			}
		}
	}
	
	/** damage otherwise  */
	return true;
}

void UHealthComp::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UHealthComp, bDead);

	/** only use for HUD  */
	DOREPLIFETIME_CONDITION(UHealthComp, Health, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UHealthComp, MaxHealth, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UHealthComp, Armor, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UHealthComp, MaxArmor, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UHealthComp, Stamina, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UHealthComp, MaxStamina, COND_OwnerOnly);
}
