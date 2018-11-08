// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/InteractableInterface.h"
#include "Weapon.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDamagedActor, float, Damage, class AActor*, Victim);

/** weapon type  */
UENUM(BlueprintType)
enum class EWeaponType: uint8
{
	WT_Main		 UMETA(DisplayName = "Main"),
	WT_Secondary UMETA(DisplayName = "Secondary"),
	WT_Special	 UMETA(DisplayName = "Special"),
	WT_Grenade	 UMETA(DisplayName = "Grenade"),
	WT_Melee	 UMETA(DisplayName = "Melee"),
	WT_Turret	 UMETA(DisplayName = "Turret"),
	WT_None		 UMETA(DisplayName = "None")
};

UCLASS(Abstract)
class NETWORKSHOOTER_API AWeapon : public AActor, public IInteractableInterface
{
	GENERATED_BODY()
	
public:	
	
	AWeapon();

	/** called to ask server to start/stop using(primary ability) the weapon */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	virtual void ToggleUseWeapon(bool bUse);

	/** called to ask server to use(secondary ability) the weapon */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void ToggleSecondaryAction(bool bUse);

	/** called whenever this weapon is drop to nullify owner and instigator */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	virtual void OnWeaponDropped();

	/** called to set new weapon owner */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void SetWeaponOwner(class APCh* PCh) { WeaponOwner = PCh; }

	/** called whenever this weapon damage other actor to notify owner about */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void OnWeaponDamagedActor(float Damage, class AActor* Victim);

	//~ Begin IInteractableInterface Interface
	virtual bool Interact_Implementation(class APCh* Character);
	//~ End IInteractableInterface Interface
	
	/** broadcasted whenever this weapon damaged other actor  */
	UPROPERTY(BlueprintAssignable)
	FOnDamagedActor OnDamagedActor;
	
protected:

	/** socket name on character's mesh to attach this weapon  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	FName SocketName = "Weapon";

	/** type of this weapon  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	EWeaponType WeaponType = EWeaponType::WT_Main;

	/** damage to cause  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	float Damage = 20.f;

	/** current ammo amount  */
	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	int32 CurrentAmmo = 100;

	/** time between shots  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	float UseRate = 1.f;

	/** shows whether this weapon is able to autofire or not  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	uint32 bAutoFire : 1;

	/** shows whether we are firing right now (bAutoFire must be enable) or not  */
	UPROPERTY(BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	uint32 bFireActive : 1;
	
	/** player owner reference  */
	UPROPERTY(ReplicatedUsing = OnRep_WeaponOwner, Transient, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class APCh* WeaponOwner;
	
	/** shows whether this weapon is using second ability right now  or not  */
	UPROPERTY(ReplicatedUsing = OnRep_UsingSecondary, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	uint32 bUsingSecondary : 1;
	
	/** shows whether this weapon is using primary ability right now  or not  */
	UPROPERTY(ReplicatedUsing = OnRep_UsingWeapon, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	uint32 bUsingWeapon : 1;

	/** shows whether this weapon has secondary ability or not  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	uint32 bHasSecondaryAction : 1;

	/** fire rate timer  */
	UPROPERTY()
	FTimerHandle CooldownTimer;

	/** sound to play when weapon used */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class USoundBase* UseSound;

	/** attenuation settings for this weapon */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class USoundAttenuation* AttenuationSettings;
	
	// -----------------------------------------------------------------------------------
	
	/* called whenever WeaponOwner updated */
	UFUNCTION()
	virtual void OnRep_WeaponOwner();

	/* called whenever UsingSecondary updated */
	UFUNCTION()
	virtual void OnRep_UsingSecondary();

	/* called whenever UsingWeapon updated */
	UFUNCTION()
	virtual void OnRep_UsingWeapon();

	/** called to start using weapon */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	virtual bool UseWeapon();

	/** called to use secondary action if specified */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	virtual void UseSecondaryAction();
	
	/** called to stop use secondary action if specified */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	virtual void StopUseSecondaryAction();

	/** shows whether we can use weapon right now or not  */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	virtual bool CanUseWeapon() const;

	/** shows whether we can use weapon's second action right now or not  */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	virtual bool CanUseSecondaryAction() const;

	/** called to player fire weapon fx */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AAA")
	void PlayWeaponFX();
	virtual void PlayWeaponFX_Implementation();
	
	/** called to destroy self when owner is dead */
	UFUNCTION()
	virtual void OwnerDead();

	/** shows where this code running */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "AAA")
	bool IsOtherPlayer() const;

	/** called to stop using weapon */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	virtual void StopUseWeapon();

private:
	/** called to notify client of successfully hit other actor */
	UFUNCTION(Client, Reliable)
	void ClientWeaponDamageActor(float DamageToCause, class AActor* Victim);
	void ClientWeaponDamageActor_Implementation(float DamageToCause, class AActor* Victim);

	/* called to notify players about start/stop using weapon */
	UFUNCTION(NetMulticast, Reliable)
	void MulticastToggleWeaponUse(bool bUse);
	void MulticastToggleWeaponUse_Implementation(bool bUse);

	/* called to toggle a weapon's primary action */
	UFUNCTION(Server, Reliable, WithValidation) // Unreliable
	void ServerToggleUseWeapon(bool bUse);
	bool ServerToggleUseWeapon_Validate(bool bUse){ return true; }
	void ServerToggleUseWeapon_Implementation(bool bUse);

	/* called to toggle a weapon's secondary action */
	UFUNCTION(Server, Reliable, WithValidation) // Unreliable
	void ServerToggleSecondaryAction(bool bUse);
	bool ServerToggleSecondaryAction_Validate(bool bUse){ return true; }
	void ServerToggleSecondaryAction_Implementation(bool bUse);

	/** called to fire again if bAutoFire is enable and we are firing now */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void StopRateDelay();

	/** called to nullify weapon's owner and instigator */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void WeaponDropped();

	/** called to start cooldown timer */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void OwnerWeaponUsed();
	
public:	
	FORCEINLINE EWeaponType GetWeaponType() const { return WeaponType; }
	FORCEINLINE class APCh* GetWeaponOwner() const { return WeaponOwner; }
};
