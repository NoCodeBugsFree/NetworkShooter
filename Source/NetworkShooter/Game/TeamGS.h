// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Game/GS.h"
#include "TeamGS.generated.h"

/**
 * 
 */
UCLASS()
class NETWORKSHOOTER_API ATeamGS : public AGS
{
	GENERATED_BODY()
	
public:

	/** returns points for this team  */
	UFUNCTION(BlueprintCallable, Category = "UI")
	int32 GetPoints(ETeam Team) const;

	/** called to add some points to team points */
	UFUNCTION(BlueprintCallable, Category = "UI")
	void AddTeamPoints(ETeam Team, int32 Value);

	/** returns the list of player states for specified team  */
	UFUNCTION(BlueprintCallable, Category = "UI")
	TArray<class ATeamPS*> GetTeamPlayers(ETeam Team) const;

private:
	
	/** points of team A  */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	int32 TeamAPoints = 0;

	/** points of team B  */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	int32 TeamBPoints = 0;
};
