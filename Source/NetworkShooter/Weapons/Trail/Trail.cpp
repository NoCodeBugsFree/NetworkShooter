// Fill out your copyright notice in the Description page of Project Settings.

#include "Trail.h"
#include "Components/SplineMeshComponent.h"

ATrail::ATrail()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	/* scene component  */
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	/** spline mesh component  */
	SplineMeshComponent = CreateDefaultSubobject<USplineMeshComponent>(TEXT("SplineMeshComponent"));
	SplineMeshComponent->SetupAttachment(Root);
	SplineMeshComponent->SetGenerateOverlapEvents(false);
	SplineMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ATrail::BeginPlay()
{
	Super::BeginPlay();
	
	FVector End = Hit.bBlockingHit ? Hit.Location : Hit.TraceEnd;

	FVector StartPos = GetActorLocation();
	FVector StartTangent = (End - GetActorLocation()).GetSafeNormal();;
	FVector EndPos = End;
	FVector EndTangent = StartTangent;
	SplineMeshComponent->SetStartAndEnd(StartPos, StartTangent, EndPos, EndTangent);

	SetLifeSpan(0.5f);
}


