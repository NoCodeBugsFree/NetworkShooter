// Fill out your copyright notice in the Description page of Project Settings.

#include "SniperRifle.h"
#include "ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"

ASniperRifle::ASniperRifle()
{
	bHasSecondaryAction = true;
	TotalAmmo = 36;
	ReloadTime = 5.f;
	ProjectileClass = nullptr;
	SpreadAngle = 3.f;
	ShootDistance = 200000.f;
	Recoil = 10.f;
	Damage = 200.f;
	CurrentAmmo = 6;

	/**  set the widget asset */
	static ConstructorHelpers::FClassFinder<UUserWidget> SniperScopeWidgetBPClass(TEXT("/Game/BP/UI/Sniper/WBP_Sniper"));
	if (SniperScopeWidgetBPClass.Class)
	{
		SniperScopeWidgetClass = SniperScopeWidgetBPClass.Class;
	}
}

void ASniperRifle::UseSecondaryAction()
{
	Super::UseSecondaryAction();
	
	if (bUsingSecondary)
	{
		SpreadAngle = 0.f;
		if (GetInstigatorController() && GetInstigatorController()->IsLocalPlayerController() && WeaponOwner)
		{
			WeaponOwner->GetCameraComponent()->SetFieldOfView(10.f);

			if (CurrentWidget)
			{
				CurrentWidget->AddToViewport(-1);
			} 
			else
			{
				if (SniperScopeWidgetClass)
				{
					CurrentWidget = CreateWidget<UUserWidget>(GetWorld(), SniperScopeWidgetClass);
					if (CurrentWidget)
					{
						CurrentWidget->AddToViewport(-1);
					}
				}
			}
		}
	}
}

void ASniperRifle::StopUseSecondaryAction()
{
	Super::StopUseSecondaryAction();
	
	SpreadAngle = GetDefault<ASniperRifle>(GetClass())->SpreadAngle;

	if (GetInstigatorController() && GetInstigatorController()->IsLocalPlayerController() && WeaponOwner)
	{
		WeaponOwner->GetCameraComponent()->SetFieldOfView(90.f);

		if (CurrentWidget)
		{
			CurrentWidget->RemoveFromParent();
		}
	}
}
