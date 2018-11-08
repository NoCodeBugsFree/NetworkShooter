// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Player/Animation/FPSAnimInstance.h"
#include "TPSAnimInst.generated.h"

/**
 * 
 */
UCLASS()
class NETWORKSHOOTER_API UTPSAnimInst : public UFPSAnimInstance
{
	GENERATED_BODY()
	
public:

	UTPSAnimInst();

protected:

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

private:
	
	/** owner's speed  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config", meta = (AllowPrivateAccess = "true"))
	float Speed;

	/** shows whether owner is crouched or not  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	uint32 bCrouched: 1;

	/** replicated aim pitch */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config", meta = (AllowPrivateAccess = "true"))
	float AimPitch;

	/** owner's movement angle  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config", meta = (AllowPrivateAccess = "true"))
	float MovementAnge;
	
};
