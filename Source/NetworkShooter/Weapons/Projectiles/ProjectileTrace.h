// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/Projectiles/Projectile.h"
#include "ProjectileTrace.generated.h"

/**
 * 
 */
UCLASS()
class NETWORKSHOOTER_API AProjectileTrace : public AProjectile
{
	GENERATED_BODY()
	
protected:
	
	AProjectileTrace();

	virtual void BeginPlay() override;
	
};
