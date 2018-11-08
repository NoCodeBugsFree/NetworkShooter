// Fill out your copyright notice in the Description page of Project Settings.

#include "FollowSpectator.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

AFollowSpectator::AFollowSpectator()
{
	/** camera boom  */
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.f;
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->bInheritPitch = true;
	CameraBoom->bInheritRoll = false;
	CameraBoom->bInheritYaw = true;
	
	/* camera component  */
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(CameraBoom);
}

void AFollowSpectator::BeginPlay()
{
	Super::BeginPlay();
	
	FTimerDelegate IncreaseSpeedDelegate;
	IncreaseSpeedDelegate.BindLambda([&] { Speed = 20.f;  });
	
	FTimerHandle IncreaseSpeedTimer;
	GetWorldTimerManager().SetTimer(IncreaseSpeedTimer, IncreaseSpeedDelegate, 1.f, false);
}

void AFollowSpectator::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (FollowPawn)
	{
		SetActorLocation(FMath::VInterpTo(GetActorLocation(), FollowPawn->GetActorLocation(), DeltaSeconds, Speed));
	}
}
