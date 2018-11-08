// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/FireWeapon.h"
#include "SniperRifle.generated.h"

/**
 * 
 */
UCLASS()
class NETWORKSHOOTER_API ASniperRifle : public AFireWeapon
{
	GENERATED_BODY()
	
public:

	ASniperRifle();

protected:

	/** called to use secondary action if specified, adds sniper scope to the viewport */
	virtual void UseSecondaryAction() override;

	/** called to stop use secondary action if specified, removes sniper scope from the viewport */
	virtual void StopUseSecondaryAction() override;

private:

	/** widget class that represents sniper scope */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UUserWidget> SniperScopeWidgetClass;

	/** current sniper scope widget */
	UPROPERTY()
	class UUserWidget* CurrentWidget;
};
