// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectileTrace.h"
#include "Kismet/GameplayStatics.h"

AProjectileTrace::AProjectileTrace()
{
	/** no mesh  */
	ProjectileMesh->SetStaticMesh(nullptr);
}

void AProjectileTrace::BeginPlay()
{
	Super::BeginPlay();

	if(APCh* PCh = Cast<APCh>(GetOwner()))
	{		
		float Distance = 100000.f;
		FVector Direction = GetActorForwardVector();
		FVector Start = PCh->GetCameraComponent()->GetComponentLocation();
		FVector End = Start + (Direction * Distance);
		
		FHitResult Hit;
		
		FCollisionQueryParams CollisionQueryParams;
		CollisionQueryParams.AddIgnoredActor(this);
		CollisionQueryParams.bTraceComplex = false;
		
		FCollisionObjectQueryParams ObjectQueryParams;
		ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);
		ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
		ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
		ObjectQueryParams.AddObjectTypesToQuery(ECC_PhysicsBody);
		
		if (GetWorld()->LineTraceSingleByObjectType(Hit, Start, End, ObjectQueryParams, CollisionQueryParams))
		{
			FVector HitFromDirection = (Hit.TraceStart - Hit.TraceEnd).GetSafeNormal();
			
			UGameplayStatics::ApplyPointDamage(Hit.GetActor(), Damage, HitFromDirection, Hit, GetInstigatorController(), this, nullptr);
		}
	}
}
