// Fill out your copyright notice in the Description page of Project Settings.

#include "WeaponSpawner.h"
#include "ConstructorHelpers.h"
#include "Components/SphereComponent.h"
#include "Weapons/Weapon.h"

AWeaponSpawner::AWeaponSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	/* scene component  */
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	/** mesh  */
	ConeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ConeMesh"));
	ConeMesh->SetupAttachment(RootComponent);

	ConeMesh->SetGenerateOverlapEvents(false);
	ConeMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ConeMesh->SetCollisionObjectType(ECC_WorldStatic);
	ConeMesh->SetRelativeScale3D(FVector(1.f, 1.f, 0.1f));

	/**  set the default static mesh */
	static ConstructorHelpers::FObjectFinder<UStaticMesh> Cone(TEXT("StaticMesh'/Engine/BasicShapes/Cone.Cone'"));
	if (Cone.Succeeded())
	{
		ConeMesh->SetStaticMesh(Cone.Object);
	}

	/* sphere collision  */
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComponent->SetupAttachment(RootComponent);
	SphereComponent->SetSphereRadius(100.f);

	SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	/**  set the weapon class TODO*/
	static ConstructorHelpers::FClassFinder<AWeapon> BPWeaponClass(TEXT("/Game/BP/Weapons/BP_SniperRifle"));
	if (BPWeaponClass.Class)
	{
		WeaponClass = BPWeaponClass.Class;
	}
	
	bNetLoadOnClient = false;
}

void AWeaponSpawner::BeginPlay()
{
	Super::BeginPlay();

	if (Role == ROLE_Authority)
	{
		SpawnWeapon();
	}
}

void AWeaponSpawner::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	
	if (Role == ROLE_Authority)
	{
		if(APCh* PCh = Cast<APCh>(OtherActor))
		{
			if (PCh->GetWeaponManager() && CurrentWeapon)
			{
				/** check and add weapon if we have no weapon of this type/class  */
				bool bHasWeaponByType = PCh->GetWeaponManager()->HasWeaponByType(CurrentWeapon->GetWeaponType());
				bool bHasWeaponByClass = PCh->GetWeaponManager()->FindWeaponByClass(WeaponClass);
				
				if (!bHasWeaponByClass || !bHasWeaponByType)
				{
					/** add the weapon  */
					PCh->GetWeaponManager()->AddWeapon(CurrentWeapon);

					/** disable weapon collision  */
					CurrentWeapon->SetActorEnableCollision(false);
					CurrentWeapon = nullptr;

					/** activate respawn timer  */
					FTimerHandle RespawnWeaponTimer;
					GetWorldTimerManager().SetTimer(RespawnWeaponTimer, this, &AWeaponSpawner::SpawnWeapon, RespawnDelay, false);
				}
			}
		}
	}
}

void AWeaponSpawner::SpawnWeapon()
{
	if (WeaponClass)
	{
		UWorld* const World = GetWorld();
		if (World)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			
			FVector SpawnLocation = GetActorLocation() + FVector(0.f, 0.f, 100.f);
			FRotator SpawnRotation = FRotator::ZeroRotator;
			
			AWeapon* Spawned = World->SpawnActor<AWeapon>(WeaponClass, SpawnLocation, SpawnRotation, SpawnParams);
			if (Spawned)
			{
				CurrentWeapon = Spawned;
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("WeaponClass == NULL"));
	}
}
