// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Engine/DataTable.h"
#include "GS.generated.h"

/** current round state in progress  */
UENUM(BlueprintType)
enum class ERoundInProgress : uint8
{
	GS_PreStart UMETA(DisplayName = "PreStart"),
	GS_Game UMETA(DisplayName = "Game"),
	GS_RoundEnd UMETA(DisplayName = "RoundEnd")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTimeEnded);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRoundEndedGS, FString, Reason);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRoundStartedGS);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerConnected, class APS*, PS);

/** map info for all maps data table  */
USTRUCT(BlueprintType, Blueprintable)
struct FMapInfo_ : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AAA")
	FName MapName = "Map01";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AAA")
	UTexture2D* MapIcon;
};

/**
 * 
 */
UCLASS()
class NETWORKSHOOTER_API AGS : public AGameStateBase
{
	GENERATED_BODY()
	
	/** chat component  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class UChatComp* ChatComp;

	/** map vote component  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class UMapVoteComp* MapVoteComp;

	/** data table that represents all available maps in this game  */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class UDataTable* MapsDT;
	
public:

	AGS();

	/** returns round time(min, sec)  */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void GetRoundTime(int32& RoundMin, int32& RoundSec) const;

	/** returns pre round time(sec)  */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void GetPreRoundTime(int32& PreRoundTime) const;

	/** shows whether we can start round or need more connected players  */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	bool IsEnoughPlayersToStartRound() const;
	
	/** broadcasts whenever time ended */
	UPROPERTY(BlueprintAssignable, Category = "EventDispatcher")
	FOnTimeEnded OnTimeEnded;

	/** broadcasts whenever round ended */
	UPROPERTY(BlueprintAssignable, Category = "EventDispatcher")
	FOnRoundEndedGS OnRoundEnded;

	/** broadcasts whenever round started */
	UPROPERTY(BlueprintAssignable, Category = "EventDispatcher")
	FOnRoundStartedGS OnRoundStarted;

	/** broadcasts whenever new player connected */
	UPROPERTY(BlueprintAssignable, Category = "EventDispatcher")
	FOnPlayerConnected OnPlayerConnected;

	// -----------------------------------------------------------------------------------

	/** current round in progress state  */
	UPROPERTY(ReplicatedUsing = OnRep_RoundInProgress, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	ERoundInProgress RoundInProgress = ERoundInProgress::GS_PreStart;

	/** 1 sec round time tick timer  */
	UPROPERTY()
	FTimerHandle TimeTickTimer;

	/** represents current round time  */
	UPROPERTY(BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	FTimespan RoundTime;

	/** represents current round time(min) for replication  */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Config")
	int32 Min;

	/** represents current round time(sec) for replication  */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Config")
	int32 Sec;

	/** time to start new round when required amount of players are connected  */
	UPROPERTY(EditDefaultsOnly, Replicated, BlueprintReadOnly, Category = "Config")
	int32 RoundStartTime = 3;

	/** round starts min players value  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config")
	int32 PlayersToStart = 2;

	/** game mode reference  */
	UPROPERTY(Transient)
	class AGM* GM;

protected:

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;
	
private:
	
	/** called to find voted map in start traveling there */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void VoteEnded(FName VotedMap);

	/* broadcast all subscribers that round is ended */
	UFUNCTION(NetMulticast, Reliable) // Unreliable
	void MulticastRoundEnded(const FString& Reason);
	void MulticastRoundEnded_Implementation(const FString& Reason);

	/**  broadcast all subscribers that round is ended, start new map voting */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void RoundEnded(FString Reason);

	/* Called whenever RoundInProgress updated */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	virtual void OnRep_RoundInProgress();

	/* fires time tick timer */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void StartRoundTimer();

	/* per sec time tick, updates replicated min, sec, checks for round finishing */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void RoundTimeTick();

	/* broadcasts to subscribers that new player connected */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void NewPlayerConnected(class APS* PS);
	
	/* called to travel server to desired map */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void TravelServerToMap(FName Map);

public:
	FORCEINLINE class UChatComp* GetChatComp() const { return ChatComp; }
	FORCEINLINE class UMapVoteComp* GetMapVoteComp() const { return MapVoteComp; }
	
};
