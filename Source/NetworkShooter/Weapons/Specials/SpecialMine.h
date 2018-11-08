// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/Specials/Special.h"
#include "SpecialMine.generated.h"

/**
 * 
 */
UCLASS()
class NETWORKSHOOTER_API ASpecialMine : public ASpecial
{
	GENERATED_BODY()
	
	/** mine mesh  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* MineMesh;

	/* emitter template */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* EmitterTemplate;

	/** Sound to play when  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class USoundBase* ExplosionSound;

	/** attenuation settings for this weapon */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class USoundAttenuation* AttenuationSettings;
	
public:

	ASpecialMine();

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	
private:
	
	/** shows whether mine is exploded or not  */
	UPROPERTY(ReplicatedUsing = OnRep_Exploded, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	uint32 bExploded : 1;

	/* explosion radius */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	float ExplodeRadius = 500.f;
	
	/* called whenever bExploded updated */
	UFUNCTION()
	virtual void OnRep_Exploded();
		
};
