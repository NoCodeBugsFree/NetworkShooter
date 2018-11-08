// Fill out your copyright notice in the Description page of Project Settings.

#include "GameSpectator.h"
#include "ConstructorHelpers.h"

AGameSpectator::AGameSpectator()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	/* scene component  */
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	/** mesh  */
	CameraMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CameraMesh"));
	CameraMesh->SetupAttachment(RootComponent);
	CameraMesh->SetGenerateOverlapEvents(false);
	CameraMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	/**  set the default static mesh */
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CameraMeshTemplate(TEXT("/Engine/EditorMeshes/Camera/SM_CineCam"));
	if (CameraMeshTemplate.Succeeded())
	{
		CameraMesh->SetStaticMesh(CameraMeshTemplate.Object);
	}
}

void AGameSpectator::BeginPlay()
{
	Super::BeginPlay();
	
}

void AGameSpectator::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("LookUp", this, &AGameSpectator::LookUp);
	PlayerInputComponent->BindAxis("Turn", this, &AGameSpectator::Turn);
}

void AGameSpectator::LookUp(float Value)
{
	AddControllerPitchInput(Value);
}

void AGameSpectator::Turn(float Value)
{
	AddControllerYawInput(Value);
}
