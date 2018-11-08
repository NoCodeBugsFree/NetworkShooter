// Fill out your copyright notice in the Description page of Project Settings.

#include "GS.h"
#include "Net/UnrealNetwork.h"
#include "Game/GM.h"
#include "Components/Game/ChatComp.h"
#include "Components/Game/MapVoteComp.h"
#include "ConstructorHelpers.h"

AGS::AGS()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 1.f;
	PrimaryActorTick.bStartWithTickEnabled = false;
	
	/** chat component  */
	ChatComp = CreateDefaultSubobject<UChatComp>(TEXT("ChatComp"));

	/** map vote component  */
	MapVoteComp = CreateDefaultSubobject<UMapVoteComp>(TEXT("MapVoteComp"));

	/** set the data table asset  */
	static ConstructorHelpers::FObjectFinder<UDataTable> DataTableAsset(TEXT("/Game/BP/Data/DataTables/Maps"));
	if (DataTableAsset.Object)
	{
		MapsDT = DataTableAsset.Object;
	}
}

void AGS::BeginPlay()
{
	Super::BeginPlay();

	if (Role == ROLE_Authority)
	{
		if (AGM* TestGM = Cast<AGM>(GetWorld()->GetAuthGameMode()))
		{
			GM = TestGM;
			GM->OnRoundEnded.AddDynamic(this, &AGS::RoundEnded);
		}
	}

	/** start ticking */
	SetActorTickEnabled(true);
}

void AGS::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	/** 1 sec tick  */
	if (Role == ROLE_Authority)
	{
		if (RoundInProgress == ERoundInProgress::GS_PreStart)
		{
			if (IsEnoughPlayersToStartRound())
			{
				/** ticking for starting a round  */
				RoundStartTime--;
				if (RoundStartTime <= 0)
				{
					if (GM)
					{
						/** starting new round  */
						GM->StartRound();
						RoundInProgress = ERoundInProgress::GS_Game;
						OnRep_RoundInProgress();
					}
				}
			} 
			else
			{
				/** waiting for required amount of players to start the round  */
				RoundStartTime = GetDefault<AGS>(GetClass())->RoundStartTime;
			}
		} 
		else
		{	/* per sec time tick, updates replicated min, sec, checks for round finishing */
			RoundTimeTick();
		}
	}
}

void AGS::VoteEnded(FName VotedMap)
{
	if (MapsDT)
	{
		TArray<FName> RowNames;
		RowNames = MapsDT->GetRowNames();
		FString ContextString;

		for (FName& RowName : RowNames)
		{
			FMapInfo_* MapInfo = MapsDT->FindRow<FMapInfo_>(RowName, ContextString);
			if (MapInfo)
			{
				TravelServerToMap(MapInfo->MapName);

				/**   */
				break;
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("MapsDT == NULL!"));
	}
}

void AGS::MulticastRoundEnded_Implementation(const FString& Reason)
{
	OnRoundEnded.Broadcast(Reason);
}

void AGS::RoundEnded(FString Reason)
{
	MulticastRoundEnded(Reason);
	RoundInProgress = ERoundInProgress::GS_RoundEnd;
	OnRep_RoundInProgress();
	MapVoteComp->StartVote();
	MapVoteComp->OnVoteEnded.AddDynamic(this, &AGS::VoteEnded);
}

void AGS::OnRep_RoundInProgress()
{
	if (RoundInProgress == ERoundInProgress::GS_Game)
	{
		OnRoundStarted.Broadcast();
	}
}

void AGS::StartRoundTimer()
{
	GetWorldTimerManager().SetTimer(TimeTickTimer, this, &AGS::RoundTimeTick, 1.f, true);
}

void AGS::RoundTimeTick()
{
	RoundTime += FTimespan(0, 0, 1);
	Min = RoundTime.GetMinutes();
	Sec = RoundTime.GetSeconds();
	if ( GM && (RoundTime >= GM->TimeLimit) && (RoundInProgress == ERoundInProgress::GS_Game) )
	{
		GM->TimeLimitReached();
		RoundInProgress = ERoundInProgress::GS_RoundEnd;
		OnRep_RoundInProgress();
	}
}

void AGS::NewPlayerConnected(class APS* PS)
{
	if (PS)
	{
		FString PlayerName = FString(PS->GetPlayerName());
		UE_LOG(LogTemp, Error, TEXT("%s connected..."), *PlayerName);
		OnPlayerConnected.Broadcast(PS);
	}
}

bool AGS::IsEnoughPlayersToStartRound() const
{
	return PlayerArray.Num() >= PlayersToStart;
}

void AGS::TravelServerToMap(FName Map)
{
	if (Role == ROLE_Authority)
	{
		/** travel to voted map  */
		FString MapName;
		Map.ToString(MapName);
		GetWorld()->ServerTravel(MapName);
	}
}

void AGS::GetRoundTime(int32& RoundMin, int32& RoundSec) const
{
	RoundMin = Min;
	RoundSec = Sec;
}

void AGS::GetPreRoundTime(int32& PreRoundTime) const
{
	PreRoundTime = RoundStartTime;
}

void AGS::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGS, RoundInProgress);
	DOREPLIFETIME(AGS, Min);
	DOREPLIFETIME(AGS, Sec);
	DOREPLIFETIME(AGS, RoundStartTime);
}
