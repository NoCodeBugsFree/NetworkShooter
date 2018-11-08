// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon.h"
#include "Kismet/GameplayStatics.h"
#include "ConstructorHelpers.h"
#include "Net/UnrealNetwork.h"

AWeapon::AWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	/** set the sound asset  */
	static ConstructorHelpers::FObjectFinder<USoundBase> FireSoundTemplate(TEXT("/Game/Assets/Sounds/128294__xenonn__layered-gunshot-2"));
	if (FireSoundTemplate.Object)
	{
		UseSound = FireSoundTemplate.Object;
	}

	/** set the sound asset  */
	static ConstructorHelpers::FObjectFinder<USoundAttenuation> SoundAttenuationTemplate(TEXT("/Game/Assets/Sounds/SA_Shot"));
	if (SoundAttenuationTemplate.Object)
	{
		AttenuationSettings = SoundAttenuationTemplate.Object;
	}
	
	SetReplicates(true);
	SetReplicateMovement(true);
	bNetUseOwnerRelevancy = true;
}

bool AWeapon::UseWeapon()
{
	if (CanUseWeapon())
	{
		if (!IsOtherPlayer())
		{
			CurrentAmmo--;
			bUsingWeapon = true;
			OnRep_UsingWeapon();
		}

		GetWorldTimerManager().SetTimer(CooldownTimer, this, &AWeapon::StopRateDelay, UseRate, false);
		if (bAutoFire)
		{
			bFireActive = true;
		}
		return true;
	}
	else
	{
		bFireActive = false;
		bUsingWeapon = false;
	}
	return false;
}

void AWeapon::ToggleUseWeapon(bool bUse)
{
	if (bUse)
	{
		if (CanUseWeapon())
		{
			/** server  */
			ServerToggleUseWeapon(true);

			/** start local using  */
			UseWeapon();
		}
	} 
	else
	{
		if (bUsingWeapon)
		{
			/** server  */
			ServerToggleUseWeapon(false);

			/** start local using  */
			StopUseWeapon();
		}
	}
}

void AWeapon::ServerToggleUseWeapon_Implementation(bool bUse)
{
	if (bUse)
	{
		if (UseWeapon())
		{
			MulticastToggleWeaponUse(true);
		}
	}
	else
	{
		if (bUsingWeapon)
		{
			StopUseWeapon();
			MulticastToggleWeaponUse(false);
		}
	}
}

void AWeapon::MulticastToggleWeaponUse_Implementation(bool bUse)
{
	if (IsOtherPlayer())
	{
		if (bUse)
		{
			UseWeapon();
		}
		else
		{
			StopUseWeapon();
		}
	}
}

void AWeapon::ToggleSecondaryAction(bool bUse)
{
	if (bHasSecondaryAction)
	{
		if (bUse)
		{
			if (CanUseSecondaryAction())
			{
				/** server   */
				ServerToggleSecondaryAction(true);

				/** locally  */
				UseSecondaryAction();
				return;
			}
		}
		else /** bUse = false  */
		{
			/** server   */
			ServerToggleSecondaryAction(false);

			/** locally  */
			StopUseSecondaryAction();
		}
	}
}

bool AWeapon::Interact_Implementation(class APCh* Character)
{
	if (Character && Character->GetWeaponManager())
	{
		Character->GetWeaponManager()->AddWeapon(this);
	}
	return true;
}

bool AWeapon::CanUseWeapon() const
{
	return (CurrentAmmo > 0 || IsOtherPlayer()) && !CooldownTimer.IsValid();
}

void AWeapon::StopRateDelay()
{
	/** clear cooldown timer  */
	GetWorldTimerManager().ClearTimer(CooldownTimer);

	/** fire again if needed  */
	if (bAutoFire && bFireActive)
	{
		UseWeapon();
	}
}

void AWeapon::StopUseWeapon()
{
	/** we are stop the fire  */
	bFireActive = false;

	/** notify about  */
	if (!IsOtherPlayer())
	{
		bUsingWeapon = false;
		OnRep_UsingWeapon();
	}
}

void AWeapon::WeaponDropped()
{
	WeaponOwner = nullptr;
	Instigator = nullptr;
	SetOwner(nullptr);
}

void AWeapon::UseSecondaryAction()
{
	if (CanUseSecondaryAction())
	{
		bUsingSecondary = true;
		OnRep_UsingSecondary();
	}
	else
	{
		bUsingSecondary = false;
		OnRep_UsingSecondary();
	}
}

bool AWeapon::CanUseSecondaryAction() const
{
	/** override according required behavior  */
	return true;
}

void AWeapon::StopUseSecondaryAction()
{
	bUsingSecondary = false;
	OnRep_UsingSecondary();
}

void AWeapon::OwnerWeaponUsed()
{
	/** fire cooldown timer  */
	if (CanUseWeapon())
	{
		GetWorldTimerManager().SetTimer(CooldownTimer, this, &AWeapon::StopRateDelay, UseRate, false);
	}
}

void AWeapon::PlayWeaponFX_Implementation()
{
	UGameplayStatics::PlaySoundAtLocation(this, UseSound, GetActorLocation(), 1.f, 1.f, 0.f, AttenuationSettings);
}

bool AWeapon::IsOtherPlayer() const
{
	return GetInstigatorController() ? false : true;
}

void AWeapon::OwnerDead()
{
	Destroy();
}

void AWeapon::OnWeaponDropped()
{
	WeaponOwner = nullptr;
	Instigator = nullptr;
	SetOwner(nullptr);
}

void AWeapon::OnWeaponDamagedActor(float Damage, class AActor* Victim)
{
	ClientWeaponDamageActor(Damage, Victim);
	OnDamagedActor.Broadcast(Damage, Victim);
}

void AWeapon::OnRep_WeaponOwner()
{
	if (WeaponOwner) /** attach to weapon owner  */
	{
		Instigator = WeaponOwner;
		SetOwner(Instigator->GetController());
		SetActorEnableCollision(false);

		if (Role == ROLE_Authority) /** server  */
		{
			AttachToActor(WeaponOwner, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			WeaponOwner->OnCharacterDead.AddDynamic(this, &AWeapon::OwnerDead);
		}
		else /** client  */
		{
			if (WeaponOwner->GetWeaponManager() && WeaponOwner->GetWeaponManager()->GetCurrentWeapon())
			{
				bool bSameWeapon = WeaponOwner->GetWeaponManager()->GetCurrentWeapon() == this;
				if (!bSameWeapon)
				{
					SetActorHiddenInGame(true);
				}
				else
				{
					AttachToComponent(WeaponOwner->GetVisibleMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketName);
				}
			}
		}
	} 
	else /** detach and destroy  */
	{
		DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
		SetActorHiddenInGame(true);
		OnWeaponDropped();
	}
}

void AWeapon::OnRep_UsingSecondary()
{

}

void AWeapon::OnRep_UsingWeapon()
{

}

void AWeapon::ClientWeaponDamageActor_Implementation(float DamageToCause, class AActor* Victim)
{
	OnDamagedActor.Broadcast(Damage, Victim);
}

void AWeapon::ServerToggleSecondaryAction_Implementation(bool bUse)
{
	if (Role == ROLE_Authority)
	{
		if (bUse)
		{
			UseSecondaryAction();
		}
		else
		{
			StopUseSecondaryAction();
		}
	}
}

void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	/** only use for HUD  */
	DOREPLIFETIME_CONDITION(AWeapon, CurrentAmmo, COND_OwnerOnly);

	DOREPLIFETIME(AWeapon, WeaponOwner);
	DOREPLIFETIME(AWeapon, bUsingWeapon); /** TODO skip owner  */
	DOREPLIFETIME(AWeapon, bUsingSecondary);
}
