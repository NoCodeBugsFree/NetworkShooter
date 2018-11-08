// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponSpawner.generated.h"

UCLASS()
class NETWORKSHOOTER_API AWeaponSpawner : public AActor
{
	GENERATED_BODY()
	
	/* scene component  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class USceneComponent* Root;
	
	/** static mesh component  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* ConeMesh;
	
	/* sphere collision  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class USphereComponent* SphereComponent;
	
public:	
	
	AWeaponSpawner();

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

protected:
	
	virtual void BeginPlay() override;
	
private:

	/** called to spawn a weapon */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void SpawnWeapon();

	/** weapon class to spawn  */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AWeapon> WeaponClass;

	/** weapon respawn delay */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	float RespawnDelay = 5.f;

	/** current (spawned) weapon reference  */
	UPROPERTY(BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class AWeapon* CurrentWeapon;
	
};
