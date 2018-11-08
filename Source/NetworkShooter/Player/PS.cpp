// Fill out your copyright notice in the Description page of Project Settings.

#include "PS.h"
#include "Net/UnrealNetwork.h"

void APS::BeginPlay()
{
	Super::BeginPlay();

	/** TODO reconnection bug  */

}

void APS::MulticastOnwerCharacterDead_Implementation(class APCh* InstigatorCh, class AActor* DeathCauser)
{
	bAlive = false;
	OnRep_Alive();
	OnPlayerDead.Broadcast(InstigatorCh, DeathCauser, this);
}

void APS::OnRep_Alive()
{
	if (bAlive)
	{
		OnPlayerAlive.Broadcast();
	} 
	else
	{
		OnPlayerDead.Broadcast(nullptr, nullptr, nullptr);
	}
}

void APS::AddDeath()
{
	Deaths++;
}

void APS::AddKill()
{
	Kills++;
}

void APS::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APS, bAlive);
	DOREPLIFETIME(APS, Kills);
	DOREPLIFETIME(APS, Deaths);
}