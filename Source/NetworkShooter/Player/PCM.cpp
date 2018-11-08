// Fill out your copyright notice in the Description page of Project Settings.

#include "PCM.h"
#include "Player/PC.h"
#include "Player/PCh.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "ConstructorHelpers.h"
#include "Materials/MaterialInterface.h"
#include "Player/CameraShakeDamage.h"

APCM::APCM()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	/**  set parent material */
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MaterialTemplate(TEXT("/Game/Assets/Other/PostProcess/DamagePP/M_PP_DamageNotify"));
	if (MaterialTemplate.Succeeded())
	{
		Parent = MaterialTemplate.Object;
	}
}

void APCM::BeginPlay()
{
	Super::BeginPlay();
	
	if(APC* PC = Cast<APC>(GetOwner()))
	{
		PC->OnPossessed.AddDynamic(this, &APCM::OnCharacterPossessed);
	}

}

void APCM::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	DamageLerp = FMath::FInterpTo(DamageLerp, 0.f, DeltaSeconds, 5.f);

	if (DMI)
	{
		DMI->SetScalarParameterValue("Lerp", DamageLerp);
	}
	if (DamageLerp <= 0.f)/** no need to tick now  */
	{
		SetActorTickEnabled(false);
	}
}

void APCM::OnCharacterPossessed(class APCh* PCh)
{
	if (PCh)
	{
		Player = PCh;
		if (Player && Player->GetHealthComponent())
		{
			Player->GetHealthComponent()->OnClientDamageReceived.AddDynamic(this, &APCM::OnCharacterDamaged);

			DMI = UKismetMaterialLibrary::CreateDynamicMaterialInstance(this, Parent);
			if (DMI)
			{
				Player->GetCameraComponent()->AddOrUpdateBlendable(DMI);
			}
		}
	}
}

void APCM::OnCharacterDamaged(float Damage, FVector DamageVector)
{
	/** start learping  */
	SetActorTickEnabled(true);

	/** init the lerping for  */
	DamageLerp = 1.f;
	
	if (DMI)
	{
		FRotator OnwerRotation = Player->GetActorRotation();
		FVector UnrotatedVector = UKismetMathLibrary::ComposeRotators(OnwerRotation, FRotator(0.f, 90.f, 0.f)).UnrotateVector(DamageVector);
		FLinearColor Color = FLinearColor(UnrotatedVector.X, UnrotatedVector.Y, 0.f);
		DMI->SetVectorParameterValue("Direction", Color);
		PlayCameraShake(UCameraShakeDamage::StaticClass());
	}
}
