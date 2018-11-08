// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Player/PS.h"
#include "TeamPS.generated.h"

/** team A or team B  */
UENUM(BlueprintType)
enum class ETeam : uint8
{
	TeamA,
	TeamB
};

/**
 * 
 */
UCLASS()
class NETWORKSHOOTER_API ATeamPS : public APS
{
	GENERATED_BODY()
	
public:

	/** asks server to set a team  */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void SetTeam(ETeam NewTeam = ETeam::TeamA);

private:

	/* server sets required team  */
	UFUNCTION(Server, Reliable, WithValidation) // Unreliable
	void ServerSetTeam(ETeam NewTeam = ETeam::TeamA);
	bool ServerSetTeam_Validate(ETeam NewTeam = ETeam::TeamA){ return true; }
	void ServerSetTeam_Implementation(ETeam NewTeam = ETeam::TeamA);
	
	/** our team  */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	ETeam Team = ETeam::TeamA;

public:
	FORCEINLINE ETeam GetTeam() const { return Team; }
};
