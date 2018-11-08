// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Game/GM.h"
#include "TeamGM.generated.h"

/**
 * 
 */
UCLASS()
class NETWORKSHOOTER_API ATeamGM : public AGM
{
	GENERATED_BODY()
	
public:

	ATeamGM();

	/** called to game mode's handling of one player kill another to update death points, team points and so on */
	virtual void CharacterDead(class APCh* DeadCharacter, class APC* DeathInstigatorPC, class AActor* DeathCausedBy) override;
	
	/** called to spawn a player for each valid player controller connected and balance teams if specified */
	virtual void StartRound() override;

	/** define game mode rules according friendly fire and so on  */
	virtual bool ShouldDamagePlayer(class APC* Victim, class APC* InstigatorPC) const override;
	
	/** team point limit to reach to finish current round  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config")
	int32 TeamPointLimit = 2;

	/** shows whether we limit this round by team points or not */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config")
	uint32 bLimitByTeamPoints : 1;

	/** shows whether friendly fire is specified or not  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config")
	uint32 bFriendlyFire : 1;

	/** shows whether we balance unbalanced team before start the round or not  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config")
	uint32 bShouldBalanceTeams : 1;

protected:

	/** called to update deaths/kills in victim/killer player states  */
	virtual void UpdateDeathPoints(class APC* DeadCharacterPC, class APC* DeathInstigatorPC) override;

	/** called to spawn a player for new connected player ( bAllowJoinInProgress must be true) */
	virtual void PlayerConnected(class APS* PS) override;

	/** called to find all free spawn points */
	virtual TArray<class ANSPlayerStart*> GetFreeSpawnPoints(class APC* PC) const;

private:

	/** called to balance the teams, bShouldBalanceTeams must be true */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void BalanceTeams();

	/** shows whether we are at one team or not  */
	bool IsAnotherTeam(class AController* Victim, class AController* InstigatorPC) const;
		
};
