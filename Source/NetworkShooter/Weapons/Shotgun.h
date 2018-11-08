// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/FireWeapon.h"
#include "Shotgun.generated.h"

/**
 * 
 */
UCLASS()
class NETWORKSHOOTER_API AShotgun : public AFireWeapon
{
	GENERATED_BODY()
	
public:

	AShotgun();

protected:
	
	/** called to spawn projectile  */
	virtual void SpawnProjectile() override;

	/** calls ShotgunSpreadTraces times   */
	virtual void FireTrace() override;

private:

	/** bullet amount for shotgun shot  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	int32 ShotgunSpreadTraces = 15;
};
