// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/Projectiles/Projectile.h"
#include "GrenadeProjectile.generated.h"

/**
 * 
 */
UCLASS()
class NETWORKSHOOTER_API AGrenadeProjectile : public AProjectile
{
	GENERATED_BODY()
	
	/* explosion FX */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* EmitterTemplate;
	
	/** sound to play when exploded  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class USoundBase* ExplodeSound;

	/** attenuation settings  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class USoundAttenuation* AttenuationSettings;

public:

	AGrenadeProjectile();

	virtual void BeginPlay() override;

	virtual void NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;
	
private:

	/*  radius of the minimum damage area, from Origin */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	float DamageOuterRadius = 1000.f;

	/* falloff exponent of damage from DamageInnerRadius to DamageOuterRadius */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	float DamageFalloff = 1.f;

	/* radius of the full damage area, from Origin */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	float DamageInnerRadius = 0.f;

	/* min damage to to apply */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	float MinimumDamage = 20.f;

	/** some impulse to add at begin play to initialize movement  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	float ImpulseStrength = 2000.f;

	/** delay before explosion  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	float TimeToExplode = 2.f;

	/** the radius of starting dealing damage by explosion  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	float ExplosionRadius = 10000.f;

	/** shows whether is explosion was happened or not  */
	UPROPERTY(ReplicatedUsing = OnRep_Exploded, EditDefaultsOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	uint32 bExploded : 1;

	/* called whenever bExploded updated */
	UFUNCTION()
	virtual void OnRep_Exploded();

	/** called whenever this grenade is exploded  */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void Explode();

};
