// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/Weapon.h"
#include "MeleeWeapon.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class NETWORKSHOOTER_API AMeleeWeapon : public AWeapon
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* WeaponMesh;
	
public:
	
	AMeleeWeapon();
	
protected:

	/** called to start using weapon */
	virtual bool UseWeapon() override;

	/** shows whether we can use weapon right now or not, only cooldown logic  */
	virtual bool CanUseWeapon() const override;

private:

	/** called to check whether we hit a character or not  */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	bool GetOverlapInfo(FHitResult& Hit);

	/** weapon attack radius  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	float Radius = 100.f;

};
