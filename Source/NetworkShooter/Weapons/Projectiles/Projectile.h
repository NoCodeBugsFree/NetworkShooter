// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

UCLASS(Abstract)
class NETWORKSHOOTER_API AProjectile : public AActor
{
	GENERATED_BODY()
	
protected:
	/* projectile sphere collision  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class USphereComponent* ProjectileCollision;

	/** static mesh component  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* ProjectileMesh;
	
	/* projectile movement component  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UProjectileMovementComponent* ProjectileMovementComponent;
	
public:	
	
	AProjectile();

	/** called to set current weapon owner */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void SetWeapon(class AWeapon* NewWeapon) { Weapon = NewWeapon; }

	/** called to set damage  */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void SetDamage(float NewDamage) { Damage = NewDamage; }

protected:
	/** weapon owner reference   */
	UPROPERTY(BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class AWeapon* Weapon;

	/** current damage to cause  */
	UPROPERTY(BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	float Damage = 1;
	
	virtual void BeginPlay() override;

	virtual void NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;
	
public:
	FORCEINLINE class USphereComponent* GetProjectileCollision() const { return ProjectileCollision; }
	FORCEINLINE class UProjectileMovementComponent* GetProjectileMovementComponent() const { return ProjectileMovementComponent; }
	FORCEINLINE class UStaticMeshComponent* GetProjectileMesh() const { return ProjectileMesh; }
};
