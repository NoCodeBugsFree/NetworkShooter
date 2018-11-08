// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "PCM.generated.h"

/**
 * 
 */
UCLASS()
class NETWORKSHOOTER_API APCM : public APlayerCameraManager
{
	GENERATED_BODY()
	
	/* DMI reference */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class UMaterialInstanceDynamic* DMI;

	/* parent material for DMI */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class UMaterialInterface* Parent;
	
public:

	APCM();
	
protected:

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

private:

	/** called whenever owner player was possessed  */
	UFUNCTION()
	void OnCharacterPossessed(class APCh* PCh);

	/** called whenever owner player was damaged  */
	UFUNCTION()
	void OnCharacterDamaged(float Damage, FVector DamageVector);

	/** owner player reference  */
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class APCh* Player;

	/** the value that we lerping for when onwer player was damaged  */
	UPROPERTY(BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	float DamageLerp = 0.f;
};
