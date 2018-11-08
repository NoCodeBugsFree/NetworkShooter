// Fill out your copyright notice in the Description page of Project Settings.

#include "TeamGS.h"
#include "Player/TeamPS.h"
#include "Net/UnrealNetwork.h"

int32 ATeamGS::GetPoints(ETeam Team) const
{
	if (Team == ETeam::TeamA)
	{
		return TeamAPoints;
	} 
	else
	{
		return TeamBPoints;
	}
}

void ATeamGS::AddTeamPoints(ETeam Team, int32 Value)
{
	if (Team == ETeam::TeamA)
	{
		TeamAPoints += Value;
	}
	else
	{
		TeamBPoints += Value;
	}
}

TArray<class ATeamPS*> ATeamGS::GetTeamPlayers(ETeam Team) const
{
	TArray<ATeamPS*> Result;
	
	for (APlayerState* PlayerState : PlayerArray)
	{
		if (ATeamPS* TeamPS = Cast<ATeamPS>(PlayerState))
		{
			/** team of player coincides with required  */
			if (TeamPS->GetTeam() == Team)
			{
				Result.Add(TeamPS);
			}
		}
	}

	return Result;
}

void ATeamGS::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATeamGS, TeamAPoints);
	DOREPLIFETIME(ATeamGS, TeamBPoints);
}