// Fill out your copyright notice in the Description page of Project Settings.

#include "MapVoteComp.h"
#include "Net/UnrealNetwork.h"
#include "ConstructorHelpers.h"
#include "Engine/DataTable.h"
#include "Game/GS.h"

UMapVoteComp::UMapVoteComp()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 1.f;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	
	/** set the data table asset  */
	static ConstructorHelpers::FObjectFinder<UDataTable> DataTableAsset(TEXT("/Game/BP/Data/DataTables/Maps"));
	if (DataTableAsset.Object)
	{
		VoteMapsDT = DataTableAsset.Object;
	}

	SetIsReplicated(true);

	bCanVoteSameMap = true;
}

void UMapVoteComp::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// 1 sec per tick !

	/** start ticking  */
	--MapVoteTime;

	/** vote time is over  */
	if (MapVoteTime <= 0)
	{
		StopVote();
	}
}

void UMapVoteComp::StartVote()
{
	/** start ticking  */
	SetComponentTickEnabled(true);

	/** called to fill the Maps list from data table */
	ChooseMapsToVote();

	/** replicate current state  */
	bVoteInProgress = true;
	OnRep_VoteInProgress();
}

void UMapVoteComp::StopVote()
{
	/** disable ticking  */
	SetComponentTickEnabled(false);
	
	/** replicate current state  */
	bVoteInProgress = false;
	OnRep_VoteInProgress();
}

void UMapVoteComp::ChooseMapsToVote()
{
	if (VoteMapsDT)
	{
		/** gather row names from data table  */
		TArray<FName> RowNames = VoteMapsDT->GetRowNames();
		
		/** TODO RowNames.Num() why NumberOfMaps ???s  */
		for (int Index = 0; Index < NumberOfMaps; Index++)
		{
			if (RowNames.Num() > 0)
			{
				int32 RandomIndex = FMath::RandRange(0, RowNames.Num() - 1);
				if (RowNames.IsValidIndex(RandomIndex))
				{
					/** TODO why random map?  */
					Maps.Add(RowNames[RandomIndex]);
					RowNames.RemoveAt(RandomIndex);
				}
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("VoteMapsDT == NULL!"));
	}
}

int32 UMapVoteComp::GetVotedMap() /** TODO BUG HERE  */
{
	TArray<int32> Votes;

	/** Make votes array  */
	for (int32 Index = 0; Index < Maps.Num(); Index++)
	{
		Votes.Add(GetMapVotes(Index));
	}

	/** Get Random max vote map  */
	TArray<int32> MaxVoteIndexes;

	/** add an index of max value  */
	if (Votes.Num() > 0)
	{
		int32 IndexOfMaxValue = 0;
		FMath::Max(Votes, &IndexOfMaxValue);
		MaxVoteIndexes.Add(IndexOfMaxValue);
	}
	
	for (int32 Index = 0; Index < Votes.Num(); Index++)
	{
		int32 ArrayElement = Votes[Index];

		/** max votes and not the same vote  */
		if (MaxVoteIndexes.Num() > 0 && Votes.IsValidIndex(MaxVoteIndexes[0]))
		{
			if (MaxVoteIndexes.IsValidIndex(0))
			{
				if (ArrayElement == Votes[MaxVoteIndexes[0]] && Index != MaxVoteIndexes[0])
				{
					MaxVoteIndexes.Add(Index);
				}
			}
		}
	}

	int32 Result = 0;
	if (MaxVoteIndexes.Num() > 0)
	{
		int32 RandomIndex = FMath::RandRange(0, MaxVoteIndexes.Num() - 1);
		int32 Result = MaxVoteIndexes[RandomIndex];
	}
	
	return Result;
}

void UMapVoteComp::SetVote(int32 MapIndex, class APS* PS)
{
	/** get GS  */
	if(AGS* GS = Cast<AGS>(GetWorld()->GetGameState()))
	{
		/** find voter's player state in PlayerArray  */
		int32 FoundIndex = GS->PlayerArray.Find(PS);
		if (FoundIndex != INDEX_NONE)
		{
			/** PlayerVotes.Num() cab be 0 here, we need to expand its size at least at @FoundIndex value   */
			if (!PlayerVotes.IsValidIndex(FoundIndex))
			{
				/** resize the player votes array  */
				PlayerVotes.SetNum(GS->PlayerArray.Num(), false);
			}

			PlayerVotes[FoundIndex] = MapIndex;
		}
	}
	
	/** update max voted map  */
	MaxVotedMap = GetVotedMap();
}

int32 UMapVoteComp::GetMapVotes(int32 MapIndex) const
{
	int32 Result = 0;
	for (int32 PlayerVote : PlayerVotes)
	{
		if (PlayerVote == MapIndex)
		{
			++Result;
		}
	}
	return Result;
}

FName UMapVoteComp::GetMaxVotedMap() const
{
	if (Maps.IsValidIndex(MaxVotedMap))
	{
		return Maps[MaxVotedMap];
	}
	return FName(TEXT("Map02"));
}

void UMapVoteComp::OnRep_Maps()
{
	OnMapsGererated.Broadcast(Maps);
}

void UMapVoteComp::OnRep_VoteInProgress()
{
	if (bVoteInProgress)
	{
		OnVoteStarted.Broadcast();
	} 
	else
	{
		OnVoteEnded.Broadcast(GetMaxVotedMap());
	}
}

void UMapVoteComp::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UMapVoteComp, MapVoteTime); 
	DOREPLIFETIME(UMapVoteComp, Maps);
	DOREPLIFETIME(UMapVoteComp, PlayerVotes);
	DOREPLIFETIME(UMapVoteComp, MaxVotedMap);
}
