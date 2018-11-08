// Fill out your copyright notice in the Description page of Project Settings.

#include "SpecialMine.h"
#include "ConstructorHelpers.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "ConstructorHelpers.h"
#include "Particles/ParticleSystem.h"
#include "Player/PCh.h"

ASpecialMine::ASpecialMine()
{
	/** mesh  */
	MineMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MineMesh"));
	MineMesh->SetupAttachment(RootComponent);

	/**  set the default static mesh */
	static ConstructorHelpers::FObjectFinder<UStaticMesh> Cylinder(TEXT("StaticMesh'/Engine/BasicShapes/Cylinder.Cylinder'"));
	if (Cylinder.Succeeded())
	{
		MineMesh->SetStaticMesh(Cylinder.Object);
	}
	MineMesh->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.1f));
	MineMesh->SetGenerateOverlapEvents(false);
	MineMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Trigger->SetSphereRadius(100.f);
	BlockingCollision->SetRelativeLocation(FVector(0.f, 0.f, 6.f));
	BlockingCollision->SetBoxExtent(FVector(32.f, 32.f, 5.f));

	/** set the explosion emitter  */
	static ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleSystem(TEXT("/Engine/Tutorial/SubEditors/TutorialAssets/TutorialParticleSystem"));
	if (ParticleSystem.Object)
	{
		EmitterTemplate = ParticleSystem.Object;
	}

	/** set the sound asset  */
	static ConstructorHelpers::FObjectFinder<USoundBase> FireSoundTemplate(TEXT("/Engine/EditorSounds/Notifications/CompileFailed_Cue"));
	if (FireSoundTemplate.Object)
	{
		ExplosionSound = FireSoundTemplate.Object;
	}

	/** set the sound asset  */
	static ConstructorHelpers::FObjectFinder<USoundAttenuation> SoundAttenuationTemplate(TEXT("/Game/Assets/Sounds/SA_Explosion"));
	if (SoundAttenuationTemplate.Object)
	{
		AttenuationSettings = SoundAttenuationTemplate.Object;
	}
}

void ASpecialMine::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (Role == ROLE_Authority && !bExploded)
	{
		/** victim pawn is valid  */
		APCh* VictimPawn = Cast<APCh>(OtherActor);
		if (VictimPawn)
		{
			/** victim player controller can be nullptr */
			APC* VictimPC = Cast<APC>(VictimPawn->GetController());
			
			/** instigator controller can be nullptr  */
			APC* InstigatorPC = Cast<APC>(GetInstigatorController());

			AGM* GM = Cast<AGM>(GetWorld()->GetAuthGameMode());
			if (GM && GM->ShouldDamagePlayer(VictimPC, InstigatorPC))
			{
				bExploded = true;
				OnRep_Exploded();
				
				TArray<AActor*> IgnoredActors;
				IgnoredActors.Add(this);
				UGameplayStatics::ApplyRadialDamage(this, Damage, GetActorLocation() + FVector(0.f, 0.f, 100.f),
					ExplodeRadius, nullptr, IgnoredActors, this, GetInstigatorController());

				SetLifeSpan(5.f);
			}
		}
	}
}

void ASpecialMine::OnRep_Exploded()
{
	if (GetWorld() && GetWorld()->GetNetMode() != NM_DedicatedServer)
	{
		SetActorHiddenInGame(true);
		SetActorEnableCollision(false);

		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), EmitterTemplate, GetActorLocation());
		UGameplayStatics::PlaySoundAtLocation(this, ExplosionSound, GetActorLocation(), 1.f, 1.f, 0.f, AttenuationSettings);
	}
}

void ASpecialMine::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ASpecialMine, bExploded);
}