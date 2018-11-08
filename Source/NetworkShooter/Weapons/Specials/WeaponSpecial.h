// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/Weapon.h"
#include "WeaponSpecial.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class NETWORKSHOOTER_API AWeaponSpecial : public AWeapon
{
	GENERATED_BODY()
	
public:

	AWeaponSpecial();

	/** called to ask server to start/stop using(primary ability) the weapon */
	virtual void ToggleUseWeapon(bool bUse) override;

	/** called whenever this weapon is drop to nullify owner and instigator */
	virtual void OnWeaponDropped() override;
	
	/**   */
	virtual void Destroyed() override;

	/** called to start using weapon */
	virtual bool UseWeapon() override;

protected:

	/** called to draw special weapon location to place in the world  */
	virtual void Tick(float DeltaSeconds) override;

	/** collision check and highlight special according can we place it here or not */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void HighlightSpecial(FVector &Start, FVector &End, FHitResult &Hit);

	/** called to spawn special in the world  */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void SpawnSpecial();

	/** special class to spawn  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class ASpecial> SpecialClass;

	/** spawned special reference  */
	UPROPERTY(BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class ASpecial* SpecialBeforeSet;

	/** maximum available distance to setup the special weapon   */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	float MaxDistanceToSetWeapon = 500.f;

	/** shows whether we can setup weapon right now or not*/
	UPROPERTY(BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	uint32 bCanSetupSpecial : 1;

private:
	
	/** called to spawn special weapon and set up it into the world */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void SetSpecial();
};
