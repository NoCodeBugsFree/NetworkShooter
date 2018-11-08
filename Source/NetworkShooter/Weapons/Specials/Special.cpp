// Fill out your copyright notice in the Description page of Project Settings.

#include "Special.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "ConstructorHelpers.h"
#include "Materials/MaterialInterface.h"

ASpecial::ASpecial()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	/* scene component  */
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	/* sphere collision  */
	Trigger = CreateDefaultSubobject<USphereComponent>(TEXT("Trigger"));
	Trigger->SetupAttachment(RootComponent);
	Trigger->SetSphereRadius(100.f);

	Trigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Trigger->SetCollisionResponseToAllChannels(ECR_Ignore);
	Trigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	/* box collision  */
	BlockingCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BlockingCollision"));
	BlockingCollision->SetupAttachment(RootComponent);
	BlockingCollision->SetCollisionProfileName("BlockAll");

	/**  set material */
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MaterialTemplate(TEXT("/Game/BP/Weapons/Projectiles/M_Projectile"));
	if (MaterialTemplate.Succeeded())
	{
		ExplodedMaterial = MaterialTemplate.Object;
	}
	
	SetReplicates(true);
	InitialLifeSpan = 60.f;
}

void ASpecial::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	SetColor();
}

void ASpecial::BeginPlay()
{
	Super::BeginPlay();
	
	SetTeamColor();
}

void ASpecial::SetColor(FLinearColor Color /*= FLinearColor::Red*/)
{
	/** set color for each mesh with exploded material  */
	for (UActorComponent* Mesh : GetComponentsByClass(UMeshComponent::StaticClass()))
	{
		TArray<UMeshComponent*> Meshes;
		UMeshComponent* ThisMesh = Cast<UMeshComponent>(Mesh);
		if (ThisMesh)
		{
			Meshes.Push(ThisMesh);
		}

		for (UMeshComponent* Mesh : Meshes)
		{
			UMaterialInstanceDynamic* DMI = Mesh->CreateDynamicMaterialInstance(0, ExplodedMaterial);
			if (DMI)
			{
				DMI->SetVectorParameterValue("Color", Color);
			}
		}
	}
}

void ASpecial::SetTeamColor()
{
	if (GetInstigator())
	{
		if(ATeamPS* TeamPS = Cast<ATeamPS>(GetInstigator()->PlayerState))
		{
			if (TeamPS->GetTeam() == ETeam::TeamA)
			{
				SetColor(FLinearColor::Red);
			} 
			else
			{
				SetColor(FLinearColor::Blue);
			}
		}
	}
}


