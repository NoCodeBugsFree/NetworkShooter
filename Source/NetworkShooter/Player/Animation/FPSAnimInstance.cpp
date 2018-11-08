// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSAnimInstance.h"
#include "Player/PCh.h"
#include "Weapons/Weapon.h"
#include "Weapons/FireWeapon.h"
#include "ConstructorHelpers.h"
#include "Animation/AnimSequenceBase.h"
#include "Components/Player/WeaponManager.h"

UFPSAnimInstance::UFPSAnimInstance()
{
	/**  set reload animation asset */
	static ConstructorHelpers::FObjectFinder<UAnimSequenceBase> AssetTemplate(TEXT("/Game/Assets/Characters/FirstPerson/Animations/ReloadArms"));
	if (AssetTemplate.Succeeded())
	{
		ReloadAnimationAsset = AssetTemplate.Object;
	}
}

void UFPSAnimInstance::NativeInitializeAnimation()
{
	if(APCh* TestPCh = Cast<APCh>(TryGetPawnOwner()))
	{
		PCh = TestPCh;
	}
}

void UFPSAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	if (PCh)
	{
		bInAir = PCh->GetMovementComponent()->IsFalling();
		bMoving = PCh->GetVelocity().Size() > 0.f;
		CurrentWeapon = PCh->GetWeaponManager()->GetCurrentWeapon();
		WeaponType = CurrentWeapon ? CurrentWeapon->GetWeaponType() : EWeaponType::WT_None;
	}
}

void UFPSAnimInstance::PlayReloadAnimation()
{
	if(AFireWeapon* FireWeapon = Cast<AFireWeapon>(CurrentWeapon))
	{
		float ReloadTime = FireWeapon->GetReloadTime();
		PlaySlotAnimationAsDynamicMontage(ReloadAnimationAsset, ArmsSlotName, 0.25f, 0.25f, ReloadAnimationDuration / ReloadTime);
	}
}
