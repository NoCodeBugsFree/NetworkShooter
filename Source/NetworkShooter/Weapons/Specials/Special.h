// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Special.generated.h"

UCLASS(Abstract)
class NETWORKSHOOTER_API ASpecial : public AActor
{
	GENERATED_BODY()
	
protected:
	/* scene component  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class USceneComponent* Root;
	
	/* sphere collision  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class USphereComponent* Trigger;
	
	/* box collision  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* BlockingCollision;

	/* material */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class UMaterialInterface* ExplodedMaterial;
			
public:	
	
	ASpecial();

	virtual void OnConstruction(const FTransform& Transform) override;

	/** called to set color for all meshes */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void SetColor(FLinearColor Color = FLinearColor::Red);

	/** called to set damage to cause by this weapon */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void SetDamage(float NewDamage) { Damage = NewDamage; }

protected:

	/** damage to cause  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	float Damage = 20.f;

	/** called to set color according team */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void SetTeamColor();
	
	virtual void BeginPlay() override;

public:
	FORCEINLINE class UBoxComponent* GetBlockingCollision() const { return BlockingCollision; }
	
};
