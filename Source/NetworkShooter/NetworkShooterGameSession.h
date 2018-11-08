// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameSession.h"
#include "NetworkShooterGameSession.generated.h"

/**
 * 
 */
UCLASS()
class NETWORKSHOOTER_API ANetworkShooterGameSession : public AGameSession
{
	GENERATED_BODY()

private:

	/** Allow a dedicated server a chance to register itself with an online service */
	virtual void RegisterServer() override;
};
