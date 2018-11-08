// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MapVoteComp.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnVoteStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnVoteEnded, FName, VotedMap);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMapsGererated, const TArray<FName>&, MapNames);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable)
class NETWORKSHOOTER_API UMapVoteComp : public UActorComponent
{
	GENERATED_BODY()

	/** Data table that represents all available maps in this game  */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class UDataTable* VoteMapsDT;
	
public:	
	
	UMapVoteComp();

	/** called to start vote next map when round ends */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void StartVote();

	/** called to set vote for our player */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void SetVote(int32 MapIndex, class APS* PS);

	/** returns the name of max voted map  */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	FName GetMaxVotedMap() const;

	/** returns votes for particular map (by index)*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "AAA")
	int32 GetMapVotes(int32 MapIndex) const;

	/** broadcasts whenever vote is started */
	UPROPERTY(BlueprintAssignable, Category = "EventDispatcher")
	FOnVoteStarted OnVoteStarted;

	/** broadcasts whenever vote is ended */
	UPROPERTY(BlueprintAssignable, Category = "EventDispatcher")
	FOnVoteEnded OnVoteEnded;

	/** broadcasts whenever map list generated for expose it to HUD */
	UPROPERTY(BlueprintAssignable, Category = "EventDispatcher")
	FOnMapsGererated OnMapsGererated;

private:

	/** called to finish the vote */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void StopVote();

	/** called to fill the Maps list from data table */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void ChooseMapsToVote();

	/** called to fill the Maps array from data table  */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	int32 GetVotedMap();

	// -----------------------------------------------------------------------------------

	/** summary voting time  */
	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	int32 MapVoteTime = 20;

	/** vote maps list lengths (TODO why not Maps.Num() ???) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	int32 NumberOfMaps = 2;

	/** shows whether we can vote for the same(previous) map or not  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	uint32 bCanVoteSameMap: 1;
	
	/** generated map list from data table  */
	UPROPERTY(ReplicatedUsing = OnRep_Maps, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	TArray<FName> Maps;
	
	/* called whenever Maps updated */
	UFUNCTION()
	virtual void OnRep_Maps();

	/**   */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	TArray<int32> PlayerVotes;
	
	/** shows whether voting is in progress or not  */
	UPROPERTY(ReplicatedUsing = OnRep_VoteInProgress, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	uint32 bVoteInProgress : 1;

	/* called whenever VoteInProgress updated */
	UFUNCTION()
	virtual void OnRep_VoteInProgress();

	/** index of next potential map  */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	int32 MaxVotedMap = 0;

public:	
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

};
