// Fill out your copyright notice in the Description page of Project Settings.

#include "TeamGM.h"
#include "Game/TeamGS.h"
#include "Player/TeamPS.h"
#include "UI/TeamNSHUD.h"
#include "Game/NSPlayerStart.h"

ATeamGM::ATeamGM()
{
	GameStateClass = ATeamGS::StaticClass();
	PlayerStateClass = ATeamPS::StaticClass();
	HUDClass = ATeamNSHUD::StaticClass();

	bLimitByTeamPoints = true;
	bShouldBalanceTeams = true;
	bFriendlyFire = false;
}

void ATeamGM::CharacterDead(class APCh* DeadCharacter, class APC* DeathInstigatorPC, class AActor* DeathCausedBy)
{
	Super::CharacterDead(DeadCharacter, DeathInstigatorPC, DeathCausedBy);

	if (bLimitByTeamPoints)
	{
		/** team game state required  */
		if (ATeamGS* TeamGS = Cast<ATeamGS>(GetWorld()->GetGameState()))
		{
			if (DeathInstigatorPC)
			{
				/** team player state required  */
				if (ATeamPS* TeamPS = Cast<ATeamPS>(DeathInstigatorPC->PlayerState))
				{
					/** killer's team has enough points to win  */
					if (TeamGS->GetPoints(TeamPS->GetTeam()) >= TeamPointLimit)
					{
						StopRound("TeamPointsLimit");
					}
				}
			} 
			else /** DeathInstigatorPC is null  */
			{
				if (DeadCharacter)
				{
					/** team player state required  */
					if (ATeamPS* TeamPS = Cast<ATeamPS>(DeadCharacter->PlayerState))
					{
						ETeam DeadCharacterTeam = TeamPS->GetTeam();
						ETeam DeathInstigatorTeam = DeadCharacterTeam == ETeam::TeamA ? ETeam::TeamB : ETeam::TeamA;

						/** killer's team has enough points to win  */
						if (TeamGS->GetPoints(DeathInstigatorTeam) >= TeamPointLimit)
						{
							StopRound("TeamPointsLimit");
						}
					}
				}
			}
		}
	} 
}

void ATeamGM::StartRound()
{
	BalanceTeams(); /** if bShouldBalanceTeams is true  */
	Super::StartRound();
}

bool ATeamGM::ShouldDamagePlayer(class APC* Victim, class APC* InstigatorPC) const
{
	/** friendly fire - always damage all players  */
	if (bFriendlyFire)
	{
		return true;
	}
	
	/** no friendly fire, check for team, don't damage teammate  */
	if (Victim && InstigatorPC)
	{
		ATeamPS* VictimTeamPS = Cast<ATeamPS>(Victim->PlayerState);
		ATeamPS* InstigatorTeamPS = Cast<ATeamPS>(InstigatorPC->PlayerState);
		if (VictimTeamPS && InstigatorTeamPS)
		{
			/** no friendly fire and same team  */
			if (VictimTeamPS->GetTeam() == InstigatorTeamPS->GetTeam())
			{
				return false;
			}
		}
	}
	
	return true;
}

void ATeamGM::UpdateDeathPoints(class APC* DeadCharacterPC, class APC* DeathInstigatorPC)
{
	Super::UpdateDeathPoints(DeadCharacterPC, DeathInstigatorPC);

	/** team game state required  */
	if (ATeamGS* TeamGS = Cast<ATeamGS>(GetWorld()->GetGameState()))
	{
		if (DeathInstigatorPC)
		{
			/** team player state required  */
			if (ATeamPS* TeamPS = Cast<ATeamPS>(DeathInstigatorPC->PlayerState))
			{
				/** add one point */
				if (IsAnotherTeam(DeadCharacterPC, DeathInstigatorPC))
				{
					TeamGS->AddTeamPoints(TeamPS->GetTeam(), 1);
				}
				else /** remove one point  */
				{
					TeamGS->AddTeamPoints(TeamPS->GetTeam(), -1);
				}
			}
		} 
		else /** DeathInstigatorPC is null add points to another team */
		{
			if (DeadCharacterPC)
			{
				if (ATeamPS* TeamPS = Cast<ATeamPS>(DeadCharacterPC->PlayerState))
				{
					TeamGS->AddTeamPoints(TeamPS->GetTeam(), 1);
				}
			}
		}
	}
}

void ATeamGM::PlayerConnected(class APS* PS)
{
	ATeamPS* TeamPS = Cast<ATeamPS>(PS);
	ATeamGS* TeamGS = Cast<ATeamGS>(GetWorld()->GetGameState());
	
	/** start playing only if allowed join in progress, spectating only otherwise  */
	if (bAllowJoinInProgress && TeamGS && TeamPS)
	{
		int32 PlayersInTeamA = TeamGS->GetTeamPlayers(ETeam::TeamA).Num();
		int32 PlayersInTeamB = TeamGS->GetTeamPlayers(ETeam::TeamB).Num();
		if (PlayersInTeamA != PlayersInTeamB)
		{
			/** find team which has less players and join to it  */
			ETeam NewTeam = PlayersInTeamA > PlayersInTeamB ? ETeam::TeamB : ETeam::TeamA;
			TeamPS->SetTeam(NewTeam);
		}
		Super::PlayerConnected(PS);
	}
}

TArray<class ANSPlayerStart*> ATeamGM::GetFreeSpawnPoints(APC* PC) const
{
	/** get all spawn points on the map  */
	TArray<AActor*> SpawnPoints;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ANSPlayerStart::StaticClass(), SpawnPoints);

	/** return value  */
	TArray<ANSPlayerStart*> Result;

	if (PC)
	{
		ATeamPS* TeamPS = Cast<ATeamPS>(PC->PlayerState);
		if (TeamPS)
		{
			for (AActor* SpawnPoint : SpawnPoints)
			{
				ANSPlayerStart* TestSpawnPoint = Cast<ANSPlayerStart>(SpawnPoint);
				
				/** spawn points is valid, is free and same team with player  */
				if (TestSpawnPoint && TestSpawnPoint->IsFree() && TestSpawnPoint->GetTeam() == TeamPS->GetTeam())
				{
					Result.AddUnique(TestSpawnPoint);
				}
			}
		}
	}

	return Result;
}

void ATeamGM::BalanceTeams()
{
	if (bShouldBalanceTeams)
	{
		ATeamGS* TeamGS = Cast<ATeamGS>(GetWorld()->GetGameState());
		if (TeamGS)
		{
			int32 PlayersInTeamA = TeamGS->GetTeamPlayers(ETeam::TeamA).Num();
			int32 PlayersInTeamB = TeamGS->GetTeamPlayers(ETeam::TeamB).Num();

			/** one team must have at least two more player to start balancing  */
			int32 Difference = FMath::Abs(PlayersInTeamB - PlayersInTeamA);
			if (Difference > 1)
			{
				/** we need to move only half of player's, otherwise we'll obtain the same balance, but for other team  */
				for (int Index = 0; Index < Difference / 2; Index++)
				{
					/** team to set (has less players now) */
					ETeam NewTeam = PlayersInTeamA > PlayersInTeamB ? ETeam::TeamB : ETeam::TeamA;

					/** another team (has more players now)  */
					ETeam OppositeTeam = PlayersInTeamA > PlayersInTeamB ? ETeam::TeamA : ETeam::TeamB;

					/** get all opposite team player states  */
					TArray<ATeamPS*> TeamPlayerStates = TeamGS->GetTeamPlayers(OppositeTeam);

					/** move random of them to another team  */
					if (TeamPlayerStates.Num() > 0)
					{
						int32 RandomIndex = FMath::RandRange(0, TeamPlayerStates.Num() - 1);
						if (TeamPlayerStates.IsValidIndex(RandomIndex))
						{
							ATeamPS* TeamPS = TeamPlayerStates[RandomIndex];
							if (TeamPS)
							{
								TeamPS->SetTeam(NewTeam);
							}
						}
					}
				}
			}
		}
	}
}

bool ATeamGM::IsAnotherTeam(class AController* Victim, class AController* InstigatorPC) const
{
	if (Victim && InstigatorPC)
	{
		ATeamPS* VictimTeamPS = Cast<ATeamPS>(Victim->PlayerState);
		ATeamPS* InstigatorTeamPS = Cast<ATeamPS>(InstigatorPC->PlayerState);
		if (VictimTeamPS && InstigatorTeamPS)
		{
			return VictimTeamPS->GetTeam() != InstigatorTeamPS->GetTeam();
		}
	}
	return false;
}
