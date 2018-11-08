// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/Weapon.h"
#include "FireWeapon.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnReloadStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnReloadFinished);

/**
 * 
 */
UCLASS(Abstract)
class NETWORKSHOOTER_API AFireWeapon : public AWeapon
{
	GENERATED_BODY()
	
protected:

	/* skeletal mesh component  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* GunMesh;
	
public:

	AFireWeapon();

	/** called to ask server to reload a weapon */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void ReloadWeapon();
	
	/** returns reload remaining time */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	float GetRemainingReloadTime() const;

	/** broadcasts whenever reload is started */
	UPROPERTY(BlueprintAssignable, Category = "EventDispatcher")
	FOnReloadStarted OnReloadStarted;

	/** broadcasts whenever reload is finished */
	UPROPERTY(BlueprintAssignable, Category = "EventDispatcher")
	FOnReloadFinished OnReloadFinished;
	
protected:

	/** called to  */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	virtual FVector GetMuzzleLocation() const;

	/** called to start using weapon */
	virtual bool UseWeapon() override;

	/** called to spawn projectile */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	virtual void SpawnProjectile();

	/** called to make a line trace and deal damage (non-projectile weapon)  */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	virtual void FireTrace();

	/** shows whether we can use weapon right now or not  */
	virtual bool CanUseWeapon() const override;

	/** called to player fire weapon fx */
	virtual void PlayWeaponFX_Implementation() override;

	/** max available ammo (CurrentAmmo supposed to be clip value)  */
	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	int32 TotalAmmo = 100;
	
	/** shows whether we are reloading now or not  */
	UPROPERTY(ReplicatedUsing = OnRep_Reloading, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	uint32 bReloading : 1;
		
	/* called whenever bReloading updated */
	UFUNCTION()
	virtual void OnRep_Reloading();

	/** reloading time for this weapon  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	float ReloadTime = 1.f;

	/** calculated amount of ammo to reload for particular reloading situation  */
	UPROPERTY(BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	int32 AmmoToReload = 0;
	
	UPROPERTY(BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	FTimerHandle ReloadTimer;

	/** projectile class  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class ABulletProjectile> ProjectileClass;
	
	/** weapon cone fire spread amount  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	float SpreadAngle = 3.f;
	
	/** max available fire distance for this weapon  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	float ShootDistance = 100000.f;

	/** recoil amount after each shot  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	float Recoil = 5.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	FRandomStream RandomStream;

	/* hole material */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class UMaterialInterface* HoleMaterial;
	
private:

	/* called to reload the weapon */
	UFUNCTION(Server, Reliable, WithValidation) // Unreliable
	void ServerReloadWeapon();
	bool ServerReloadWeapon_Validate(){ return true; }
	void ServerReloadWeapon_Implementation();
	
	/** return shot direction for particular shot  */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	FVector GetShootDirection() const;

	/** called whenever weapon has finished reloading  */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void ReloadFinished();

	/** shows whether we can reload right now or not  */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	bool CanReload() const;

	/** muzzle socket name  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	FName MuzzleSocketName = "Muzzle";

public:
	
	FORCEINLINE bool IsReloading() const { return bReloading; }
	FORCEINLINE float GetReloadTime() const { return ReloadTime; }
	FORCEINLINE class UMaterialInterface* GetHoleMaterial() const { return HoleMaterial; }
};
