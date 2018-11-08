// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/Weapon.h"
#include "ThrowableWeapon.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class NETWORKSHOOTER_API AThrowableWeapon : public AWeapon
{
	GENERATED_BODY()
	
	/** static mesh component  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* WeaponMesh;
	
public:

	AThrowableWeapon();

protected:

	/** called to start using weapon */
	virtual bool UseWeapon() override;

	/** called to stop using weapon */
	virtual void StopUseWeapon() override;

	/** projectile class template  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AProjectile> ThrowProjectileClass;
	
	UPROPERTY()
	FTimerHandle ThrowProjectileTimer;

	/** shows whether we must hold fire button to throw or not  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	uint32 bMustHoldToThrow : 1;

	/** hold delay between throw  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	float HoldTime = 1.f;

private:
	/** called to spawn projectile */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void ThrowProjectile();
};
