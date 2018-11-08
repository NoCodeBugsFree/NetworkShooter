// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/Specials/Special.h"
#include "SpecialTurret.generated.h"

/**
 * 
 */
UCLASS()
class NETWORKSHOOTER_API ASpecialTurret : public ASpecial
{
	GENERATED_BODY()

protected:

	/** static mesh component  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* BaseMesh;

	/** static mesh component  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* YawPlatform;
	
	/** static mesh component  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* PitchPlatform;
	
	/** static mesh component  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* Barrel01;

	/** static mesh component  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* Barrel02;

	/* arrow component  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UArrowComponent* Muzzle01;
	
	/* arrow component  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UArrowComponent* Muzzle02;

	/* Emitter Template */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* EmitterTemplate;

	/** Sound to play when  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class USoundBase* FireSound;

	/** attenuation settings for this weapon */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class USoundAttenuation* AttenuationSettings;
	
public:

	ASpecialTurret();

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;
	
protected:

	virtual void Tick(float DeltaSeconds) override;

private:

	/** called to check whether intruder is an enemy or not */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	bool IsEnemy(class APCh* Intruder) const;

	/** called to try find new target */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void FindNewTarget();

	/** called to start fire timer */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void StartFire();

	/** called to stop fire timer */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void StopFire();

	/** called to fire single shot */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void Fire();

	/** called to disalarm turret on current target, stops fire, try to find new target */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void Disalarm();

	/** called to set new target if it is an enemy */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void SetTarget(class APCh* Intruder);
	
	/* called whenever Target updated */
	UFUNCTION()
	virtual void OnRep_Target();

	/** target reference  */
	UPROPERTY(ReplicatedUsing = OnRep_Target, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class APCh* Target;

	/** delay between fire shoots  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	float ShootDelay = 1.f;
	
	UPROPERTY()
	FTimerHandle ShotTimer;

public:
	
	
};
