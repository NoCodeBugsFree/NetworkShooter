// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "FPSAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class NETWORKSHOOTER_API UFPSAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:

	UFPSAnimInstance();
	
	/** called to play specified reload animation */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	virtual void PlayReloadAnimation();

protected:
	
	virtual void NativeInitializeAnimation() override;
	
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
	/** owner player reference  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class APCh* PCh;

	/** shows whether owner is moving or not  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config", meta = (AllowPrivateAccess = "true"))
	uint32 bMoving : 1;

	/** shows whether owner is falling or not  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config", meta = (AllowPrivateAccess = "true"))
	uint32 bInAir : 1;

	/** owner's current weapon (can be null)  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class AWeapon* CurrentWeapon;

	/** owner's weapon type  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config", meta = (AllowPrivateAccess = "true"))
	EWeaponType WeaponType = EWeaponType::WT_Main;

	/** shows whether is  or not  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config", meta = (AllowPrivateAccess = "true"))
	UAnimSequenceBase* ReloadAnimationAsset;

	/** slot name where we play reload animation  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config", meta = (AllowPrivateAccess = "true"))
	FName ArmsSlotName = "Arms";

	/** the actual length of reloading animation duration */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config", meta = (AllowPrivateAccess = "true"))
	float ReloadAnimationDuration = 2.083f;
};
