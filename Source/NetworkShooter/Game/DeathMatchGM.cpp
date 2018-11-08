// Fill out your copyright notice in the Description page of Project Settings.

#include "DeathMatchGM.h"

ADeathMatchGM::ADeathMatchGM()
{
	bLimitByKills = true;
	TimeLimit = FTimespan(0, 0, 10);
}

void ADeathMatchGM::CharacterDead(class APCh* DeadCharacter, class APC* DeathInstigatorPC, class AActor* DeathCausedBy)
{
	Super::CharacterDead(DeadCharacter, DeathInstigatorPC, DeathCausedBy);

	/** check for kills limit  */
	if (DeathInstigatorPC)
	{
		if (APS* PS = Cast<APS>(DeathInstigatorPC->PlayerState))
		{
			if (PS->GetKills() >= KillsLimit)
			{
				StopRound("KillLimit");
			}
		}
	}
	else/** TODO handle this  */
	{
		UE_LOG(LogTemp, Error, TEXT("DeathInstigatorPC is null"));
	}
}
