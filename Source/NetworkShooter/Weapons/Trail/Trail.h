// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Trail.generated.h"

UCLASS()
class NETWORKSHOOTER_API ATrail : public AActor
{
	GENERATED_BODY()
	
	/* scene component  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class USceneComponent* Root;
	
	/** spline mesh component  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class USplineMeshComponent* SplineMeshComponent;
	
public:	
	
	ATrail();
	
	/** spline mesh direction  */
	UPROPERTY(BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true", ExposeOnSpawn = true))
	FHitResult Hit;

protected:
	
	virtual void BeginPlay() override;

};
