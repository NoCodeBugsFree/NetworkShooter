// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GameSpectator.generated.h"

UCLASS()
class NETWORKSHOOTER_API AGameSpectator : public APawn
{
	GENERATED_BODY()
	
protected:

	/* scene component  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class USceneComponent* Root;
	
	/** static mesh component  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* CameraMesh;
	
public:
	
	AGameSpectator();

	/** controller reference  */
	UPROPERTY(Transient)
	class APC* PC;

protected:

	/** called to look up/down  */
	void LookUp(float Value);

	/** called to turn around  */
	void Turn(float Value);
	
	virtual void BeginPlay() override;

public:	
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
};
