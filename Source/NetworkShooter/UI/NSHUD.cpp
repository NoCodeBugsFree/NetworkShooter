// Fill out your copyright notice in the Description page of Project Settings.

#include "NSHUD.h"
#include "Game/GS.h"
#include "Player/PCh.h"

void ANSHUD::BeginPlay()
{
	Super::BeginPlay();

	CreateChat();

	TogglePreRound(true);

	if(AGS* TestGS = Cast<AGS>(GetWorld()->GetGameState()))
	{
		GS = TestGS;
		GS->OnRoundStarted.AddDynamic(this, &ANSHUD::RoundStarted);
		GS->OnRoundEnded.AddDynamic(this, &ANSHUD::RoundEnded);
	}
}

void ANSHUD::RoundStarted()
{
	TogglePreRound(false);
	ToggleHUD(true);
	
	if(APS* PS = Cast<APS>(GetOwningPlayerController()->PlayerState))
	{
		PS->OnPlayerAlive.AddDynamic(this, &ANSHUD::PlayerAlive);
		PS->OnPlayerDead.AddDynamic(this, &ANSHUD::PlayerDead);
	}
}

void ANSHUD::RoundEnded(FString Reason)
{
	ToggleHUD(false);
	ToggleSpectate(false);
	ToggleTabMenu(false);
	TogglePostRound(true);
}

void ANSHUD::PlayerAlive()
{
	ToggleHUD(true);
	ToggleSpectate(false);
}

void ANSHUD::PlayerDead_Implementation(class APCh* InstigatorCharecter, class AActor* DeathCauser, class APS* PS)
{
	ToggleHUD(false);
	ToggleSpectate(true);
}
