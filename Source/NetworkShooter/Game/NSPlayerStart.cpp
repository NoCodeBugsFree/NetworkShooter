// Fill out your copyright notice in the Description page of Project Settings.

#include "NSPlayerStart.h"
#include "Components/CapsuleComponent.h"
#include "ConstructorHelpers.h"

ANSPlayerStart::ANSPlayerStart(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	/**  pawn to spawn */
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/BP/Player/BP_PCh"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		CharacterClass = PlayerPawnBPClass.Class;
	}
}

bool ANSPlayerStart::IsFree() const
{
	TArray<AActor*> CollectedActors;
	GetCapsuleComponent()->GetOverlappingActors(CollectedActors);
	
	for (AActor* TestActor : CollectedActors)
	{
		if(APCh* PCh = Cast<APCh>(TestActor))
		{
			/** at least one character overlaps me  */
			return false;
		}
	}

	/** no overlaps  */
	return true;
}

class APCh* ANSPlayerStart::SpawnCharacter(APC* PC) const
{
	/** result  */
	APCh* SpawnedCharacter = nullptr;

	if (CharacterClass)
	{
		UWorld* const World = GetWorld();
		if (World)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = PC;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
			
			FTransform SpawnTransform = GetActorTransform();
			
			APCh* Spawned = World->SpawnActor<APCh>(CharacterClass, SpawnTransform, SpawnParams);
			if (Spawned)
			{
				SpawnedCharacter = Spawned;
				SpawnedCharacter->PlayerColor = Team == ETeam::TeamA ? FLinearColor::Red : FLinearColor::Blue;
				if (PC)
				{
					PC->Possess(SpawnedCharacter);
				}
				OnCharacterSpawned.Broadcast(SpawnedCharacter, PC);
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("CharacterClass == NULL"));
	}

	return SpawnedCharacter;
}
