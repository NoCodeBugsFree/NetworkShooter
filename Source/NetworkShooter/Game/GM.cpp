// Fill out your copyright notice in the Description page of Project Settings.

#include "GM.h"
#include "Game/GS.h"
#include "Player/PS.h"
#include "UI/NSHUD.h"
#include "NetworkShooterGameSession.h"
#include "Kismet/GameplayStatics.h"
#include "Game/NSPlayerStart.h"
#include "Game/Spectators/FollowSpectator.h"
#include "Game/Spectators/FreeSpectator.h"
#include "ConstructorHelpers.h"

AGM::AGM()
{
	bUseSeamlessTravel = true;
	bStartPlayersAsSpectators = true;

	GameSessionClass = ANetworkShooterGameSession::StaticClass();
	GameStateClass = AGS::StaticClass();
	PlayerControllerClass = APC::StaticClass();
	PlayerStateClass = APS::StaticClass();
	HUDClass = ANSHUD::StaticClass();
	
	/**  BP Pawn asset */
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/BP/Player/BP_PCh"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
		
	/** class defaults  */
	bAutoRespawn = true;
	bAllowFastRespawn = true;
	bLimitByTime = false;
	bAllowJoinInProgress = true;
	FollowSpectatorClass = AFollowSpectator::StaticClass();
	FreeSpectatorClass = AFreeSpectator::StaticClass();
}

void AGM::BeginPlay()
{
	Super::BeginPlay();
	
	if(AGS* GS = Cast<AGS>(GameState))
	{
		GS->OnPlayerConnected.AddDynamic(this, &AGM::PlayerConnected);
	}
}

void AGM::PlayerConnected(APS* PS)
{
	if (AGS* GS = Cast<AGS>(GameState))
	{
		if (GS->RoundInProgress == ERoundInProgress::GS_Game && bAllowJoinInProgress && PS)
		{
			if(APC* PC = Cast<APC>(PS->GetOwner()))
			{
				SpawnPlayer(PC);
			}
		}
	}
}

void AGM::SpawnPlayer(class APC* PC)
{
	APCh* SpawnedCharacter = nullptr;
	if (PC)
	{
		/** destroy spectator  */
		if (PC->GetPawn())
		{
			PC->GetPawn()->Destroy();
		}

		/** find free spawn points  */
		TArray<ANSPlayerStart*> FreeSpawnPoints = GetFreeSpawnPoints(PC);
		if (FreeSpawnPoints.Num() > 0)
		{
			int32 RandonIndex = FMath::RandRange(0, FreeSpawnPoints.Num() - 1);
			if (FreeSpawnPoints.IsValidIndex(RandonIndex))
			{
				ANSPlayerStart* NSPlayerStart = FreeSpawnPoints[RandonIndex];
				if (NSPlayerStart)
				{
					/** spawn character at random found spawn point  */
					SpawnedCharacter = NSPlayerStart->SpawnCharacter(PC);
					if(APS* PS = Cast<APS>(PC->PlayerState))
					{
						/** "resurrect" dead character  */
						PS->bAlive = true;
						PS->OnRep_Alive();
					}
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("SpawnPoint not valid"));
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("No Free spawn points"));
		}
	}
	if (SpawnedCharacter == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("SpawnPlayer failed!"));
	}
}

void AGM::CharacterDead(APCh* DeadCharacter, APC* DeathInstigatorPC, AActor* DeathCausedBy)
{
	/** DeathInstigatorPC can be null  */
	if (DeadCharacter && DeathCausedBy)
	{
		/** notify all subscribers  */
		OnCharDead.Broadcast(DeadCharacter, DeathInstigatorPC, DeathCausedBy);
		
		APC* DeadCharacterPC = Cast<APC>(DeadCharacter->GetController());
		
		UpdateDeathPoints(DeadCharacterPC, DeathInstigatorPC);

		/** all clients notify  */
		APS* DeadCharacterPS = Cast<APS>(DeadCharacter->PlayerState);
		if (DeadCharacterPS)
		{
			if (DeathInstigatorPC)
			{
				APCh* DeathInstigatorPlayerCharacter = Cast<APCh>(DeathInstigatorPC->GetPawn());
				DeadCharacterPS->MulticastOnwerCharacterDead(DeathInstigatorPlayerCharacter, DeathCausedBy);
			}
		}
		
		/** TODO BUG  */
		/** auto respawn player */
		/*if (bAutoRespawn && DeadCharacterPC)
		{
			DeadCharacterPC->LaunchRespawnTimer(RespawnTime);
		}*/

		/** respawn by timer  */
		if (bAutoRespawn && DeadCharacterPC)
		{
			/** set respawn timer manually   */
			FTimerDelegate RespawnDelegate;
			FTimerHandle DeadPlayerRespawnTimer;
			RespawnDelegate.BindUFunction(this, FName("SpawnPlayer"), DeadCharacterPC); 
			GetWorldTimerManager().SetTimer(DeadPlayerRespawnTimer, RespawnDelegate, RespawnTime, false);
		}
		
		/** spawn temporarily spectator  */
		SpawnSpectator(DeadCharacterPC, DeathInstigatorPC);
	}
}

void AGM::UpdateDeathPoints(class APC* DeadCharacterPC, class APC* DeathInstigatorPC)
{
	/** update victim deaths  */
	if (DeadCharacterPC)
	{
		if(APS* DeadCharacterPS = Cast<APS>(DeadCharacterPC->PlayerState))
		{
			DeadCharacterPS->AddDeath();
		}	
	}

	/** update killers kills  */
	if (DeathInstigatorPC)
	{
		if (APS* DeathInstigatorPS = Cast<APS>(DeathInstigatorPC->PlayerState))
		{
			DeathInstigatorPS->AddKill();
		}
	}
}

AGameSpectator* AGM::SpawnSpectator(class APC* DeadCharacterPC, class APC* InstigatorPC)
{
	AGameSpectator* Result = nullptr;

	/** free or follow spectator  */
	if (InstigatorPC) /** follow spectator */
	{
		if (FollowSpectatorClass)
		{
			UWorld* const World = GetWorld();
			if (World)
			{
				FActorSpawnParameters SpawnParams;
				SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				
				FTransform SpawnTransform;
				if (DeadCharacterPC && DeadCharacterPC->GetPawn())
				{
					SpawnTransform = DeadCharacterPC->GetPawn()->GetActorTransform();
				}
				
				AFollowSpectator* SpawnedFollowSpectator = World->SpawnActor<AFollowSpectator>(FollowSpectatorClass, SpawnTransform, SpawnParams);
				if (SpawnedFollowSpectator)
				{
					SpawnedFollowSpectator->FollowPawn = InstigatorPC->GetPawn() ? InstigatorPC->GetPawn() : nullptr;
					SpawnedFollowSpectator->PC = DeadCharacterPC;
					Result = SpawnedFollowSpectator;
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("FollowSpectatorClass == NULL"));
		}
	}
	else /** free spectator */
	{
		if (FreeSpectatorClass)
		{
			UWorld* const World = GetWorld();
			if (World)
			{
				FActorSpawnParameters SpawnParams;
				SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				
				FTransform SpawnTransform;
				
				AFreeSpectator* SpawnedFreeSpectator = World->SpawnActor<AFreeSpectator>(FreeSpectatorClass, SpawnTransform, SpawnParams);
				if (SpawnedFreeSpectator)
				{
					SpawnedFreeSpectator->PC = DeadCharacterPC;
					Result = SpawnedFreeSpectator;
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("FreeSpectatorClass == NULL"));
		}
	}

	if (Result && DeadCharacterPC)
	{
		DeadCharacterPC->Possess(Result);
	}

	return Result;
}

TArray<ANSPlayerStart*> AGM::GetFreeSpawnPoints(APC* PC) const
{
	TArray<AActor*> SpawnPoints;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ANSPlayerStart::StaticClass(), SpawnPoints);

	TArray<ANSPlayerStart*> Result;
	for (AActor* SpawnPoint : SpawnPoints)
	{
		ANSPlayerStart* TestSpawnPoint = Cast<ANSPlayerStart>(SpawnPoint);
		/** find and return all free spawn points  */
		if (TestSpawnPoint && TestSpawnPoint->IsFree())
		{
			Result.AddUnique(TestSpawnPoint);
		}
	}

	return Result;
}

void AGM::StartRound()
{
	if (GameState)
	{
		for (APlayerState* PlayerState : GameState->PlayerArray)
		{
			if (PlayerState && PlayerState->GetOwner())
			{
				if (APC* PC = Cast<APC>(PlayerState->GetOwner()))
				{
					/** spawn player character for each valid PC  */
					SpawnPlayer(PC);
				}
			}
		}

		/** notify subscribers that round was started  */
		OnRoundStarted.Broadcast();
		UE_LOG(LogTemp, Warning, TEXT("Round Started >>> "));
	}
}

void AGM::StopRound(FString Reason)
{
	/** for each player - spawn spectator  */
	for (APlayerState* PS : GameState->PlayerArray)
	{
		if(APC* PC = Cast<APC>(PS->GetOwner()))
		{
			SpawnSpectator(PC, nullptr);
		}
	}
	
	/** remove all player pawns  */
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APCh::StaticClass(), FoundActors);

	for (AActor* Actor : FoundActors)
	{
		if(APCh* PCh = Cast<APCh>(Actor))
		{
			/** and their weapons  */
			PCh->GetWeaponManager()->DestroyAllWeapons();
			PCh->Destroy();
		}
	}
	/** notify subscribers that round was ended  */
	OnRoundEnded.Broadcast(Reason);
}

void AGM::TimeLimitReached()
{
	if (bLimitByTime)
	{
		StopRound("Time Limit");
	}
}

bool AGM::ShouldDamagePlayer(class APC* Victim, class APC* InstigatorPC) const
{	
	/** if instigator controller is null we should damage player   */
	return true;
}

bool AGM::IsAnotherTeam(AController* Victim, AController* InstigatorPC) const
{
	return true;
}
