// Fill out your copyright notice in the Description page of Project Settings.

#include "TPSAnimInst.h"
#include "ConstructorHelpers.h"
#include "Animation/AnimSequenceBase.h"

UTPSAnimInst::UTPSAnimInst()
{
	ReloadAnimationDuration = 2.166f;

	/**  set reload animation asset */
	static ConstructorHelpers::FObjectFinder<UAnimSequenceBase> AssetTemplate(TEXT("/Game/FirstPerson/Animations/Reload_Rifle_Ironsights"));
	if (AssetTemplate.Succeeded())
	{
		ReloadAnimationAsset = AssetTemplate.Object;
	}
}

void UTPSAnimInst::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (PCh)
	{
		Speed = PCh->GetVelocity().Size();
		bCrouched = PCh->bIsCrouched;
		AimPitch = PCh->GetAimPitch();
		FRotator RotX = FRotationMatrix::MakeFromX(PCh->GetVelocity()).Rotator();
		FRotator Result = RotX - PCh->GetActorRotation();
		Result.Normalize();
		MovementAnge = Result.Yaw;
	}
}
