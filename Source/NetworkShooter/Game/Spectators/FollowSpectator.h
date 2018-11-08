// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Game/Spectators/GameSpectator.h"
#include "FollowSpectator.generated.h"

/**
 * 
 */
UCLASS()
class NETWORKSHOOTER_API AFollowSpectator : public AGameSpectator
{
	GENERATED_BODY()
	
	/** camera boom  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;
	
	/* camera component  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* CameraComponent;
	
public:

	AFollowSpectator();
	
	/** pawn we want to follow  */
	UPROPERTY(Transient)
	class APawn* FollowPawn;

protected:

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;
	
	/** follow interpolaton speed  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	float Speed = 5.f;
};
