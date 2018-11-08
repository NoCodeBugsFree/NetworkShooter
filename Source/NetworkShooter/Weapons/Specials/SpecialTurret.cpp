// Fill out your copyright notice in the Description page of Project Settings.

#include "SpecialTurret.h"
#include "Net/UnrealNetwork.h"
#include "ConstructorHelpers.h"
#include "Components/ArrowComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "DrawDebugHelpers.h"

ASpecialTurret::ASpecialTurret()
{
	PrimaryActorTick.bCanEverTick = true;

	Trigger->SetSphereRadius(500.f);
	BlockingCollision->SetBoxExtent(FVector(50.f, 50.f, 50.f));
	BlockingCollision->SetRelativeScale3D(FVector(0.75f, 1.f, 0.75f));
	BlockingCollision->SetRelativeLocation(FVector(0.f, 0.f, 57.f));

	/** BaseMesh  */
	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
	BaseMesh->SetupAttachment(RootComponent);
	BaseMesh->SetRelativeLocation(FVector(0.f, 0.f, 10.f));
	BaseMesh->SetRelativeScale3D(FVector(1.f, 1.f, 0.5f));
	BaseMesh->SetGenerateOverlapEvents(false);
	BaseMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	/** YawPlatform  */
	YawPlatform = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("YawPlatform"));
	YawPlatform->SetupAttachment(RootComponent);
	YawPlatform->SetRelativeLocation(FVector(0.f, 0.f, 70.f));
	YawPlatform->SetRelativeScale3D(FVector(0.25f, 0.5f, 1.f));
	YawPlatform->SetGenerateOverlapEvents(false);
	YawPlatform->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	/** PitchPlatform  */
	PitchPlatform = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PitchPlatform"));
	PitchPlatform->SetupAttachment(YawPlatform);
	PitchPlatform->SetRelativeLocation(FVector(0.f, 0.f, 20.f));
	PitchPlatform->SetRelativeScale3D(FVector(3.f, 0.5f, 0.25f));
	PitchPlatform->SetGenerateOverlapEvents(false);
	PitchPlatform->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	/** Barrel01  */
	Barrel01 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Barrel01"));
	Barrel01->SetupAttachment(PitchPlatform);

	/** Barrel02  */
	Barrel02 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Barrel02"));
	Barrel02->SetupAttachment(PitchPlatform);

	/* arrow component  */
	Muzzle01 = CreateDefaultSubobject<UArrowComponent>(TEXT("Muzzle01"));
	Muzzle01->SetupAttachment(Barrel01);
	Muzzle01->ArrowColor = FColor::Orange;

	/* arrow component  */
	Muzzle02 = CreateDefaultSubobject<UArrowComponent>(TEXT("Muzzle02"));
	Muzzle02->SetupAttachment(Barrel02);
	Muzzle02->ArrowColor = FColor::Orange;
	
	/**  set the default static mesh */
	static ConstructorHelpers::FObjectFinder<UStaticMesh> Cylinder(TEXT("StaticMesh'/Engine/BasicShapes/Cylinder.Cylinder'"));
	if (Cylinder.Succeeded())
	{
		BaseMesh->SetStaticMesh(Cylinder.Object);
		Barrel01->SetStaticMesh(Cylinder.Object);
		Barrel02->SetStaticMesh(Cylinder.Object);
	}

	/**  set the default static mesh */
	static ConstructorHelpers::FObjectFinder<UStaticMesh> Cube(TEXT("StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
	if (Cube.Succeeded())
	{
		YawPlatform->SetStaticMesh(Cube.Object);
		PitchPlatform->SetStaticMesh(Cube.Object);
	}

	/** set the explosion emitter  */
	static ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleSystem(TEXT("/Game/FX/P_MuzzleFlash"));
	if (ParticleSystem.Object)
	{
		EmitterTemplate = ParticleSystem.Object;
	}

	/** set the sound asset  */
	static ConstructorHelpers::FObjectFinder<USoundBase> FireSoundTemplate(TEXT("/Game/Assets/Sounds/128295__xenonn__layered-gunshot-1"));
	if (FireSoundTemplate.Object)
	{
		FireSound = FireSoundTemplate.Object;
	}

	/** set the sound asset  */
	static ConstructorHelpers::FObjectFinder<USoundAttenuation> SoundAttenuationTemplate(TEXT("/Game/Assets/Sounds/SA_Shot"));
	if (SoundAttenuationTemplate.Object)
	{
		AttenuationSettings = SoundAttenuationTemplate.Object;
	}
}

void ASpecialTurret::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (Target)
	{
		FVector TargetLocation = Target->GetActorLocation();
		FVector StartLocation = PitchPlatform->GetComponentLocation();
		FRotator FindLookRotator = FRotationMatrix::MakeFromX(TargetLocation - StartLocation).Rotator();

		YawPlatform->SetWorldRotation(FRotator(0.f, FindLookRotator.Yaw - 90.f, 0.f));
		PitchPlatform->SetRelativeRotation(FRotator(0.f, 0.f, FindLookRotator.Pitch * -1.f));
	}
}

bool ASpecialTurret::IsEnemy(class APCh* Intruder) const
{
	/** intruder is alive  */
	if (Intruder && Intruder->GetHealthComponent() && !Intruder->GetHealthComponent()->IsDead())
	{
		if (Instigator)
		{
			if (Instigator == Intruder) /** ignore self  */
			{
				return false;
			} 
			else /** check for teammate  */
			{
				if(AGM* GM = Cast<AGM>(GetWorld()->GetAuthGameMode()))
				{
					return GM->IsAnotherTeam(Intruder->GetController(), GetInstigatorController());
				}
			}
		} 
		else /** initially placed turrets on level have null instigators  */
		{
			return true;
		}
	}

	return true;
}

void ASpecialTurret::FindNewTarget()
{
	TArray<AActor*> CollectedActors;
	Trigger->GetOverlappingActors(CollectedActors);
	
	/** find enemy  */
	for (AActor* TestActor : CollectedActors)
	{
		if(APCh* PCh = Cast<APCh>(TestActor))
		{
			/** start fire  */
			if (IsEnemy(PCh))
			{
				Target = PCh;
				OnRep_Target();
				StartFire();
				break;
			}
		}
	}
}

void ASpecialTurret::StartFire()
{
	GetWorldTimerManager().SetTimer(ShotTimer, this, &ASpecialTurret::Fire, ShootDelay, true);
}

void ASpecialTurret::StopFire()
{
	GetWorldTimerManager().ClearTimer(ShotTimer);
}

void ASpecialTurret::Fire()
{
	/** target is valid and alive  */
	if (Target && !Target->GetHealthComponent()->IsDead())
	{
		if (Role == ROLE_Authority) /** server - damage the target  */
		{
			FCollisionQueryParams CollisionQueryParams;
			CollisionQueryParams.AddIgnoredActor(this);
			CollisionQueryParams.bTraceComplex = false;
			
			FCollisionObjectQueryParams ObjectQueryParams;
			ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);
			ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
			ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
			
			FHitResult Hit;
			if (GetWorld()->LineTraceSingleByObjectType(Hit, Muzzle01->GetComponentLocation(), Target->GetActorLocation(), ObjectQueryParams, CollisionQueryParams))
			{
				DrawDebugLine(GetWorld(), Muzzle01->GetComponentLocation(), Target->GetActorLocation(), FColor::Green, true, 10.f, (uint8)0, 10.f);

				FVector HitFromDirection = (Hit.TraceStart - Hit.TraceEnd).GetSafeNormal();
				UGameplayStatics::ApplyPointDamage(Hit.GetActor(), Damage, HitFromDirection, Hit, GetInstigatorController(), this, nullptr);
			}
			
		}
		else /** client - fire FX  */
		{
			UGameplayStatics::SpawnEmitterAttached(EmitterTemplate, Muzzle01, NAME_None, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, true);
			UGameplayStatics::SpawnEmitterAttached(EmitterTemplate, Muzzle02, NAME_None, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, true);
			UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation(), 1.f, 1.f, 0.f, AttenuationSettings);
		}
	} 
	else
	{
		Disalarm();
	}
}

void ASpecialTurret::Disalarm()
{
	Target = nullptr;
	OnRep_Target();
	StopFire();
	FindNewTarget();
}

void ASpecialTurret::SetTarget(class APCh* Intruder)
{
	if (Intruder)
	{
		if (IsEnemy(Intruder))
		{
			Target = Intruder;
			OnRep_Target();
			StartFire();
		}
	}
}

void ASpecialTurret::OnRep_Target()
{
	if (Target)
	{
		StartFire();
	} 
	else
	{
		StopFire();
	}
}

void ASpecialTurret::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (Role == ROLE_Authority)
	{
		if(APCh* PCh = Cast<APCh>(OtherActor))
		{
			if (!Target)
			{
				SetTarget(PCh);
			}
		}
	}
}

void ASpecialTurret::NotifyActorEndOverlap(AActor* OtherActor)
{
	Super::NotifyActorEndOverlap(OtherActor);

	if (Role == ROLE_Authority)
	{
		if (OtherActor == Target)
		{
			Disalarm();
		}
	}
}

void ASpecialTurret::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASpecialTurret, Target);
}