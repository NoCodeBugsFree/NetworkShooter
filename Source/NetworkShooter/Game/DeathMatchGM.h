// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Game/GM.h"
#include "DeathMatchGM.generated.h"

/**
 * 
 */
UCLASS()
class NETWORKSHOOTER_API ADeathMatchGM : public AGM
{
	GENERATED_BODY()
	
public:

	ADeathMatchGM();

	/** handle game state after each kill  */
	virtual void CharacterDead(class APCh* DeadCharacter, class APC* DeathInstigatorPC, class AActor* DeathCausedBy) override;
	
private:

	/** shows whether we use kills limit or not */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	uint32 bLimitByKills : 1;

	/** kills limit for this match  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	int32 KillsLimit = 3;

};
