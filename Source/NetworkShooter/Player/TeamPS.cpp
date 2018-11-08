// Fill out your copyright notice in the Description page of Project Settings.

#include "TeamPS.h"
#include "Net/UnrealNetwork.h"

void ATeamPS::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ATeamPS, Team);
}

void ATeamPS::SetTeam(ETeam NewTeam /*= ETeam::TeamA*/)
{
	ServerSetTeam(NewTeam);
}

void ATeamPS::ServerSetTeam_Implementation(ETeam NewTeam /*= ETeam::TeamA*/)
{
	Team = NewTeam;
}

