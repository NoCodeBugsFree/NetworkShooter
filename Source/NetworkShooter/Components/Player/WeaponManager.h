// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Weapons/Weapon.h"
#include "WeaponManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponSelected, class AWeapon*, Weapon);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable )
class NETWORKSHOOTER_API UWeaponManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	
	UWeaponManager();
	
	/** called to create a weapon and add it to inventory */
	UFUNCTION(BlueprintAuthorityOnly, BlueprintCallable, Category = "AAA")
	class AWeapon* CreateWeapon(TSubclassOf<class AWeapon> WeaponClass);

	/** called to select desired weapon  */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	class AWeapon* SelectWeapon(EWeaponType WeaponType, AWeapon* Weapon = nullptr, int32 Index = 0);
	
	/** called to scroll up/down a weapon */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	bool ScrollWeapon(bool bNext);

	/** called to gather all available weapons in to the array  */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void BuildWeaponsArray(TArray<AWeapon*> &Weapons);

	/** called to add weapon to inventory  */
	UFUNCTION(BlueprintAuthorityOnly, BlueprintCallable, Category = "AAA")
	void AddWeapon(class AWeapon* Weapon);

	/** called ask server to drop (destroy logic, not place in the world) */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void DropWeapon(class AWeapon* Weapon);
	
	/** returns whether we have this type of weapon or not  */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	bool HasWeaponByType(EWeaponType WeaponType) const;

	/** called to find weapon by specified class  */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	bool FindWeaponByClass(TSubclassOf<class AWeapon> WeaponClass, class AWeapon* FoundedWeapon = nullptr) const;

	/** called to update current weapon to a new one */
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void SetCurrentWeapon(class AWeapon* NewWeapon);

	/** called to destroy all our weapons when owner is dead */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void DestroyAllWeapons();

	/** broadcasts whenever new weapon is selected */
	UPROPERTY(BlueprintAssignable, Category = "EventDispatcher")
	FOnWeaponSelected OnWeaponSelected;

protected:
	
	virtual void BeginPlay() override;

private:

	/* called to actually drop a weapon */
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerDropWeapon(class AWeapon* Weapon);
	bool ServerDropWeapon_Validate(class AWeapon* Weapon) { return true; }
	void ServerDropWeapon_Implementation(class AWeapon* Weapon);

	/* called to actually select a new weapon  */
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSelectWeapon(EWeaponType WeaponType, AWeapon* SelectedWeapon, int32 Index);
	bool ServerSelectWeapon_Validate(EWeaponType WeaponType, AWeapon* SelectedWeapon, int32 Index) { return true; }
	void ServerSelectWeapon_Implementation(EWeaponType WeaponType, AWeapon* SelectedWeapon, int32 Index);

	/** replicated owner character reference  */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class APCh* PCh;

	/** current active weapon (can be nullptr) */
	UPROPERTY(ReplicatedUsing = OnRep_CurrentWeapon, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class AWeapon* CurrentWeapon;
	
	/* called whenever CurrentWeapon updated */
	UFUNCTION()
	virtual void OnRep_CurrentWeapon();

	/** main(rifles) weapon reference (can be nullptr) */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class AWeapon* MainWeapon;

	/** second(pistols) weapon reference (can be nullptr) */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class AWeapon* SecondWeapon;

	/** melee(knife) weapon reference (can be nullptr) */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class AWeapon* MeleeWeapon;

	/** client attached weapon reference  (can be nullptr) */
	UPROPERTY(BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class AWeapon* ClientWeaponAttached;

	/** list of special weapons (can be blank) */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	TArray<class AWeapon*> Specials;

	/** list of grenades (can be blank) */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	TArray<class AWeapon*> Grenades;
	
public:	
	/** returns current weapon **/
	FORCEINLINE class AWeapon* GetCurrentWeapon() const { return CurrentWeapon; }
	
};
