// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "PS.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerAlive);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnPlayerDead, class APCh*, InstigatorCharecter, class AActor*, DeathCauser, class APS*, PS);

/**
 * 
 */
UCLASS()
class NETWORKSHOOTER_API APS : public APlayerState
{
	GENERATED_BODY()
	
public:

	/* called to notify subscribers that player was dead  */
	UFUNCTION(NetMulticast, Reliable) // Unreliable
	void MulticastOnwerCharacterDead(class APCh* InstigatorCh, class AActor* DeathCauser);
	void MulticastOnwerCharacterDead_Implementation(class APCh* InstigatorCh, class AActor* DeathCauser);

	/** called to add 1 death to Deaths */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void AddDeath();

	/** called to add 1 kill to Kills */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void AddKill();

	/* Called whenever bAlive updated */
	UFUNCTION()
	virtual void OnRep_Alive();

	/** shows whether owner is alive or not  */
	UPROPERTY(ReplicatedUsing = OnRep_Alive, BlueprintReadOnly, Category = "Config")
	bool bAlive = true;
	
	/** broadcasts whenever owner player is alive */
	UPROPERTY(BlueprintAssignable, Category = "EventDispatcher")
	FOnPlayerAlive OnPlayerAlive;

	/** broadcasts whenever owner player is dead */
	UPROPERTY(BlueprintAssignable, Category = "EventDispatcher")
	FOnPlayerDead OnPlayerDead;

protected:

	virtual void BeginPlay() override;

	/** shows how many people was killed by owner player  */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	int32 Kills = 0;

	/** shows how many times owner player was killed */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	int32 Deaths = 0;

public:
	FORCEINLINE int32 GetKills() const { return Kills; }
	FORCEINLINE int32 GetDeaths() const { return Deaths; }

};
