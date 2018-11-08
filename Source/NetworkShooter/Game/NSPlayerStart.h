// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerStart.h"
#include "Player/TeamPS.h"
#include "NSPlayerStart.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCharacterSpawned, class APCh*, PCh, class APC*, PC);

/**
 * 
 */
UCLASS()
class NETWORKSHOOTER_API ANSPlayerStart : public APlayerStart
{
	GENERATED_BODY()
	
public:

	ANSPlayerStart(const FObjectInitializer& ObjectInitializer);

	/** returns whether other player character overlaps this actor or not  */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	bool IsFree() const;

	/** called to spawn a new player character  */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	class APCh* SpawnCharacter(class APC* PC) const;

	/** broadcasts whenever new character is spawned */
	UPROPERTY(BlueprintAssignable, Category = "EventDispatcher")
	FOnCharacterSpawned OnCharacterSpawned;

private:

	/** character class to spawn at this spawn point  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class APCh> CharacterClass;
	
	/** team of this spawn point, use for spawn only required team characters  */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	ETeam Team = ETeam::TeamA;

public:
	
	/** returns the team of this spawn point  */
	FORCEINLINE ETeam GetTeam() const { return Team; }
	
};
