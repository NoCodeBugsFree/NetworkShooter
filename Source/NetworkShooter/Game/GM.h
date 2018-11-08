// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GM.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnCharDead, class APCh*, DeadCharacter, class APC*, DeathInstigatorC, class AActor*, DeathCauseBy);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRoundStartedGM);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRoundEndedGM, FString, Reason);

/**
 * 
 */
UCLASS(Abstract)
class NETWORKSHOOTER_API AGM : public AGameModeBase
{
	GENERATED_BODY()
	
public:

	AGM();

	/** define game mode rules according friendly fire and so on  */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	virtual bool ShouldDamagePlayer(class APC* Victim, class APC* InstigatorPC) const;

	/** called to game mode's handling of one player kill another to update death points, team points and so on */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	virtual void CharacterDead(class APCh* DeadCharacter, class APC* DeathInstigatorPC, class AActor* DeathCausedBy);
	
	/** called to spawn a player for each valid player controller connected */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	virtual void StartRound();

	/** shows whether players are in the same team or not  */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	bool IsAnotherTeam(AController* Victim, AController* InstigatorPC) const;

	/** called to stop the round if bLimitByTime = true  */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void TimeLimitReached();

	/** called to destroy spectator and spawn player character at free spawn point */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void SpawnPlayer(class APC* PC);

	/** broadcasts whenever character is dead */
	UPROPERTY(BlueprintAssignable, Category = "EventDispatcher")
	FOnCharDead OnCharDead;

	/** broadcasts whenever round is started */
	UPROPERTY(BlueprintAssignable, Category = "EventDispatcher")
	FOnRoundStartedGM OnRoundStarted;

	/** broadcasts whenever round is ended */
	UPROPERTY(BlueprintAssignable)
	FOnRoundEndedGM OnRoundEnded;

	/** time to play on this map ( bLimitByTime must be true) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	FTimespan TimeLimit = FTimespan(0, 0, 30);

	/** shows whether fast respawn is available or not  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	uint32 bAllowFastRespawn : 1;

protected:

	virtual void BeginPlay() override;

	/** called to spawn a player for new connected player ( bAllowJoinInProgress must be true) */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	virtual void PlayerConnected(class APS* PS);

	/** called to update deaths/kills in victim/killer player states  */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	virtual void UpdateDeathPoints(class APC* DeadCharacterPC, class APC* DeathInstigatorPC);

	/** called to spawn free or follow spectator */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	class AGameSpectator* SpawnSpectator(class APC* DeadCharacterPC, class APC* InstigatorPC);
	
	/** called to find all free spawn points */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	virtual TArray<class ANSPlayerStart*> GetFreeSpawnPoints(class APC* PC) const;

	/** called to spawn spectators, broadcast round end */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void StopRound(FString Reason);


	// -----------------------------------------------------------------------------------
	
	/** follow spectator class template  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config")
	TSubclassOf<class AFollowSpectator> FollowSpectatorClass;

	/** free spectator class template  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config")
	TSubclassOf<class AFreeSpectator> FreeSpectatorClass;
	
	/** shows whether auto respawn is available or not  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	uint32 bAutoRespawn : 1;

	/** shows whether this game is limited by time (TimeLimit) or not  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	uint32 bLimitByTime : 1;

	/** shows whether  we can join the game in its progress or not  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	uint32 bAllowJoinInProgress : 1;

	/** specifies respawn time for dead character (bAutoRespawn must be true) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	float RespawnTime = 3.f;
};
